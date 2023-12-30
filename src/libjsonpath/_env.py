from __future__ import annotations

from typing import TYPE_CHECKING
from typing import Dict
from typing import List

if TYPE_CHECKING:
    from libjsonpath import FilterFunction
    from libjsonpath import JSONPathNode


from libjsonpath import FunctionExtension
from libjsonpath import FunctionExtensionTypes
from libjsonpath import Parser
from libjsonpath import parse
from libjsonpath import query

from ._nothing import NOTHING
from ._path import JSONPath
from .functions import Count
from .functions import Length
from .functions import Match
from .functions import Search
from .functions import Value


class JSONPathEnvironment:
    __slots__ = ("function_register", "_parser", "_function_types")

    def __init__(self) -> None:
        self.function_register: Dict[str, FunctionExtension] = {}
        self.setup_function_register()
        self._function_types = self._just_function_types()
        self._parser = Parser(self._function_types)

    def register_function(self, name: str, func: FilterFunction) -> None:
        self.function_register[name] = FunctionExtension(
            func, list(func.arg_types), func.return_type
        )
        self._function_types = self._just_function_types()
        self._parser = Parser(self._function_types)

    def setup_function_register(self) -> None:
        """Initialize function extensions."""
        self.register_function("count", Count())
        self.register_function("length", Length())
        self.register_function("match", Match())
        self.register_function("search", Search())
        self.register_function("value", Value())

    def compile(self, path: str) -> JSONPath:  # noqa: A003
        return JSONPath(self, parse(path))

    def findall(self, path: str, data: object) -> List[object]:
        return [node.value for node in self.query(path, data)]

    def query(self, path: str, data: object) -> List[JSONPathNode]:
        return query(path, data, self.function_register, self._function_types, NOTHING)

    def _just_function_types(self) -> Dict[str, FunctionExtensionTypes]:
        return {
            k: FunctionExtensionTypes(v.args, v.res)
            for k, v in self.function_register.items()
        }
