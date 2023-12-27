from _libjsonpath import TokenType
from _libjsonpath import Token
from _libjsonpath import Lexer
from _libjsonpath import BinaryOperator
from _libjsonpath import ExpressionType
from _libjsonpath import NullLiteral
from _libjsonpath import BooleanLiteral
from _libjsonpath import IntegerLiteral
from _libjsonpath import FloatLiteral
from _libjsonpath import StringLiteral
from _libjsonpath import LogicalNotExpression
from _libjsonpath import InfixExpression
from _libjsonpath import RelativeQuery
from _libjsonpath import RootQuery
from _libjsonpath import FunctionCall
from _libjsonpath import NameSelector
from _libjsonpath import IndexSelector
from _libjsonpath import WildSelector
from _libjsonpath import SliceSelector
from _libjsonpath import FilterSelector
from _libjsonpath import Segment
from _libjsonpath import RecursiveSegment
from _libjsonpath import parse
from _libjsonpath import to_string
from _libjsonpath import singular_query
from _libjsonpath import JSONPathNode
from _libjsonpath import FunctionExtension
from _libjsonpath import query

from .__about__ import __version__

from ._nothing import NOTHING
from .filter_function import FilterFunction
from ._path import JSONPath
from ._env import JSONPathEnvironment


__all__ = (
    "JSONPathEnvironment",
    "FilterFunction",
    "JSONPath",
    "NOTHING",
    "TokenType",
    "Token",
    "Lexer",
    "BinaryOperator",
    "ExpressionType",
    "NullLiteral",
    "BooleanLiteral",
    "IntegerLiteral",
    "FloatLiteral",
    "StringLiteral",
    "LogicalNotExpression",
    "InfixExpression",
    "RelativeQuery",
    "RootQuery",
    "FunctionCall",
    "NameSelector",
    "IndexSelector",
    "WildSelector",
    "SliceSelector",
    "FilterSelector",
    "Segment",
    "RecursiveSegment",
    "parse",
    "to_string",
    "singular_query",
    "JSONPathNode",
    "FunctionExtension",
    "query",
    "compile",
    "findall",
)

DEFAULT_ENV = JSONPathEnvironment()
compile = DEFAULT_ENV.compile  # noqa: A001
findall = DEFAULT_ENV.findall
xquery = DEFAULT_ENV.query  # XXX:
