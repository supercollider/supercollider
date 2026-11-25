// Comments where several rewrites change the same code.
(
// The prefix, then trailing closures, then the final `;` in each function.
a = { |x /* in */, y = (2)| // params
	if(x > y) { x /* bigger */ } { // else
		y // smaller
	} // the if
};
b = { |x = 1 /* c */|
	x.do { |i| if(i > 1) { i.postln /* a */ } /* b */ } // c
};
// A comment stops one rewrite, the others still happen.
c = { |x|
	x /* c */ .if { 1 } { 2 }
};
d = { arg /* c */ x;
	if(x, /* c */ { 1 }, { 2 })
};
e = { |x|
	while(x, { x = x - 1 } /* c */)
};
f = { |x|
	switch(x, 1 /* one */, { a }, 2, { b })
};
); // the first region
(
g = { |x, y /* c */ = 4| if(x) { y /* d */ } }
); /* the second */
h = { |x| if(x) { x } }; // the third
i = 1
