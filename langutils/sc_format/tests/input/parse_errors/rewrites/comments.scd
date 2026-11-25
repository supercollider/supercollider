// The statement with the error stays as written, the rest is rewritten, comments in both.
f = { arg a /* c */, b;
	a.if({ b; /* c */ });
	a =  ; // the error
	b.if({ a; }, /* c */ { b; });
};
g = { arg a; // fine
	a.do({ |i| i; }) // c
};
(
x = 1 2; // the error
x.if { 1 };
)
(
y = 3 // c
)
z = { arg x; x.if({ 1; }) }
