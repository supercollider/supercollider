// Comments where several rewrites change the same code.
(
// The prefix, then trailing closures, then the final `;` in each function.
a = { arg x /* in */, y(2); // params
	(x > y).if({ x; /* bigger */ }, { // else
		y; // smaller
	}); // the if
};
b = { |x 1 /* c */|
	x.do({ |i| (i > 1).if({ i.postln; /* a */ }); /* b */ }); // c
};
// A comment stops one rewrite, the others still happen.
c = { arg x;
	x /* c */ .if({ 1; }, { 2; });
};
d = { arg /* c */ x;
	x.if(/* c */ { 1; }, { 2; });
};
e = { arg x;
	x.while({ x = x - 1; } /* c */);
};
f = { arg x;
	x.switch(1 /* one */, { a; }, 2, { b; });
};
) // the first region
(
g = { |x y /* c */ = 4| x.if { y; /* d */ } }
) /* the second */
h = { arg x; x.if({ x; }) } // the third
i = 1
