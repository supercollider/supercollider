// A comment inside the pipes or after `arg` keeps the parameters on the brace line.
a = { |x /* c */|
	x
};
b = { | /* c */ x|
	x
};
c = { |x = 1 /* c */, y|
	x
};
d = { arg x /* c */;
	x
};
e = { |x /* c */|
	x
};
