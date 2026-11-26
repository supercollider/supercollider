// The prefix, then the trailing closures, with comments in the way of one or the other.
if(x) { 1 } { 2 } /* c */;
if(x, /* c */ { 1 }, { 2 });
if(x, { 1 }, /* c */ { 2 });
if(x, { 1 }, { 2 } /* c */);
if(x, { 1 } // c
);
x /* c */ .if { 1 } { 2 };
x.if /* c */ ({ 1 }, { 2 });
(a > b /* c */).if { 1 };
for(1, 10 /* to */, { |i| i });
for(1, 10, /* c */ { |i| i });
switch(x, 1 /* c */, { a }, 2, { b });
switch(x) { 1 } { a /* c */ } { 2 } { b };
while { c } { b /* c */ };
{ c /* c */ }.while { b };
try { 1 } { |e| e } // c
;
// Over lines.
if(x) {
	// yes
	1
} {
	2
} // done
;
