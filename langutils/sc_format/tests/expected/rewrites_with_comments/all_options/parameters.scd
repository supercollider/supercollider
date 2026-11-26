// Comments in parameter lists.
(
	// `arg` to pipes, with comments between the parameters.
	a = { |x, /* c */ y| x };
	b = { |x /* c */, y| x };
	c = { |x, // c
			y|
		x };
	d = { |x // c
			, y|
		x };
	e = { |x /* a */, /* b */ y /* c */ = 1, z| x };
	// Before `arg` or after the `;`.
	f = { /* c */ |x| x };
	g = { // c
		|x|
		x
	};
	h = { |x| /* c */ x };
	i = { |x| // c
		x
	};
	j = { |x| /* c */x };
	k = { |x| // c
		x
	};
	// Not changed: a comment next to `arg` or the `;`.
	l = { arg /* c */ x; x };
	m = { arg // c
			x;
		x };
	n = { arg x /* c */; x };
	o = { arg x // c
			;
		x };
	p = { arg x, y /* c */; x };
	q = { arg x = 1 /* c */; x };
	// An empty `arg;` goes, keeping a comment before or after it.
	r = { /* c */ 1 };
	s = { // c
		1
	};
	t = { /* c */ 1 };
	u = { // c
		1
	};
	v = { /* a */ /* b */ 1 };
	w = { arg /* c */; 1 };
	x = { arg // c
			;
		1 };
	// A default in brackets, around comments by the `=` or in the value.
	a = { |w /* c */ = (4.dbamp)| w };
	b = { |w = /* c */ (4.dbamp)| w };
	c = { |w = (4 /* c */ .dbamp)| w };
	d = { |w = (4.dbamp) /* c */, x| w };
	e = { |w = (4.dbamp) // c
			, x|
		w };
	f = { |w // c
			= (4.dbamp), x|
		w };
	g = { |w = // c
			(4.dbamp), x|
		w };
	h = { |w = (4.dbamp), // c
			x = (Server.default) /* s */, y|
		w };
	i = { |w = (4.dbamp) /* c */, x| w };
	// Missing commas, with a comment between the parameters.
	j = { |x, /* c */ y| x };
	k = { |x, // c
			y|
		x };
	l = { |x = 1, /* c */ y = 2| x };
	m = { |x /* c */ ...rest| x };
	// Missing `=`, with a comment between the name and the value.
	n = { |x = /* c */ 1| x };
	o = { |x = // c
			1|
		x };
	p = { |x = /* c */ (1)| x };
	q = { |x = 1 /* c */, y = 2| x };
	r = { |x = 1, /* a */ y = /* b */ 2| x };
	// Every rewrite in one list.
	s = { |a = (1) /* one */, b = (2.sqrt), // two
			c|
		a };
	t = { |a = 1, /* one */ b = 2, // two
			c = (3) ...rest|
		a };
)
