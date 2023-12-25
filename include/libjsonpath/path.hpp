#ifndef LIBJSONPATH_PATH_H
#define LIBJSONPATH_PATH_H

#include <cstdint>
#include <string_view>
#include <vector>

#include "libjsonpath/node.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace libjsonpath {

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

// Apply the JSONPath query represented by _segments_ to JSON-like data _obj_.
JSONPathNodeList query(const segments_t& segments, py::object obj,
                       py::dict functions, py::object nothing);

// Parse the JSONPath query expression _path_ and use it to query JSON-like
// data in _obj_.
JSONPathNodeList query(std::string_view path, py::object obj,
                       py::dict functions, py::object nothing);

}  // namespace libjsonpath

#endif