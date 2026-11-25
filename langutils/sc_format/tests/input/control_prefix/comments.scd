// Comments around a control message. Before or after the part that changes, they stay where they are.
/* c */ x.if { 1 };
x.if { 1 } /* c */;
x.if { 1 } // c
;
x.if({ 1 }) /* c */ { 2 };
x.if { 1 } /* c */ { 2 };
x.if { 1 } { 2 } /* c */ .postln;
// After the `(`, which the prefix keeps.
x.if(/* c */ { 1 }, { 2 });
x.if( // c
	{ 1 }, { 2 });
x.if(
	{ 1 }, { 2 });
x.if( { 1 }, { 2 });
x.if({ 1 } /* c */, { 2 });
x.if({ 1 }, { 2 } /* c */);
x.if({ 1 }, { 2 } // c
);
1.for(/* from */ 10 /* to */) { |i| i };
// Inside the functions.
x.if { /* c */ 1 } { 2 // c
};
x.while { // c
	1
};
// Over lines, comments in the functions and after them.
(a > b).if({
	// yes
	1
}, {
	2 // no
}) // done
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
x /* c */ .switch(1, { a }, 2, { b });
// A comment that reaches the next line stays a `//` comment: the next code is still on its own line.
(
x // c
.if { 1 }
)
// Nested: the inner message is rewritten, then the outer one only without a comment in the part that changes.
(x.if /* c */ { 1 }).if { 2 };
(x /* c */ .if { 1 }).if { 2 };
(x.if { 1 } /* c */).if { 2 };
(x.if { 1 }).if /* c */ { 2 };
// Block comments that hold a `/*`, or text like code.
x.if { 1 /* a /* b */ c */ };
x.if { /* x.if { 1 } */ 2 };
x.if { "/* not a comment */" };
