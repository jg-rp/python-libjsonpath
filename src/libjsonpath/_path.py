from __future__ import annotations

from typing import TYPE_CHECKING
from typing import List

from libjsonpath import NOTHING
from libjsonpath import FunctionExtension
from libjsonpath import functions
from libjsonpath import parse
from libjsonpath import query
from libjsonpath import to_string

if TYPE_CHECKING:
    from libjsonpath import JSONPathNode

# TODO: add type hints here, remove _path.pyi


class JSONPath:
    __slots__ = (
        "environment",
        "segments",
    )

    def __init__(self, environment, segments) -> None:
        self.environment = environment
        self.segments = segments

    def findall(self, data):
        return [node.value for node in self.query(data)]

    def query(self, data):
        return query(
            self.segments,
            data,
            self.environment.function_register,
            NOTHING,
        )

    def __repr__(self) -> str:
        return f"<libjsonpath.JSONPath {to_string(self.segments)}>"


class JSONPathEnvironment:
    __slots__ = ("function_register",)

    def __init__(self) -> None:
        self.function_register = {}
        self.setup_function_register()

    def register_function(self, name, func) -> None:
        self.function_register[name] = FunctionExtension(
            func, func.arg_types, func.return_type
        )

    def setup_function_register(self) -> None:
        """Initialize function extensions."""
        self.register_function("length", functions.Length())
        self.register_function("count", functions.Count())
        self.register_function("match", functions.Match())
        self.register_function("search", functions.Search())
        self.register_function("value", functions.Value())

    def compile(self, path: str) -> JSONPath:  # noqa: A003
        return JSONPath(self, parse(path))

    def findall(self, path: str, data: object) -> List[object]:
        return [node.value for node in self.query(path, data)]

    def query(self, path: str, data: object) -> List[JSONPathNode]:
        return query(path, data, self.function_register, NOTHING)


__all__ = ("NOTHING", "JSONPath", "JSONPathEnvironment")
