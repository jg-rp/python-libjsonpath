"""The standard `value` function extension."""
from __future__ import annotations

from typing import TYPE_CHECKING

from libjsonpath import NOTHING
from libjsonpath import ExpressionType
from libjsonpath import FilterFunction

if TYPE_CHECKING:
    from libjsonpath import JSONPathNodeList


class Value(FilterFunction):
    """A type-aware implementation of the standard `value` function."""

    arg_types = (ExpressionType.nodes,)
    return_type = ExpressionType.value

    def __call__(self, nodes: JSONPathNodeList) -> object:
        """Return the first node in a node list if it has only one item."""
        if len(nodes) == 1:
            return nodes[0].value
        return NOTHING
