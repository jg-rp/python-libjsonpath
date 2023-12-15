import libjsonpath


def token_eq(
    t: libjsonpath.Token,
    type_: libjsonpath.TokenType,
    index: int,
    value: str,
    query: str,
) -> bool:
    return (
        t.type == type_ and t.index == index and t.value == value and t.query == query
    )


def test_tokenize() -> None:
    """Test that we can tokenize a JSONPath query."""
    query = "$.foo.bar"
    lexer = libjsonpath.Lexer(query)
    lexer.run()
    tokens = lexer.tokens()
    assert len(tokens) == 4  # + EOF
    assert token_eq(tokens[0], libjsonpath.TokenType.root, 0, "$", query)
    assert token_eq(tokens[1], libjsonpath.TokenType.name, 2, "foo", query)
    assert token_eq(tokens[2], libjsonpath.TokenType.name, 6, "bar", query)
    assert token_eq(tokens[3], libjsonpath.TokenType.eof_, 9, "", query)
