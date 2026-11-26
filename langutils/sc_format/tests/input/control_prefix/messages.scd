// Control flow moves its receiver into the call, brackets around it go.
(x).if { 1 } { 2 };
x.if({ 1 }, { 2 });
(a > b).if({ 1 });
{ c }.while({ b });
{ c }.while { b };
1.for(10) { |i| i };
-1.for(10) { |i| i };
((x)).if { 1 };
1.forBy(10, 2, { |i| i });
x.switch(1, { a }, 2, { b });
x.case;
{ a }.try { |e| e };
{ a }.try;
// Not iterators, a super send, or with a comment in the part that changes.
x.do { |i| i };
x.collect({ |i| i });
super.if({ 1 });
x /* c */ .if { 1 };
// A comment after it stays.
x.if /* c */ { 1 };
// Nested, and messages after it.
(x.if { 1 } { 2 }).if { 3 };
x.if { 1 } { 2 }.postln;
x.if.(1);
// Over lines.
(a > b).if {
	1
} {
	2
};
