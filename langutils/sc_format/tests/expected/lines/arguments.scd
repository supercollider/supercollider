(
a = foo(
	1,
	2,
	freq: 440,
	amp: 0.1
);
b = Pbind(
	\freq,
	440,
	\dur,
	0.25
);
c = foo.(
	1,
	2
);
// Not split: blocks over several lines, an index, a closer alone on its line.
d = if(x, {
	1
}, {
	2
});
e = x[0,
	1];
f = foo(1, {
	2
}
);
)
