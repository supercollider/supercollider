// A comment ending a row lines up as one more column, and isn't an element.
~notes = [
	60, 62, // first
	64, 65, 67, 69, // too many, so one per line
];

~notes = [
	60, 62, // first
	1064, 65, // second
	67,
];

// A comment on its own line doesn't end the rows.
~notes = [
	60, 62,
	// middle
	64, 65,
];

// A comment between elements on one line: expanded, not aligned.
~notes = [
	60, /* c */ 62,
	64, 65,
];

// A comment after the last comma stays at the end of its line.
~notes = [1, 2, /* end */];
f(a, // first
	b,);
