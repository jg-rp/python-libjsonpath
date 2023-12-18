import libjsonpath


def test_parse() -> None:
    """Test that we can parse a JSONPath query."""
    query = "$.foo.bar"
    segments = libjsonpath.parse(query)
    assert libjsonpath.to_string(segments) == "$['foo']['bar']"


def test_parse_recursive() -> None:
    """Test that we can parse a JSONPath query."""
    query = "$..[1]"
    segments = libjsonpath.parse(query)
    assert libjsonpath.to_string(segments) == "$..[1]"


def test_parse_issue() -> None:
    """Test that we can parse a JSONPath query."""
    query = "$[?@.a]"
    segments = libjsonpath.parse(query)
    assert libjsonpath.to_string(segments) == "$[?@['a']]"
    assert libjsonpath.to_string(segments) == "$[?@['a']]"
