(
// Inside the off lines, nothing changes, comments or not.
// sc-format off
a = { arg x /* c */, y; x.if({ 1; }, { 2; }); };
b = { |x 1 y// c
	2| x };
// sc-format on
// Just outside, the rewrites happen.
c = { arg x /* c */, y; x.if({ 1; }, { 2; }); };
// A rewrite whose edits reach into the off lines doesn't happen, the rest of the code still changes.
d = { arg x; // params
	x.if({ 1; },
// sc-format off
		{ 2; }
// sc-format on
	);
};
e = { arg x,
// sc-format off
	y;
// sc-format on
	x.if({ 1; }, { 2; });
};
f = {
	x.do({ |i| i; }); /* c */
// sc-format off
};
// sc-format on
)
// The `;` after a region goes on the line before the off lines.
(
g = 1 // c
)
/* sc-format off */
(
h = 2
)
/* sc-format on */
(
i = 3
)
