# Wrapping the _libjsonpath module in a package so we can include type stubs.
from _libjsonpath import *  # noqa: F403

from .__about__ import __version__
