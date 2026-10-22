// Each item of a region is compiled into its own function, inside an SCRegion, whether it is an expression or a block.
// The list's result is the last region's.
// expect: -> 7
// expect: not poisoned
1 + 2;
(
	var b = 3;
	b + 1
);
a = 5;
(|x = 6| x);
{ |y| if (y) { 1 } { 2 } }.value(true);
7
