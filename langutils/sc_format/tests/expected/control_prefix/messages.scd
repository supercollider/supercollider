// Control flow moves its receiver into the call, brackets around it go.
if(x) { 1 } { 2 };
if(x, { 1 }, { 2 });
if(a > b, { 1 });
while({ c }, { b });
while({ c }) { b };
for(1, 10) { |i| i };
for(-1, 10) { |i| i };
if((x)) { 1 };
forBy(1, 10, 2, { |i| i });
switch(x, 1, { a }, 2, { b });
case(x);
try({ a }) { |e| e };
try({ a });
// Not iterators, a super send, or with a comment in the part that changes.
x.do { |i| i };
x.collect({ |i| i });
super.if({ 1 });
x /* c */ .if { 1 };
// A comment after it stays.
if(x) /* c */ { 1 };
// Nested, and messages after it.
if(if(x) { 1 } { 2 }) { 3 };
if(x) { 1 } { 2 }.postln;
if(x).(1);
// Over lines.
if(a > b) {
	1
} {
	2
};
