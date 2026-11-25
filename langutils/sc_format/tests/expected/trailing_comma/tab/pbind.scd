Pbind(
	\instrument, \default,
	\freq,       440,
	\dur,        0.25,
	\amp,        0.1,
).play;

// The first row moves down from the opener's line.
Pbind(
	\instrument, \default,
	\freq,       440,
	\dur,        0.25,
	\legato,     0.8,
);

// A row of four among rows of two: one per line.
Pbind(
	\instrument, \default,
	\freq,       440,      \dur, 0.25,
	\legato,     0.8,
);

// Keyword arguments are one element each.
Synth(
	\default,
	[
		\freq,
		440,
	],
	target: s,
	addAction: \addToHead,
);
