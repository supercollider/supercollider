// Comments around a control message. Before or after the part that changes, they stay where they are.
/* c */ if (x) { 1 };
if (x) { 1 } /* c */;
if (x) { 1 } // c
;
if (x) { 1 } /* c */ { 2 };
if (x) { 1 } /* c */ { 2 };
if (x) { 1 } { 2 } /* c */ .postln;
// After the `(`, which the prefix keeps.
if (x, /* c */ { 1 }, { 2 });
if (
	x, // c
	{ 1 },
	{ 2 }
);
if (x) { 1 } { 2 };
if (x) { 1 } { 2 };
if (x, { 1 } /* c */, { 2 });
if (x, { 1 }, { 2 } /* c */);
if (x, { 1 }, { 2 } // c
);
for (1, /* from */ 10 /* to */) { |i| i };
// Inside the functions.
if (x) { /* c */ 1 } {
	2 // c
};
while (x) { // c
	1
};
// Over lines, comments in the functions and after them.
if (a > b) {
	// yes
	1
} {
	2 // no
} // done
;
// Not with a comment in the part that changes: the receiver, its brackets, the `.`, the selector, or the `(`.
(a /* c */ > b).if { 1 };
(a // c
		> b).if { 1 };
(/* c */ x).if { 1 };
(x /* c */).if { 1 };
((x) /* c */).if { 1 };
x // c
.if { 1 };
x./* c */if { 1 };
x. // c
if { 1 };
x.if /* c */ ({ 1 }, { 2 });
x.if // c
({ 1 }, { 2 });
1 /* c */ .for(10) { |i| i };
-1 /* c */ .for(10) { |i| i };
{ a } /* c */ .try { |e| e };
x /* c */ .switch { 1 } { a } { 2 } { b };
// A comment that reaches the next line stays a `//` comment: the next code is still on its own line.
(
	x // c
		.if { 1 }
);
// Nested: the inner message is rewritten, then the outer one only without a comment in the part that changes.
(if (x) /* c */ { 1 }).if { 2 };
(x /* c */ .if { 1 }).if { 2 };
(if (x) { 1 } /* c */).if { 2 };
if (if (x) { 1 }) /* c */ { 2 };
// Block comments that hold a `/*`, or text like code.
if (x) { 1 /* a /* b */ c */ };
if (x) { /* x.if { 1 } */ 2 };
if (x) { "/* not a comment */" };
