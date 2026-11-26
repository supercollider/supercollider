// An element over several lines: one per line.
~fs = [
	{ |x|
	x * 2 },
	2,
	3,
	4,
];

// An inner list that expands makes its element span lines.
~nested = [
	[
		1,
		2,
	],
	[
		3,
		4,
	],
	[
		5,
		6,
	],
	[
		7,
		8,
	],
];

// Inner lists without a trailing comma stay as written.
~nested = [
	[1, 2], [3, 4],
	[5, 6], [7, 8],
];

x.do(
	{ |i|
	i.postln
},
);

// A row's last element may span lines, as a switch's functions do.
switch(
	x,
	\low, {
		1
	},
	\middle, {
		2
	},
);

// An element that isn't its row's last spanning lines: one per line.
switch(
	x,
	\low,
	{
		1
	},
	\middle,
	{ 2 },
	\high,
	{ 3 },
);
