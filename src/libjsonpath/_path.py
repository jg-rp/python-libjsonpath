from __future__ import annotations

from typing import TYPE_CHECKING
from typing import List

from libjsonpath import query
from libjsonpath import to_string

from ._nothing import NOTHING

if TYPE_CHECKING:
    from libjsonpath import JSONPathEnvironment
    from libjsonpath import JSONPathNode
    from libjsonpath import Segments


class JSONPath:
    __slots__ = (
        "environment",
        "segments",
    )

    def __init__(self, environment: JSONPathEnvironment, segments: Segments) -> None:
        self.environment = environment
        self.segments = segments

    def findall(self, data: object) -> List[object]:
        return [node.value for node in self.query(data)]

    def query(self, data: object) -> List[JSONPathNode]:
        return query(
            self.segments,
            data,
            self.environment.function_register,
            NOTHING,
        )

    def __repr__(self) -> str:
        return f"<libjsonpath.JSONPath {to_string(self.segments)}>"
