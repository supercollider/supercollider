(
a = { |x, y| x + y };
b = { 1 };
c = {
	1
};
// A default outside the pipe grammar's literals goes in brackets.
d = { |w = (4.dbamp), x = (y), z = (-1.abs), s = (Server.default)| w };
// Literals, and a default already in brackets, don't.
e = { |i = -1, f = 2.5, p = 2pi, s = \sym, q = 'q', t = "t", c = $c, b = true, n = nil, a = [1, 2], h = #[3], d = (k: 1), g = { 1 }, r = (1 + 2)| i };
f = { |a ...rest| rest };
g = { |freq = 440, // the frequency
	amp = 0.1|
	freq * amp
};
h = { |x| // a comment right after
	x
};
// Not changed: a comment next to `arg` or the `;`.
k = { arg /* x */ x; x };
)
