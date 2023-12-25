#include <cmath>    // std::abs
#include <cstdint>  // std::int64_t
#include <limits>   // std::numeric_limits
#include <string>   // std::string
#include <variant>  // std::variant std::visit

#include "libjsonpath/exceptions.hpp"
#include "libjsonpath/jsonpath.hpp"
#include "libjsonpath/node.hpp"
#include "libjsonpath/path.hpp"
#include "libjsonpath/selectors.hpp"

namespace py = pybind11;

namespace libjsonpath {

using namespace std::string_literals;
using expression_rv = std::variant<JSONPathNodeList, py::object>;

// Convert negative indicies to their positive equivalents given
// an "array" length.
size_t normalized_index(size_t length, std::int64_t index, const Token& token) {
  if (index >= 0) {
    return static_cast<size_t>(index);
  }

  if (length > static_cast<size_t>(std::numeric_limits<std::int64_t>::max())) {
    // TODO: better exception
    throw Exception("array index out of range", token);
  }

  std::int64_t positive_index = length + index;
  return (positive_index >= 0) ? static_cast<size_t>(positive_index) : length;
}

// JSONPath expression result truthiness test.
bool is_truthy(const expression_rv& rv) {
  if (std::holds_alternative<JSONPathNodeList>(rv)) {
    auto nodes = std::get<JSONPathNodeList>(rv);
    return !nodes.empty();
  }

  auto value{std::get<py::object>(rv)};
  return !(py::isinstance<py::bool_>(value) && !value.cast<py::bool_>());
}

// Visit every object with _node.value_ at the root.
void descend(const JSONPathNode& node, std::vector<JSONPathNode>& out_nodes) {
  out_nodes.push_back(node);
  if (py::isinstance<py::dict>(node.value)) {
    auto obj{py::cast<py::dict>(node.value)};
    for (auto item : obj) {
      py::str key{item.first};
      py::object val = py::cast<py::object>(item.second);
      location_t location{node.location};
      location.push_back(item.first.cast<std::string>());
      descend({val, location}, out_nodes);
    }
  } else if (py::isinstance<py::list>(node.value)) {
    auto obj{py::cast<py::list>(node.value)};
    size_t index{0};
    for (auto item : obj) {
      py::object val = py::cast<py::object>(item);
      location_t location{node.location};
      location.push_back(index);
      descend({val, location}, out_nodes);
      index++;
    }
  }
}

// Return a list of values from a node list, or a single value if
// the node list only has one item.
py::object values_or_singular(const JSONPathNodeList& nodes) {
  if (nodes.size() == 1) {
    return nodes[0].value;
  }

  py::list values{};
  for (auto node : nodes) {
    values.append(node.value);
  }

  return values;
}

class QueryContext {
public:
  QueryContext(py::object root_, py::dict functions_, py::object nothing_);

  const py::object root;
  const py::dict functions;
  const py::object nothing;
};

// Contextual objects a JSONPath filter will operate on.
struct FilterContext {
  const QueryContext& query;
  py::object current;
};

QueryContext::QueryContext(py::object root_, py::dict functions_,
                           py::object nothing_)
    : root{root_}, functions{functions_}, nothing{nothing_} {}

class ExpressionVisitor {
private:
  const FilterContext& m_context;

public:
  ExpressionVisitor(const FilterContext& filter_context)
      : m_context{filter_context} {}

  ~ExpressionVisitor() = default;

  expression_rv operator()(const NullLiteral& expression) const {
    return py::none();
  }

  expression_rv operator()(const BooleanLiteral& expression) const {
    return py::bool_(expression.value);
  }

  expression_rv operator()(const IntegerLiteral& expression) const {
    return py::int_(expression.value);
  }

  expression_rv operator()(const FloatLiteral& expression) const {
    return py::float_(expression.value);
  }

  expression_rv operator()(const StringLiteral& expression) const {
    return py::str(expression.value);
  }

  expression_rv operator()(const Box<LogicalNotExpression>& expression) const {
    return py::bool_(!is_truthy(std::visit(*this, expression->right)));
  }

