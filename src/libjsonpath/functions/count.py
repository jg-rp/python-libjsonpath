"""The standard `count` function extension."""
from __future__ import annotations

from typing import TYPE_CHECKING

from libjsonpath import ExpressionType

from .filter_function import FilterFunction

if TYPE_CHECKING:
    from libjsonpath import JSONPathNodeList


class Count(FilterFunction):
    """The built-in `count` function."""

    arg_types = (ExpressionType.nodes,)
    return_type = ExpressionType.value

    def __call__(self, node_list: JSONPathNodeList) -> int:
        """Return the number of nodes in the node list."""
        return len(node_list)
