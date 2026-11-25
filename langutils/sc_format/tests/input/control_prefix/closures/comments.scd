// The prefix, then the trailing closures, with comments in the way of one or the other.
x.if({ 1 }, { 2 }) /* c */;
x.if(/* c */ { 1 }, { 2 });
x.if({ 1 }, /* c */ { 2 });
x.if({ 1 }, { 2 } /* c */);
x.if({ 1 } // c
);
x /* c */ .if({ 1 }, { 2 });
x.if /* c */ ({ 1 }, { 2 });
(a > b /* c */).if({ 1 });
1.for(10 /* to */, { |i| i });
1.for(10, /* c */ { |i| i });
x.switch(1 /* c */, { a }, 2, { b });
x.switch(1, { a /* c */ }, 2, { b });
{ c }.while({ b /* c */ });
{ c /* c */ }.while({ b });
{ 1 }.try({ |e| e }) // c
;
// Over lines.
x.if({
	// yes
	1
}, {
	2
}) // done
;
