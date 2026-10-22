// A line whose first token can't continue the previous line gets a semicolon inserted before it, and it is reported.
// The lexer asks the parser whether it can take the token, so the expression on the previous line is kept.
1 + 2
r { 3 }
(
	4
)
(
	5
)
// These continue the previous line, so nothing is inserted.
a = 6
- 1;
b = a
.neg;
c = [7,
	8];
// A separator can only go between region items, inside brackets and functions these are errors.
(9
10);
{ 11
12 };
a + b
