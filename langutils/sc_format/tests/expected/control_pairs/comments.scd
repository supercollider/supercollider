f = { |x|
	// Comments in the pairs, after the trailing closures rewrite, stay with what they follow.
	switch(x)
		{ \a } { 1 }
		{ \b } {
		2 // two
	}
		{ 3 };
	switch(x)
		{ \a } { /* one */ 1 }
		{ \b } {
		2
	}
		{ 3 /* default */ };
	case
		{ x > 1 /* big */ } { 1 }
		{ x < 0 } {
		2
	};
	// Not rewritten: a comment between the pairs, before or after the commas.
	switch(x, \a, { 1 }, // first
		\b, { 2 }, // second
		{ 3 });
	switch(x, \a, { 1 } /* first */, \b, {
		2
	});
	case({ x > 1 }, { 1 }, // first
		{ x < 0 }, { 2 });
	// Already trailing, with comments on their own lines and after `switch(x)`.
	switch(x) // which
	// a
	{ \a } { 1 }
	/* b */
	{ \b } { 2 };
	case /* c */
	{ x > 1 } { 1 } /* first */
		{ x < 0 } {
		2
	};
	x
};
