#ifndef LIBJSONPATH_PATH_H
#define LIBJSONPATH_PATH_H

#include <cstdint>
#include <vector>

#include "libjsonpath/node.hpp"
#include "libjsonpath/selectors.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace libjsonpath {

// TODO: rename this file?
// TODO: just define `query`, the rest don't need to be in the header.

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

// Possible JSONPath function extension argument and result types.
enum class ExpressionType {
  value,
  logical,
  nodes,
};

// A JSONPath function extension's argument and result types.
struct FunctionExtension {
  py::function func;
  std::vector<ExpressionType> args;
  ExpressionType res;
};

// TODO: Rename this to indicate it is low-level and not meant to be called
// TODO: overload query to accept py::list for segments?
JSONPathNodeList query(const segments_t& segments, py::object obj,
                       py::dict functions, py::object nothing);

// Return a list of values from a node list, or a single value if
// the node list only has one item.
py::object values_or_singular(const JSONPathNodeList& nodes);

}  // namespace libjsonpath

#endif