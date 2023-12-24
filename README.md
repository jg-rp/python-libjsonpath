# libjsonpath

Python bindings for [libjsonpath](https://github.com/jg-rp/libjsonpath).

## Performance

Repeating 364 queries from the [compliance test suite](https://github.com/jsonpath-standard/jsonpath-compliance-test-suite) 100 times. These are very small queries on very small data.

| Impl                                   | Compile and find | Just compile | Just find |
| -------------------------------------- | ---------------- | ------------ | --------- |
| Pure Python <sup>[1]</sup>             | 3.43 sec         | 2.3 sec      | 0.9 sec   |
| Python on libjsonpath <sup>[2]</sup>   | 1.81 sec         | 0.09 sec     | 1.61 sec  |
| pybind11 on libjsonpath <sup>[3]</sup> | 0.61 sec         | 0.08 sec     | 0.47 sec  |

1. [jg-rp/python-jsonpath](https://github.com/jg-rp/python-jsonpath/graphs/traffic) - Works on any Mapping/Sequence types.
2. [jg-rp/jsonpath11](https://github.com/jg-rp/jsonpath11) - Works on any Mapping/Sequence types.
3. This project - Limited to dicts and lists. Still uses Python functions as the JSONPath function extension point.