  expression_rv operator()(const Box<InfixExpression>& expression) const {
    // Unpack single value node list.
    expression_rv left{std::visit(*this, expression->left)};
    if (std::holds_alternative<JSONPathNodeList>(left)) {
      auto left_ = std::get<JSONPathNodeList>(left);
      if (left_.size() == 1) {
        left = left_[0].value;
      }
    }

    // Unpack single value node list.
    expression_rv right{std::visit(*this, expression->right)};
    if (std::holds_alternative<JSONPathNodeList>(right)) {
      auto right_ = std::get<JSONPathNodeList>(right);
      if (right_.size() == 1) {
        right = right_[0].value;
      }
    }

    if (expression->op == BinaryOperator::logical_and) {
      return py::bool_(is_truthy(left) && is_truthy(right));
    }

    if (expression->op == BinaryOperator::logical_or) {
      return py::bool_(is_truthy(left) || is_truthy(right));
    }

    return py::bool_(compare(left, expression->op, right));
  }

  expression_rv operator()(const Box<RelativeQuery>& expression) const {
    return query(expression->query, m_context.current,
                 m_context.query.functions, m_context.query.nothing);
  }

  expression_rv operator()(const Box<RootQuery>& expression) const {
    return query(expression->query, m_context.query.root,
                 m_context.query.functions, m_context.query.nothing);
  }

  expression_rv operator()(const Box<FunctionCall>& expression) const {
    if (!m_context.query.functions.contains(py::str(expression->name))) {
      throw Exception(  // TODO: better exception
          "undefined filter function '"s + std::string(expression->name) + "'"s,
          expression->token);
    }

    FunctionExtension func_ext{
        m_context.query.functions[py::str(expression->name)]
            .cast<FunctionExtension>()};

    py::list args{};
    size_t index = 0;

    for (auto arg : expression->args) {
      expression_rv arg_rv{std::visit(*this, arg)};
      if (std::holds_alternative<JSONPathNodeList>(arg_rv)) {
        auto nodes{std::get<JSONPathNodeList>(arg_rv)};
        // Is the parameter expected a node list of values?
        // XXX: assumes the function call has already been validated.
        if (func_ext.args[index] != ExpressionType::nodes) {
          args.append(values_or_singular(nodes));
        } else {
          py::list node_list = py::cast(nodes);
          args.append(node_list);
        }
      } else {
        args.append(std::get<py::object>(arg_rv));
      }

      index++;
    }

    auto rv{func_ext.func(*args)};
    if (func_ext.res == ExpressionType::nodes) {
      // TODO: catch exception.
      return rv.cast<JSONPathNodeList>();
    }
    return rv;
  }

private:
  bool compare(const expression_rv& left, BinaryOperator op,
               const expression_rv& right) const {
    switch (op) {
      case BinaryOperator::eq:
        return equals(left, right);
      case BinaryOperator::ne:
        return !equals(left, right);
      case BinaryOperator::lt:
        return less_than(left, right);
      case BinaryOperator::gt:
        return less_than(right, left);
      case BinaryOperator::ge:
        return less_than(right, left) || equals(left, right);
      case BinaryOperator::le:
        return less_than(left, right) || equals(left, right);
      default:
        return false;
    }
  }

  bool equals(const expression_rv& left_, const expression_rv& right_) const {
    if (std::holds_alternative<JSONPathNodeList>(left_)) {
      return node_list_equals(std::get<JSONPathNodeList>(left_), right_);
    }

    if (std::holds_alternative<JSONPathNodeList>(right_)) {
      return node_list_equals(std::get<JSONPathNodeList>(right_), left_);
    }

    // Both left and right are py objects.
    py::object left{std::get<py::object>(left_)};
    py::object right{std::get<py::object>(right_)};

    // TODO: is this needed?
    // if (left.equal(m_filter_context.query.nothing) &&
    //     right.equal(m_filter_context.query.nothing)) {
    //   return true;
    // }

    return left.equal(right);
  }

