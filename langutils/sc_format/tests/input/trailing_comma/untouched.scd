// No trailing comma: as written.
Pbind(\freq, 440,
	\dur, 0.25);
[1, 2,
	3];

// Not lists: parameters, generators, indexes.
f = { |a, b| a + b };
g = {: x, x <- (1..3) };
a[0, 1];

// A comma starting a line joins the line before.
[1
	, 2
	, 3,
];
