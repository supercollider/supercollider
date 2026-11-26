// Every message, with comments: around the message they stay, between the parts that move they stop it.
x.collect({ |i| i /* each */ }).sum; // total
x.inject(0, { |a, b| a + b }) /* c */;
Routine({ /* c */ 1.wait });
Routine({
	// wait
	1.wait
});
Foo /* c */ .new({ 1 }, { 2 });
Foo.new /* c */ ({ 1 }, { 2 });
Foo.new(/* c */ { 1 }, { 2 });
Foo.new({ 1 }, /* c */ { 2 });
Foo.new({ 1 }, { 2 } /* c */);
f(/* c */ { 1 });
f({ 1 } // c
);
f.(/* c */ { 1 });
f.({ 1 }) /* c */;
x.collect(f /* c */, { 1 });
x.collect(f, /* c */ { 1 });
x.collect(f, // c
	{ 1 });
Pbind(\a, 1, /* c */ \b, { 2 });
