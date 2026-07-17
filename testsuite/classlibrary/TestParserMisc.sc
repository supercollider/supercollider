// An attempt to test most of the moderately complex rules in the parser.
// It is not meant as a replacement for fuzz testing, but just as a quick and dirty answer to the question
//   'did I just definitively break everything?'
TestParserRules : UnitTest {
	test_floatp {
		this.assertEquals(pi.round(0.001), 3.142);
		this.assertEquals(-pi.round(0.001), -3.142);
		this.assertEquals(3pi, 3 * pi);
		this.assertEquals(-3pi, -3 * pi);
		this.assertEquals(-3.0pi, -3.0 * pi);
	}

	test_accidental {
		// This is odd. This means -3 sharpen it twice, not 3 double sharp, negate.
		this.assertEquals(-3ss, -2.8);
		this.assertEquals(3bb, 2.8);
	}

	test_listlit {
		var a = Array();
		a = a.add(1);
		a = a.add(2);
		this.assertEquals([], Array.new);
		this.assertEquals(#[], Array.new);
		this.assertEquals([1, 2], a);
		this.assertEquals(#[1, 2], a);
	}

	test_argdecls1 {
		var f;
		f = { arg foo; };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { arg foo, bar; };
		this.assertEquals(f.def.argNames[0], \foo);
		this.assertEquals(f.def.argNames[1], \bar);

		f = { arg foo = 1; };
		this.assertEquals(f.def.argNames[0], \foo);
		f = { arg foo(1); };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { arg ...foo; };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { |foo| };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { |foo, bar| };
		this.assertEquals(f.def.argNames[0], \foo);
		this.assertEquals(f.def.argNames[1], \bar);

		f = { |foo 1| };
		this.assertEquals(f.def.argNames[0], \foo);
		f = { |foo=1| };
		this.assertEquals(f.def.argNames[0], \foo);
		f = { |foo=(1; 1)| };
		this.assertEquals(f.def.argNames[0], \foo);
		f = { |foo(1; 1)| };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { |...foo| };
		this.assertEquals(f.def.argNames[0], \foo);

		f = { |...foo, bar| };
		this.assertEquals(f.def.argNames[0], \foo);
		this.assertEquals(f.def.argNames[1], \bar);
	}

	test_vardecls {
		var f;
		f = { var foo; };
		this.assertEquals(f.def.varNames[0], \foo);

		f = { var foo, bar; };
		this.assertEquals(f.def.varNames[0], \foo);
		this.assertEquals(f.def.varNames[1], \bar);

		f = { var foo = 1; };
		this.assertEquals(f.def.varNames[0], \foo);
		f = { var foo(1); };
		this.assertEquals(f.def.varNames[0], \foo);
	}

	test_mavar {
		var a, b, c, rest;

		#a, b, c = [1, 2, 3];
		this.assertEquals(a, 1);
		this.assertEquals(b, 2);
		this.assertEquals(c, 3);

		#a, b, c ...rest = #[1, 2, 3, 4, 5, 6];
		this.assertEquals(a, 1);
		this.assertEquals(b, 2);
		this.assertEquals(c, 3);
		this.assertEquals(rest, [4, 5, 6]);
	}

	test_arrayelems {
		var a = [ 1; 2; 3;, \a; \b : 4; 5, keybinop: 1,];
		this.assertEquals(a[0], 3);
		this.assertEquals(a[1], \b);
		this.assertEquals(a[2], 5);
		this.assertEquals(a[3], \keybinop);
		this.assertEquals(a[4], 1);
		this.assertEquals(a.size, 5);
	}

	test_expr {
		// Just some expressions, not all.
		var a, b;
		this.assertEquals((`~f = #a, b = [1, 2]).value, (`[1, 2]).value);

		this.assertEquals([1].[0] = 1, [1]);
		this.assertEquals([].[0], nil);
		this.assertEquals([1][0] = 1, [1]);
		this.assertEquals([][0], nil);
	}

	test_valrange3 {
		// calls seriesIter
		var a;
		a = (:..1);
		this.assertEquals(a.next, 0);
		this.assertEquals(a.next, 1);
		this.assertEquals(a.next, nil);


		a = (:0;1;2;0..);
		10.do { |i|
			this.assertEquals(a.next, i)
		};

		a = (:0..2);
		this.assertEquals(a.next, 0);
		this.assertEquals(a.next, 1);
		this.assertEquals(a.next, 2);
		this.assertEquals(a.next, nil);

		a = (:0, 2..10);
		this.assertEquals(a.next, 0);
		this.assertEquals(a.next, 2);
		this.assertEquals(a.next, 4);
		this.assertEquals(a.next, 6);
		this.assertEquals(a.next, 8);
		this.assertEquals(a.next, 10);
		this.assertEquals(a.next, nil);

		a = (:0, 2..);
		10.do { |i|
			this.assertEquals(a.next, i * 2)
		};
	}

	valrange2Inf1 { |n|
		(0..).do{ |i|
			if (i >= n) { ^i }
		};
	}

	test_valrange2 {
		this.assertEquals(this.valrange2Inf1(10), 10);
		this.assertEquals((0..10), [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
	}

	test_valrangexd {
		var r = [1, 2, 3, 4, 5];
		this.assertEquals(r.[0..], r);
		this.assertEquals(r.[1..], [2, 3, 4, 5]);

		this.assertEquals(r[0..], r);
		this.assertEquals(r[1..], [2, 3, 4, 5]);
	}

	test_list_comp {
		// Lifted out of the help documentation.
		// Incomplete.

		this.assertEquals(
			all {:[x,y], x <- (1..5), y <- (1..x), (x+y).isPrime },
			[ [ 1, 1 ], [ 2, 1 ], [ 3, 2 ], [ 4, 1 ], [ 4, 3 ], [ 5, 2 ] ]
		);

		this.assertEquals(
			all {:[x,y], x <- (1..3), y <- [\a,\b,\c] },
			[ [ 1, \a ], [ 1, \b ], [ 1, \c ], [ 2, \a ], [ 2, \b ], [ 2, \c ], [ 3, \a ], [ 3, \b ], [ 3, \c ] ]
		);

		this.assertEquals(
			all {:y, x <- (1..4), y <- (x..1) },
			[ 1, 2, 1, 3, 2, 1, 4, 3, 2, 1 ]
		);

		this.assertEquals(
			all {: x, x <- (1..3) },
			[ 1, 2, 3 ]
		);

		this.assertEquals(
			all {: [x,y], x <- (1..2), y <- (10,20..30) },
			[ [ 1, 10 ], [ 1, 20 ], [ 1, 30 ], [ 2, 10 ], [ 2, 20 ], [ 2, 30 ] ]
		);

		this.assertEquals(
			all {: x, x <- (1..3), y <- (1..4-x) },
			[ 1, 1, 1, 2, 2, 3 ]
		);

		this.assertEquals(
			all {: x, x <- (0..10), x.odd },
			[ 1, 3, 5, 7, 9 ]
		);

		this.assertEquals(
			all {: z, x <- (1..20), var z = (x*x-x) div: 2, z.odd },
			[ 1, 3, 15, 21, 45, 55, 91, 105, 153, 171 ]
		);

		// testing side effects, don't remove postln.
		this.assertEquals(
			all {: z, x <- (1..20), var z = (x*x-x) div: 2, :: [x,z].postln, z.even },
			[0, 6, 10, 28, 36, 66, 78, 120, 136, 190]
		);

		this.assertEquals(
			all {: z, x <- (1..20), var z = (x*x-x) div: 2,  :: [x,z].postln, :while z < 50 },
			 [0, 1, 3, 6, 10, 15, 21, 28, 36, 45]
		);
	}
}
