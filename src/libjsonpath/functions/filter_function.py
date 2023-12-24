"""Classes modeling the JSONPath spec type system for function extensions."""
from __future__ import annotations

from abc import ABC, abstractmethod
from typing import Any, Tuple

from libjsonpath import ExpressionType


class FilterFunction(ABC):
    """Base class for typed function extensions."""

    @property
    @abstractmethod
    def arg_types(self) -> Tuple[ExpressionType]:
        """Argument types expected by the filter function."""

    @property
    @abstractmethod
    def return_type(self) -> ExpressionType:
        """The type of the value returned by the filter function."""

    @abstractmethod
    def __call__(self, *args: Any, **kwds: Any) -> Any:
        """Called the filter function."""
