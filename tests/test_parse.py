import libjsonpath


def test_parse() -> None:
    """Test that we can parse a JSONPath query."""
    query = "$.foo.bar"
    segments = libjsonpath.parse(query)
    assert libjsonpath.to_string(segments) == "$['foo']['bar']"
