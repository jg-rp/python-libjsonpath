# libjsonpath

Python bindings for [libjsonpath](https://github.com/jg-rp/libjsonpath).

## Performance

Repeating 364 queries from the [compliance test suite](https://github.com/jsonpath-standard/jsonpath-compliance-test-suite) 100 times. These are very small queries on very small data.

| Impl                                   | Compile and find (values) | Compile and find (nodes) | Just compile | Just find (values) | Just find (nodes) |
| -------------------------------------- | ------------------------- | ------------------------ | ------------ | ------------------ | ----------------- |
| Pure Python <sup>[1]</sup>             | 3.45 s                    | 3.35 s                   | 2.3 s        | 0.87 s             | 0.86 s            |
| Python on libjsonpath <sup>[2]</sup>   | 1.81 s                    | 1.78 s                   | 0.09 s       | 1.62 s             | 1.63 s            |
| pybind11 on libjsonpath <sup>[3]</sup> | 0.55 s                    | 0.25 s                   | 0.08 s       | 0.47 s             | 0.17 s            |

<sup>[1]</sup> [jg-rp/python-jsonpath](https://github.com/jg-rp/python-jsonpath/graphs/traffic) - Works on any Mapping/Sequence types.

<sup>[2]</sup> [jg-rp/jsonpath11](https://github.com/jg-rp/jsonpath11) - Works on any Mapping/Sequence types.

<sup>[3]</sup> This project - Limited to dicts and lists. Still uses Python functions as the JSONPath function extension point.

### Nodes vs values

[1] and [2]'s use of Python generators means the difference between retrieving a query's nodes vs values from those nodes is negligible. [3] builds a C++ std::vector of nodes internally. Converting that to a Python list of values is more significant.
