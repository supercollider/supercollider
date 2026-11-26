// Comments with trailing closures. Before the message, or in a function that moves, they move with it.
/* c */ if(c) { 1 } { 2 };
if /* c */ (c) { 1 } { 2 };
if(c) { /* c */ 1 } { 2 // c
};
if(c) {
	// yes
	1
} {
	2 /* no */
};
x.do { |i /* index */| i };
x.do { |i| i } /* c */;
x.do { |i| i } // c
;
// Before a function already after the `)`.
x.do { 1 } /* c */ { 2 };
x.do { 1 } // c
{ 2 };
// In the arguments that stay.
if(/* c */ c) { 1 } { 2 };
if(c /* c */ > 1) { 1 } { 2 };
for(1, /* to */ 10) { |i| i };
// Not with a comment between the parts that move: before or after a moved function's `(` or comma, or the `)`.
x.do /* c */ ({ |i| i });
x.do // c
({ |i| i });
x.do(/* c */ { |i| i });
x.do( // c
	{ |i| i });
x.do({ |i| i } /* c */);
x.do({ |i| i } // c
);
x.do({ |i| i }, /* c */);
x.do({ |i| i } /* c */,);
if(c /* c */, { 1 }, { 2 });
if(c // c
	, { 1 }, { 2 });
if(c, /* c */ { 1 }, { 2 });
if(c, // c
	{ 1 }, { 2 });
if(c, { 1 } /* c */, { 2 });
if(c, { 1 } // c
	, { 2 });
if(c, { 1 }, // c
	{ 2 });
while({ a }, /* c */ { b });
try({ 1 }, /* c */ { |error| error });
// Nested: the inner one is rewritten when the outer one has a comment in the way.
x.do(/* c */ { |i| if(i > 1) { i } { 0 } });
x.do { |i| if(i > 1, { i }, /* c */ { 0 }) };
// A `switch` or `case`: a comment by a test or a function stops it.
switch(x, /* c */ 1, { a }, 2, { b });
switch(x, 1 /* c */, { a }, 2, { b });
switch(x, 1, /* c */ { a }, 2, { b });
switch(x, 1, { a } /* c */, 2, { b });
switch(x, 1, { a }, 2, { b }, /* c */ { c });
switch(x, 1, { a }, // c
	2, { b });
switch(x /* c */, 1, { a }, 2, { b });
case({ a } /* c */, { b });
case({ a }, { b }, // c
	{ c }, { d });
// Inside a test or a function, the `switch` changes and the comment stays in it.
switch(x) { 1 } { /* c */ a } { 2 } { b // c
};
case { a /* c */ } { b };
