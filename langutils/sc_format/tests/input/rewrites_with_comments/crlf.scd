// Every rewrite with CRLF newlines, which become LF.
(
a = { arg x /* c */, y; // params
	x.if({ 1; }, { // else
		2; // two
	}); // the if
};
b = { arg x, // first
	y = 4.dbamp; x };
c = { 1; // one
};
d = { arg; // nothing
	1
};
)
x = 1 // c
y = 2
