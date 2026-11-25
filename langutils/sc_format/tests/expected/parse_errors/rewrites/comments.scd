// The statement with the error stays as written, the rest is rewritten, comments in both.
f = { |a /* c */, b|
	if(a) { b /* c */ };
	a =  ; // the error
	if(b, { a }, /* c */ { b })
};
g = { |a| // fine
	a.do { |i| i } // c
};
(
x = 1 2; // the error
x.if { 1 };
)
(
y = 3 // c
);
z = { |x| if(x) { 1 } }
