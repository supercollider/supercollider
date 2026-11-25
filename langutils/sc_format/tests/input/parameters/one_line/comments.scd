(
// On one line, with comments, it changes. Not over lines, also by a block comment that holds a newline, unless that
// comment is after the `;`.
a = { arg x /* c */, y = 4.dbamp; x };
b = { arg x, // c
	y; x };
c = { arg x, /* c
	*/ y; x };
d = { arg x, /* c */ y; /* the next
	line */ x };
)
