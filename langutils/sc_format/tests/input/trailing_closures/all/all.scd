// Every message.
x.collect({ |i| i }).sum;
x.inject(0, { |a, b| a + b });
Routine({ 1.wait });
Foo.new({ 1 }, { 2 });
f({ 1 });
f.({ 1 });
x.collect(f, { 1 }, g);
x.collect.at({ 1 });
// `if` still needs every branch.
if(c, x, { 2 });
