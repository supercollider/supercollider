// interpreted-version: 3.16
// expect: poisoned
var a = (1 +);
var b = [(1 2), (3 + 4)];
(
	var c = 1;
	c + (2 3)
);
a + b
