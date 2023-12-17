import libjsonpath


def test_singular_query() -> None:
    """Test that we can identify singular queries."""
    query = "$.foo"
    segments = libjsonpath.parse(query)
    assert libjsonpath.singular_query(segments)


def test_non_singular_query() -> None:
    """Test that we can identify non-singular queries."""
    query = "$.*"
    segments = libjsonpath.parse(query)
    assert not libjsonpath.singular_query(segments)
