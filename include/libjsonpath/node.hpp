#ifndef LIBJSONPATH_NODE_H
#define LIBJSONPATH_NODE_H

#include <pybind11/pybind11.h>

#include <string>
#include <variant>
#include <vector>

namespace py = pybind11;

namespace libjsonpath {

using location_t = std::vector<std::variant<size_t, std::string>>;

// A JSON-like object and its location within a JSON document.
class JSONPathNode {
public:
  py::object value;
  location_t location;

  JSONPathNode(py::object& value_, location_t location_);

  // Return the canonical string representation of the path to this node.
  std::string path();
};

using JSONPathNodeList = std::vector<JSONPathNode>;

}  // namespace libjsonpath

#endif