(
x = {
	1;
	2
};
if (x) {
	1
} {
	2
};
y.do { |i|
	i.postln
};
// A control message's functions join if one spans lines, or each is a literal.
if (x) { 1 } { 2 };
if(x) {1} {"a"};
if(x) { 1 } {
	2
};
if(x) { foo.bar } {
	2
};
// Else they stay on their lines, as one line could be long.
if(scroll)
{ view = ScrollTopView.new(this, name, bounds) }
{ view = TopView.new(this, name, bounds) };
while { x }
{ x = x - 1 };
x.do
{ |i| i.postln };
// Not joined: a single-line block of anything else, a case's pairs, a statement, an argument, an operand, a comment
// before it.
y = Routine
{ 1 };
if(x) // why
{ 1 };
case
{ x } { 1 }
{ y } {
	2
};
{
	1
}.value;
f(
	{
		1
	}
);
z = x and:
{
	y
};
w = x // why
{
	y
};
)
// A multi-line block's `{` ends its line, after its parameters, or before a comment.
x = {
	foo;
	bar
};
y = { |a|
	a.postln;
	a
};
z = { arg a;
	a.postln;
	a
};
w = { // comment
	1
};
if(this.isPlaying) {
	bundle = MixedBundle.new;
	group = inGroup;
} { group = inGroup };
// Not a generator.
g = {: x,
	x <- y };
