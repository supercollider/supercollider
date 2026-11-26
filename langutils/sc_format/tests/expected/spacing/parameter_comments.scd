// The whitespace next to a comment stays as written, also inside pipes, where `|/*` would lex as one token.
a = { |x /* c */  | x };
b = { |  /* c */ x| x };
c = { |x /* c */| x };
d = { |x  /* c */, y| x };
