import re
from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

with open("src/libjsonpath/__about__.py", encoding="utf-8") as fd:
    match = re.search(r'__version__ = "([0-9\.]+)"', fd.read())
    assert match
    __version__ = match.group(1)

ext_modules = [
    Pybind11Extension(
        name="_libjsonpath",
        sources=[
            "src/libjsonpath/_libjsonpath.cpp",
            "src/libjsonpath/_node.cpp",
            "src/libjsonpath/_path.cpp",
            *sorted(glob("extern/libjsonpath/src/libjsonpath/*.cpp")),
        ],
        include_dirs=[
            "include",
            "extern/libjsonpath/include",
        ],
        # XXX: Example: passing in the version to the compiled code
        define_macros=[("VERSION_INFO", __version__)],
    ),
]

setup(
    name="libjsonpath",
    package_dir={"": "src"},
    packages=["libjsonpath"],
    version=__version__,
    url="https://github.com/jg-rp/python_libjsonpath",
    description="Python bindings for libjsonpath.",
    long_description=open("README.md", encoding="utf8").read(),
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
    include_package_data=True,
    package_data={"": ["py.typed", "__init__.pyi"]},
)
