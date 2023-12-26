from enum import Enum
from typing import Any
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import Sequence
from typing import Union
from typing import overload

from ._path import JSONPath
from .filter_function import FilterFunction as FilterFunction

class TokenType(Enum):
    eof_ = ...
    and_ = ...
    colon = ...
    comma = ...
    current = ...
    ddot = ...
    dq_string = ...
    eq = ...
    error = ...
    false_ = ...
    filter = ...
    float_ = ...
    func_ = ...
    ge = ...
    gt = ...
    index = ...
    int_ = ...
    lbracket = ...
    le = ...
    lparen = ...
    lt = ...
    name = ...
    ne = ...
    not_ = ...
    null_ = ...
    or_ = ...
    rbracket = ...
    root = ...
    rparen = ...
    sq_string = ...
    true_ = ...
    wild = ...

class Token:
    @property
    def type(self) -> TokenType: ...
    @property
    def value(self) -> str: ...
    @property
    def index(self) -> int: ...
    @property
    def query(self) -> str: ...

class Lexer:
    def __init__(self, query: str) -> None: ...
    def run(self) -> None: ...
    def tokens(self) -> Sequence[Token]: ...

class BinaryOperator(Enum):
    none = ...
    logical_and = ...
    logical_or = ...
    eq = ...
    ge = ...
    gt = ...
    le = ...
    lt = ...
    ne = ...

class ExpressionType(Enum):
    value = ...
    logical = ...
    nodes = ...

Segments = Sequence[Union["Segment", "RecursiveSegment"]]

Selector = Union[
    "NameSelector",
    "IndexSelector",
    "WildSelector",
    "SliceSelector",
    "FilterSelector",
]

Expression = Union[
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
]

class NullLiteral:
    @property
    def token(self) -> Token: ...

class BooleanLiteral:
    @property
    def token(self) -> Token: ...
    @property
    def value(self) -> bool: ...

class IntegerLiteral:
    @property
    def token(self) -> Token: ...
    @property
    def value(self) -> int: ...

class FloatLiteral:
    @property
    def token(self) -> Token: ...
    @property
    def value(self) -> float: ...

class StringLiteral:
    @property
    def token(self) -> Token: ...
    @property
    def value(self) -> str: ...

class LogicalNotExpression:
    @property
    def token(self) -> Token: ...
    @property
    def right(self) -> Expression: ...

class InfixExpression:
    @property
    def token(self) -> Token: ...
    @property
    def left(self) -> Expression: ...
    @property
    def op(self) -> BinaryOperator: ...
    @property
    def right(self) -> Expression: ...

class RelativeQuery:
    @property
    def token(self) -> Token: ...
    @property
    def query(self) -> Segments: ...

class RootQuery:
    @property
    def token(self) -> Token: ...
    @property
    def query(self) -> Segments: ...

class FunctionCall:
    @property
    def token(self) -> Token: ...
    @property
    def name(self) -> str: ...
    @property
    def args(self) -> Sequence[Expression]: ...

class NameSelector:
    @property
    def token(self) -> Token: ...
    @property
    def name(self) -> str: ...
    @property
    def shorthand(self) -> bool: ...

class IndexSelector:
    @property
    def token(self) -> Token: ...
    @property
    def index(self) -> int: ...

class WildSelector:
    @property
    def token(self) -> Token: ...
    @property
    def shorthand(self) -> bool: ...

class SliceSelector:
    @property
    def token(self) -> Token: ...
    @property
    def start(self) -> Optional[int]: ...
    @property
    def stop(self) -> Optional[int]: ...
    @property
    def step(self) -> Optional[int]: ...

class FilterSelector:
    @property
    def token(self) -> Token: ...
    @property
    def expression(self) -> Expression: ...

class Segment:
    @property
    def token(self) -> Token: ...
    @property
    def selectors(self) -> Sequence[Selector]: ...

class RecursiveSegment:
    @property
    def token(self) -> Token: ...
    @property
    def selectors(self) -> Sequence[Selector]: ...

def parse(query: str) -> Segments: ...
def to_string(segments: Segments) -> str: ...
def singular_query(segments: Segments) -> bool: ...

class JSONPathNode:
    @property
    def value(self) -> object: ...
    @property
    def location(self) -> List[Union[int, str]]: ...
    def path(self) -> str: ...

JSONPathNodeList = Sequence[JSONPathNode]

FilterFunctionT = Callable[..., Any]

class FunctionExtension:
    def __init__(
        self, func: Callable[..., Any], args: List[ExpressionType], res: ExpressionType
    ): ...
    @property
    def func(self) -> Callable[..., Any]: ...
    @property
    def args(self) -> List[ExpressionType]: ...
    @property
    def res(self) -> ExpressionType: ...

@overload
def query(
    segments: Segments,
    data: object,
    functions: Dict[str, FunctionExtension],
    nothing: object,
): ...
@overload
def query(
    path: str,
    data: object,
    functions: Dict[str, FunctionExtension],
    nothing: object,
): ...
def compile(path: str) -> JSONPath: ...  # noqa: A001
def findall(path: str, data: object) -> List[object]: ...
