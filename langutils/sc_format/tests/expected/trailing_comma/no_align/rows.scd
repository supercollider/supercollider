// A table of numbers with a short last row.
~table = [
	1, 22, 333,
	4444, 5, 66,
	7, 88,
];

// All on one line: one per line.
~list = [
	1,
	2,
	3,
];

// Rows of different sizes: one per line.
~uneven = [
	1, 2,
	3, 4, 5,
	6,
];

// The last row has more: one per line.
~more = [
	1,
	2,
	3,
	4,
	5,
	6,
	7,
];

// Elements with spaces inside are measured as printed.
~pairs = [
	a+b, c,
	d, e*f,
];

// The first row may have fewer too.
~first = [
	1,
	2, 3,
	4, 5,
	6,
];

// Two rows are kept as they are.
~two = [
	1, 2, 3,
	4, 5, 6, 7, 8,
];
