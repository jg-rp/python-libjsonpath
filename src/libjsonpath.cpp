#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <libjsonpath/tokens.hpp>
#include <libjsonpath/lex.hpp>
#include <libjsonpath/selectors.hpp>
#include <libjsonpath/jsonpath.hpp>
#include <variant>
#include <vector>
#include <string_view>

namespace py = pybind11;

PYBIND11_MODULE(libjsonpath, m)
{
    m.doc() = "JSONPath parser";

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
        .value("filter", libjsonpath::TokenType::filter)
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
        .value("name", libjsonpath::TokenType::name)
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
        .def("__eq__", [](const libjsonpath::Token &t, const libjsonpath::Token &other)
             { return libjsonpath::operator==(t, other); })
        .def("__str__", [](const libjsonpath::Token &t)
             { return libjsonpath::token_to_string(t); });

    py::class_<libjsonpath::Lexer>(m, "Lexer")
        .def(py::init<std::string_view>())
        .def("run", &libjsonpath::Lexer::run)
        .def("tokens", &libjsonpath::Lexer::tokens);

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

    py::class_<libjsonpath::LogicalNotExpression>(m, "LogicalNotExpression")
        .def_readonly("token", &libjsonpath::LogicalNotExpression::token)
        .def_readonly("right", &libjsonpath::LogicalNotExpression::right);

    py::class_<libjsonpath::InfixExpression>(m, "InfixExpression")
        .def_readonly("token", &libjsonpath::InfixExpression::token)
        .def_readonly("left", &libjsonpath::InfixExpression::left)
        .def_readonly("op", &libjsonpath::InfixExpression::op)
        .def_readonly("right", &libjsonpath::InfixExpression::right);

    py::class_<libjsonpath::RelativeQuery>(m, "RelativeQuery")
        .def(py::init<>())
        .def_readonly("token", &libjsonpath::RelativeQuery::token)
        .def_readonly("query", &libjsonpath::RelativeQuery::query);

    py::class_<libjsonpath::RootQuery>(m, "RootQuery")
        .def_readonly("token", &libjsonpath::RootQuery::token)
        .def_readonly("query", &libjsonpath::RootQuery::query);

    py::class_<libjsonpath::FunctionCall>(m, "FunctionCall")
        .def_readonly("token", &libjsonpath::FunctionCall::token)
        .def_readonly("name", &libjsonpath::FunctionCall::name)
        .def_readonly("args", &libjsonpath::FunctionCall::args);

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

    py::class_<libjsonpath::FilterSelector>(m, "FilterSelector")
        .def_readonly("token", &libjsonpath::FilterSelector::token)
        .def_readonly("expression", &libjsonpath::FilterSelector::expression);

    py::class_<libjsonpath::Segment>(m, "Segment")
        .def_readonly("token", &libjsonpath::Segment::token)
        .def_readonly("selectors", &libjsonpath::Segment::selectors);

    py::class_<libjsonpath::RecursiveSegment>(m, "RecursiveSegment")
        .def_readonly("token", &libjsonpath::RecursiveSegment::token)
        .def_readonly("selectors", &libjsonpath::RecursiveSegment::selectors);

    m.def("parse", &libjsonpath::parse, "Parse a JSONPath query string", py::return_value_policy::move);
    m.def("to_string", &libjsonpath::to_string, "JSONPath segments as a string");
}
