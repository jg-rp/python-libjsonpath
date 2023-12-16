from glob import glob

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.1.0"

# XXX: Make sure to run `cmake ..` from within `extern/libjsonpath/build`
# before building this package.

ext_modules = [
    Pybind11Extension(
        name="_libjsonpath",
        sources=[
            "src/libjsonpath/_libjsonpath.cpp",
            *sorted(glob("extern/libjsonpath/src/libjsonpath/*.cpp")),
        ],
        include_dirs=[
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
