// A function's last statement loses its `;`, the top level keeps it.
a = { 1; };
b = { |x| var y = x; y + 1; };
c = {
	var y = 2;
	y.postln;
	^y;
};
// With the whitespace before it, but not across a comment.
d = { 1 ; };
e = { 1 /* one */; };
f = { 1; // one
};
// After a `var` statement the body returns nothing, so the `;` stays.
g = { var y; };
h = { };
(
var x = { 1; };
x.value;
);