  bool node_list_equals(const JSONPathNodeList& left,
                        const expression_rv& right_) const {
    if (std::holds_alternative<py::object>(right_)) {
      py::object right{std::get<py::object>(right_)};

      // left is an empty node list and right is NOTHING.
      if (left.empty()) {
        return right.equal(m_context.query.nothing);
      }

      // left is a single element node list, compare the node's value to right.
      if (left.size() == 1) {
        return left[0].value.equal(right);
      }

      return false;
    }

    // left and right are node lists.
    JSONPathNodeList right{std::get<JSONPathNodeList>(right_)};

    // Are both lists are empty?
    if (left.empty() && right.empty()) {
      return true;
    }

    // Do both lists have a single node?
    if (left.size() == 1 && right.size() == 1) {
      return left[0].value.equal(right[0].value);
    }

    return false;
  }

  bool less_than(const expression_rv& left_,
                 const expression_rv& right_) const {
    if (std::holds_alternative<JSONPathNodeList>(left_) ||
        std::holds_alternative<JSONPathNodeList>(right_)) {
      return false;
    }

    py::object left{std::get<py::object>(left_)};
    py::object right{std::get<py::object>(right_)};

    if (py::isinstance<py::bool_>(left) || py::isinstance<py::bool_>(right)) {
      return false;
    }

    if (py::isinstance<py::str>(left) && py::isinstance<py::str>(right)) {
      return left < right;
    }

    if (py::isinstance<py::int_>(left) && py::isinstance<py::int_>(right)) {
      return left < right;
    }

    if (py::isinstance<py::int_>(left) && py::isinstance<py::float_>(right)) {
      return left < right;
    }

    if (py::isinstance<py::float_>(left) && py::isinstance<py::float_>(right)) {
      return left < right;
    }

    if (py::isinstance<py::float_>(left) && py::isinstance<py::int_>(right)) {
      return left < right;
    }

    return false;
  }
};

class SelectorVisitor {
private:
  const QueryContext& m_query_context;
  const JSONPathNode& m_node;
  std::vector<JSONPathNode>* m_out_nodes;

public:
  SelectorVisitor(const QueryContext& q_ctx, const JSONPathNode& node,
                  std::vector<JSONPathNode>* out_nodes)
      : m_query_context{q_ctx}, m_node{node}, m_out_nodes{out_nodes} {}

  ~SelectorVisitor() = default;

  void operator()(const NameSelector& selector) {
    if (py::isinstance<py::dict>(m_node.value)) {
      auto obj{py::cast<py::dict>(m_node.value)};
      py::str name{selector.name};
      if (obj.contains(name)) {
        py::object val{obj[name]};
        location_t location{m_node.location};
        location.push_back(selector.name);
        m_out_nodes->push_back(JSONPathNode{val, location});
      }
    }
  }

  void operator()(const IndexSelector& selector) {
    if (py::isinstance<py::list>(m_node.value)) {
      auto obj{py::cast<py::list>(m_node.value)};
      size_t len{py::len(obj)};
      auto index{normalized_index(len, selector.index, selector.token)};
      if (index >= 0 && index < len) {
        py::object val{obj[py::int_(index)]};
        location_t location{m_node.location};
        location.push_back(index);
        m_out_nodes->push_back(JSONPathNode{val, location});
      }
    }
  }

  void operator()(const WildSelector&) {
    if (py::isinstance<py::dict>(m_node.value)) {
      auto obj{py::cast<py::dict>(m_node.value)};
      for (auto item : obj) {
        py::str key{item.first};
        py::object val = py::cast<py::object>(item.second);
        location_t location{m_node.location};
        location.push_back(item.first.cast<std::string>());
        m_out_nodes->push_back(JSONPathNode{val, location});
      }
    } else if (py::isinstance<py::list>(m_node.value)) {
      auto obj{py::cast<py::list>(m_node.value)};
      size_t index{0};
      for (auto item : obj) {
        py::object val = py::cast<py::object>(item);
        location_t location{m_node.location};
        location.push_back(index);
        m_out_nodes->push_back(JSONPathNode{val, location});
        index++;
      }
    }
  }

