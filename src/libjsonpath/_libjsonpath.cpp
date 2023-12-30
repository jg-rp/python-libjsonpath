#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "libjsonpath/exceptions.hpp"
#include "libjsonpath/jsonpath.hpp"
#include "libjsonpath/lex.hpp"
#include "libjsonpath/node.hpp"
#include "libjsonpath/parse.hpp"
#include "libjsonpath/path.hpp"
#include "libjsonpath/selectors.hpp"
#include "libjsonpath/tokens.hpp"
#include "libjsonpath/utils.hpp"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<libjsonpath::JSONPathNode>);

PYBIND11_MODULE(_libjsonpath, m) {
  m.doc() = "JSONPath parser";

  auto base_exception =
      py::register_exception<libjsonpath::Exception>(m, "JSONPathException");

  py::register_exception<libjsonpath::LexerError>(m, "JSONPathLexerError",
                                                  base_exception.ptr());

  py::register_exception<libjsonpath::SyntaxError>(m, "JSONPathSyntaxError",
                                                   base_exception.ptr());

  py::register_exception<libjsonpath::TypeError>(m, "JSONPathTypeError",
                                                 base_exception.ptr());

  py::enum_<libjsonpath::TokenType>(m, "TokenType")
      .value("eof_", libjsonpath::TokenType::eof_)
      .value("and_", libjsonpath::TokenType::and_)
      .value("colon", libjsonpath::TokenType::colon)
      .value("comma", libjsonpath::TokenType::comma)
      .value("current", libjsonpath::TokenType::current)
      .value("ddot", libjsonpath::TokenType::ddot)
      .value("dq_string", libjsonpath::TokenType::dq_string)
      .value("eq", libjsonpath::TokenType::eq)
      .value("error", libjsonpath::TokenType::error)
      .value("false_", libjsonpath::TokenType::false_)
      .value("filter", libjsonpath::TokenType::filter_)
      .value("float_", libjsonpath::TokenType::float_)
      .value("func_", libjsonpath::TokenType::func_)
      .value("ge", libjsonpath::TokenType::ge)
      .value("gt", libjsonpath::TokenType::gt)
      .value("index", libjsonpath::TokenType::index)
      .value("int_", libjsonpath::TokenType::int_)
      .value("lbracket", libjsonpath::TokenType::lbracket)
      .value("le", libjsonpath::TokenType::le)
      .value("lparen", libjsonpath::TokenType::lparen)
      .value("lt", libjsonpath::TokenType::lt)
      .value("name", libjsonpath::TokenType::name_)
      .value("ne", libjsonpath::TokenType::ne)
      .value("not_", libjsonpath::TokenType::not_)
      .value("null_", libjsonpath::TokenType::null_)
      .value("or_", libjsonpath::TokenType::or_)
      .value("rbracket", libjsonpath::TokenType::rbracket)
      .value("root", libjsonpath::TokenType::root)
      .value("rparen", libjsonpath::TokenType::rparen)
      .value("sq_string", libjsonpath::TokenType::sq_string)
      .value("true_", libjsonpath::TokenType::true_)
      .value("wild", libjsonpath::TokenType::wild)
      .export_values();

  // TODO: better str and repr
  py::class_<libjsonpath::Token>(m, "Token")
      .def_readonly("type", &libjsonpath::Token::type)
      .def_readonly("value", &libjsonpath::Token::value)
      .def_readonly("index", &libjsonpath::Token::index)
      .def_readonly("query", &libjsonpath::Token::query)
      .def("__str__", [](const libjsonpath::Token& t) {
        return libjsonpath::token_to_string(t);
      });

  py::class_<libjsonpath::Lexer>(m, "Lexer")
      .def(py::init<std::string_view>())
      .def("run", &libjsonpath::Lexer::run)
      .def("tokens", &libjsonpath::Lexer::tokens);

  py::class_<libjsonpath::Parser>(m, "Parser")
      .def(py::init<>())
      .def(py::init<std::unordered_map<std::string,
                                       libjsonpath::FunctionExtensionTypes>>())
      .def("parse",
           py::overload_cast<const libjsonpath::Tokens&>(
               &libjsonpath::Parser::parse, py::const_),
           "Parse a JSONPath from a sequence of tokens",
           py::return_value_policy::move)

      .def("parse",
           py::overload_cast<std::string_view>(&libjsonpath::Parser::parse,
                                               py::const_),
           "Parse a JSONPath query string", py::return_value_policy::move);

  // TODO: __str__ for all of these..

  py::enum_<libjsonpath::BinaryOperator>(m, "BinaryOperator")
      .value("none", libjsonpath::BinaryOperator::none)
      .value("logical_and", libjsonpath::BinaryOperator::logical_and)
      .value("logical_or", libjsonpath::BinaryOperator::logical_or)
      .value("eq", libjsonpath::BinaryOperator::eq)
      .value("ge", libjsonpath::BinaryOperator::ge)
      .value("gt", libjsonpath::BinaryOperator::gt)
      .value("le", libjsonpath::BinaryOperator::le)
      .value("lt", libjsonpath::BinaryOperator::lt)
      .value("ne", libjsonpath::BinaryOperator::ne);

  py::class_<libjsonpath::NullLiteral>(m, "NullLiteral")
      .def_readonly("token", &libjsonpath::NullLiteral::token);

  py::class_<libjsonpath::BooleanLiteral>(m, "BooleanLiteral")
      .def_readonly("token", &libjsonpath::BooleanLiteral::token)
      .def_readonly("value", &libjsonpath::BooleanLiteral::value);

  py::class_<libjsonpath::IntegerLiteral>(m, "IntegerLiteral")
      .def_readonly("token", &libjsonpath::IntegerLiteral::token)
      .def_readonly("value", &libjsonpath::IntegerLiteral::value);

  py::class_<libjsonpath::FloatLiteral>(m, "FloatLiteral")
      .def_readonly("token", &libjsonpath::FloatLiteral::token)
      .def_readonly("value", &libjsonpath::FloatLiteral::value);

  py::class_<libjsonpath::StringLiteral>(m, "StringLiteral")
      .def_readonly("token", &libjsonpath::StringLiteral::token)
      .def_readonly("value", &libjsonpath::StringLiteral::value);

  py::class_<libjsonpath::Box<libjsonpath::LogicalNotExpression>>(
      m, "LogicalNotExpression")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::LogicalNotExpression>& e) {
            return e->token;
          })
      .def_property_readonly(
          "right",
          [](const libjsonpath::Box<libjsonpath::LogicalNotExpression>& e) {
            return e->right;
          });

  py::class_<libjsonpath::Box<libjsonpath::InfixExpression>>(m,
                                                             "InfixExpression")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::InfixExpression>& e) {
            return e->token;
          })
      .def_property_readonly(
          "left",
          [](const libjsonpath::Box<libjsonpath::InfixExpression>& e) {
            return e->left;
          })
      .def_property_readonly(
          "op",
          [](const libjsonpath::Box<libjsonpath::InfixExpression>& e) {
            return e->op;
          })
      .def_property_readonly(
          "right", [](const libjsonpath::Box<libjsonpath::InfixExpression>& e) {
            return e->right;
          });

  py::class_<libjsonpath::Box<libjsonpath::RelativeQuery>>(m, "RelativeQuery")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::RelativeQuery>& e) {
            return e->token;
          })
      .def_property_readonly(
          "query", [](const libjsonpath::Box<libjsonpath::RelativeQuery>& e) {
            return e->query;
          });

  py::class_<libjsonpath::Box<libjsonpath::RootQuery>>(m, "RootQuery")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::RootQuery>& e) {
            return e->token;
          })
      .def_property_readonly(
          "query", [](const libjsonpath::Box<libjsonpath::RootQuery>& e) {
            return e->query;
          });

  py::class_<libjsonpath::Box<libjsonpath::FunctionCall>>(m, "FunctionCall")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::FunctionCall>& e) {
            return e->token;
          })
      .def_property_readonly(
          "name",
          [](const libjsonpath::Box<libjsonpath::FunctionCall>& e) {
            return e->name;
          })
      .def_property_readonly(
          "args", [](const libjsonpath::Box<libjsonpath::FunctionCall>& e) {
            return e->args;
          });

  py::class_<libjsonpath::NameSelector>(m, "NameSelector")
      .def_readonly("token", &libjsonpath::NameSelector::token)
      .def_readonly("name", &libjsonpath::NameSelector::name)
      .def_readonly("shorthand", &libjsonpath::NameSelector::shorthand);

  py::class_<libjsonpath::IndexSelector>(m, "IndexSelector")
      .def_readonly("token", &libjsonpath::IndexSelector::token)
      .def_readonly("index", &libjsonpath::IndexSelector::index);

  py::class_<libjsonpath::WildSelector>(m, "WildSelector")
      .def_readonly("token", &libjsonpath::WildSelector::token)
      .def_readonly("shorthand", &libjsonpath::WildSelector::shorthand);

  py::class_<libjsonpath::SliceSelector>(m, "SliceSelector")
      .def_readonly("token", &libjsonpath::SliceSelector::token)
      .def_readonly("start", &libjsonpath::SliceSelector::start)
      .def_readonly("stop", &libjsonpath::SliceSelector::stop)
      .def_readonly("step", &libjsonpath::SliceSelector::step);

  py::class_<libjsonpath::Box<libjsonpath::FilterSelector>>(m, "FilterSelector")
      .def_property_readonly(
          "token",
          [](const libjsonpath::Box<libjsonpath::FilterSelector>& selector) {
            return selector->token;
          })
      .def_property_readonly(
          "expression",
          [](const libjsonpath::Box<libjsonpath::FilterSelector>& selector) {
            return selector->expression;
          });

  py::class_<libjsonpath::Segment>(m, "Segment")
      .def_readonly("token", &libjsonpath::Segment::token)
      .def_readonly("selectors", &libjsonpath::Segment::selectors);

  py::class_<libjsonpath::RecursiveSegment>(m, "RecursiveSegment")
      .def_readonly("token", &libjsonpath::RecursiveSegment::token)
      .def_readonly("selectors", &libjsonpath::RecursiveSegment::selectors);

  py::enum_<libjsonpath::ExpressionType>(m, "ExpressionType")
      .value("value", libjsonpath::ExpressionType::value)
      .value("logical", libjsonpath::ExpressionType::logical)
      .value("nodes", libjsonpath::ExpressionType::nodes)
      .export_values();

  py::class_<libjsonpath::FunctionExtensionTypes>(m, "FunctionExtensionTypes")
      .def(py::init<std::vector<libjsonpath::ExpressionType>,
                    libjsonpath::ExpressionType>())
      .def_readonly("args", &libjsonpath::FunctionExtensionTypes::args)
      .def_readonly("res", &libjsonpath::FunctionExtensionTypes::res);

  m.def("parse", py::overload_cast<std::string_view>(&libjsonpath::parse),
        "Parse a JSONPath query string", py::return_value_policy::move);

  m.def(
      "parse",
      py::overload_cast<
          std::string_view,
          std::unordered_map<std::string, libjsonpath::FunctionExtensionTypes>>(
          &libjsonpath::parse),
      "Parse a JSONPath query string", py::return_value_policy::move);

  m.def("to_string", &libjsonpath::to_string, "JSONPath segments as a string");
  m.def("singular_query", &libjsonpath::singular_query,
        "Return True if a JSONPath is a singular query");

  py::class_<libjsonpath::JSONPathNode>(m, "JSONPathNode")
      .def_readonly("value", &libjsonpath::JSONPathNode::value)
      .def_readonly("location", &libjsonpath::JSONPathNode::location)
      .def("path", &libjsonpath::JSONPathNode::path);

  m.def("query",
        py::overload_cast<const libjsonpath::segments_t&, py::object, py::dict,
                          py::object>(&libjsonpath::query),
        "Query JSON-like data", py::return_value_policy::move);

  m.def(
      "query",
      py::overload_cast<std::string_view, py::object, py::dict,
                        const std::unordered_map<
                            std::string, libjsonpath::FunctionExtensionTypes>&,
                        py::object>(&libjsonpath::query),
      "Query JSON-like data", py::return_value_policy::move);

  py::class_<libjsonpath::FunctionExtension>(m, "FunctionExtension")
      .def(py::init<py::function, std::vector<libjsonpath::ExpressionType>,
                    libjsonpath::ExpressionType>())
      .def_readonly("func", &libjsonpath::FunctionExtension::func)
      .def_readonly("args", &libjsonpath::FunctionExtension::args)
      .def_readonly("res", &libjsonpath::FunctionExtension::res);

  py::bind_vector<std::vector<libjsonpath::JSONPathNode>>(m,
                                                          "JSONPathNodeList");
}
