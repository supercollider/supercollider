// `if` only when every branch is a function, as the compiler inlines it only then.
if(c, { 1 }, { 2 });
if(c, { 1 });
if(c, x, { 2 });
if(c, { 1 }, y);
c.if({ 1 }, { 2 });
c.if(x, { 2 });
// The longest run of functions at the end moves, and an empty `()` goes.
while({ a }, { b });
for(1, 10, { |i| i.postln });
forBy(1, 10, 2, { |i| i.postln });
x.do({ |i| i });
x.do( { |i| i }, );
do(x, { |i| i });
{ 1 }.try({ |error| error.postln });
try({ 1 }, { |error| error.postln });
{ 1 }.try(f);
// `switch` and `case` only when no value and function pair would be split. A `switch`'s literal tests go in
// braces where both compilers inline it either way.
switch(x, 1, { a }, 2, { b });
switch(x, -1, { a }, 2.5, { b }, $c, { c }, true, { d }, \e, { e }, 'f', { f }, pi, { g }, 1s, { h }, { i });
switch(x, { a });
case({ a }, { b }, { c }, { d });
case(true, { b }, { c }, { d });
// Not a string or nil test, which isn't inlined, nor a function with parameters or variables.
switch(x, "a", { a }, 2, { b });
switch(x, nil, { a }, 2, { b });
switch(x, 1, { |y| a }, 2, { b });
switch(x, 1, { var y = 1; y }, 2, { b });
switch(x, y, { a }, 2, { b });
switch(x, (1), { a }, 2, { b });
// Line breaks between the functions stay.
if(c, {
	1
}, {
	2
});
if(c,
	{ 1 },
	{ 2 }
);
x.collect({ |i| i });
// Before a function already after the `)`.
x.do({ 1 }) { 2 };
// Not other messages, nor with a keyword or `*` argument, brackets, or a comment.
x.select({ |i| i });
x.do(a: 1, { });
x.do(*a, { });
x.do(({ 1 }));
if(c, { 1 }, /* c */ { 2 });
if(c, { 1 }, { 2 } // c
);
// Nested.
x.do({ |i| if(i > 1, { i }, { 0 }) });