  void operator()(const SliceSelector& selector) {
    if (py::isinstance<py::list>(m_node.value) && selector.step != 0) {
      auto obj{py::cast<py::list>(m_node.value)};
      size_t index{selector.start || 0};
      size_t step{selector.step || 1};
      py::slice slice{selector.start, selector.stop, selector.step};
      for (auto item : obj[slice]) {
        py::object val = py::cast<py::object>(item);
        auto norm_index{normalized_index(py::len(obj), index, selector.token)};
        location_t location{m_node.location};
        location.push_back(norm_index);
        m_out_nodes->push_back(JSONPathNode{val, location});
        index += step;
      }
    }
  }

  void operator()(const Box<FilterSelector>& selector) {
    if (py::isinstance<py::dict>(m_node.value)) {
      auto obj{py::cast<py::dict>(m_node.value)};
      for (auto item : obj) {
        py::object val = py::cast<py::object>(item.second);
        FilterContext filter_context{m_query_context, val};
        ExpressionVisitor visitor{filter_context};

        if (is_truthy(std::visit(visitor, selector->expression))) {
          location_t location{m_node.location};
          location.push_back(item.first.cast<std::string>());
          m_out_nodes->push_back({val, location});
        }
      }
    } else if (py::isinstance<py::list>(m_node.value)) {
      auto obj{py::cast<py::list>(m_node.value)};
      size_t index{0};
      for (auto item : obj) {
        py::object val = py::cast<py::object>(item);
        FilterContext filter_context{m_query_context, val};
        ExpressionVisitor visitor{filter_context};

        if (is_truthy(std::visit(visitor, selector->expression))) {
          location_t location{m_node.location};
          location.push_back(index);
          m_out_nodes->push_back({val, location});
        }

        index++;
      }
    }
  }
};

class SegmentVisitor {
private:
  const QueryContext& m_context;
  const std::vector<JSONPathNode>& m_nodes;
  std::vector<JSONPathNode>* m_out_nodes;

public:
  SegmentVisitor(const QueryContext& q_ctx,
                 const std::vector<JSONPathNode>& nodes,
                 std::vector<JSONPathNode>* out_nodes)
      : m_context{q_ctx}, m_nodes{nodes}, m_out_nodes{out_nodes} {}

  ~SegmentVisitor() = default;

  void operator()(const Segment& segment) {
    for (auto node : m_nodes) {
      SelectorVisitor visitor{m_context, node, m_out_nodes};
      for (auto selector : segment.selectors) {
        std::visit(visitor, selector);
      }
    }
  }

  void operator()(const RecursiveSegment& segment) {
    for (auto node : m_nodes) {
      std::vector<JSONPathNode> descendants{};
      descend(node, descendants);
      for (auto descendant : descendants) {
        SelectorVisitor visitor{m_context, descendant, m_out_nodes};
        for (auto selector : segment.selectors) {
          std::visit(visitor, selector);
        }
      }
    }
  }
};

JSONPathNodeList resolve_segment(
    const QueryContext& q_ctx, const JSONPathNodeList& nodes,
    const std::variant<libjsonpath::Segment, libjsonpath::RecursiveSegment>&
        segment) {
  JSONPathNodeList out_nodes{};
  SegmentVisitor visitor{q_ctx, nodes, &out_nodes};
  std::visit(visitor, segment);
  return out_nodes;
}

JSONPathNodeList query(const segments_t& segments, py::object obj,
                       py::dict functions, py::object nothing) {
  QueryContext q_ctx{obj, functions, nothing};
  // Bootstrap the node list with root object and an empty location.
  JSONPathNodeList nodes{{obj, {}}};
  for (auto segment : segments) {
    nodes = resolve_segment(q_ctx, nodes, segment);
  }
  // TODO: check for well-typed function extensions
  return nodes;
}

JSONPathNodeList query(std::string_view path, py::object obj,
                       py::dict functions, py::object nothing) {
  segments_t segments{parse(path)};
  QueryContext q_ctx{obj, functions, nothing};
  // Bootstrap the node list with root object and an empty location.
  JSONPathNodeList nodes{{obj, {}}};
  for (auto segment : segments) {
    nodes = resolve_segment(q_ctx, nodes, segment);
  }
  // TODO: check for well-typed function extensions
  return nodes;
}

}  // namespace libjsonpath