from _libjsonpath import *  # TODO: don't import all of _libjsonpath into namespace

NOTHING = object()

from .__about__ import __version__
from .filter_function import FilterFunction
from ._path import NOTHING, JSONPath, JSONPathEnvironment

# TODO: __all__

DEFAULT_ENV = JSONPathEnvironment()
compile = DEFAULT_ENV.compile  # noqa: A001
findall = DEFAULT_ENV.findall
xquery = DEFAULT_ENV.query  # XXX:
