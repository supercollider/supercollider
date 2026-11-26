// The space before a trailing closure: after a message's `)`, its name, or the closure before.
(
if(x){ 1 }{ 2 };
if(x)   { 1 }  { 2 };
x.do{ |i| i };
x.collect  { |i| i }.sum;
f(1){ 2 };
Routine{ 1.wait };
x.do #{ 1 };
x.do{:x, x <- y };
// not trailing closures
f({ 1 }, { 2 });
g = { 1 };
x.do(  { 1 }  );
while{ x }{ y };
)
