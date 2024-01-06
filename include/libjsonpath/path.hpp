#ifndef LIBJSONPATH_PATH_H
#define LIBJSONPATH_PATH_H

#include <string_view>
#include <vector>

#include "libjsonpath/node.hpp"
#include "libjsonpath/parse.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace libjsonpath {

// TODO: remove after update libjsonpath
using function_signature_map =
    std::unordered_map<std::string, libjsonpath::FunctionExtensionTypes>;

using function_extension_map = std::unordered_map<std::string, py::function>;

// Apply the JSONPath query represented by _segments_ to JSON-like data _obj_.
JSONPathNodeList query_(const segments_t& segments, py::object obj,
                        function_extension_map functions,
                        function_signature_map signatures, py::object nothing);

// Parse the JSONPath query expression _path_ and use it to query JSON-like
// data in _obj_.
JSONPathNodeList query_(std::string_view path, py::object obj,
                        function_extension_map functions,
                        function_signature_map signatures, py::object nothing);

class Env_ {
private:
  function_extension_map m_functions{};
  function_signature_map m_signatures{};
  py::object m_nothing{};
  Parser m_parser{};

public:
  Env_(function_extension_map functions, function_signature_map signatures,
       py::object nothing)
      : m_functions{functions},
        m_signatures{signatures},
        m_nothing{nothing},
        m_parser{signatures} {}

  JSONPathNodeList query(std::string_view path, py::object obj);
  JSONPathNodeList from_segments(const segments_t& segments, py::object obj);
  segments_t parse(std::string_view path);
};

}  // namespace libjsonpath

#endif