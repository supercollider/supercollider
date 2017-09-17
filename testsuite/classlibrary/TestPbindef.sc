TestPbindef : UnitTest {

	test_pbindef_arg_order_new {
		var pairs;

		Pbindef(\test).clear;
		Pbindef(\test, \a, 1, \b, 2, \c, 3);

		pairs = Pbindef(\test).source.pairs;
		this.assertEquals(pairs[0,2..], [\a, \b, \c],
			"Pbindef key order should correspond to fresh argument order"
		);

	}

	test_pbindef_arg_order_reverse {
		var pairs;

		Pbindef(\test).clear;
		Pbindef(\test, \a, 1, \b, 2, \c, 3);
		Pbindef(\test, \c, 3, \b, 2, \a, 1);

		pairs = Pbindef(\test).source.pairs;
		this.assertEquals(pairs[0,2..], [\c, \b, \a],
			"Pbindef key order should correspond to reversed argument order"
		);

	}

	test_pbindef_arg_order_insert_other {
		var pairs;

		Pbindef(\test).clear;
		Pbindef(\test, \a, 1);
		Pbindef(\test, \b, 2);

		pairs = Pbindef(\test).source.pairs;
		this.assertEquals(pairs[0,2..], [\a, \b],
			"Pbindef key order should correspond to added argument order"
		);

	}

	test_pbindef_arg_order_insert {
		var pairs;

		Pbindef(\test).clear;
		Pbindef(\test, \a, 1, \b, 2, \c, 3);
		Pbindef(\test, \y, 5, \a, 1, \x, 0, \b, 2, \c, 3, \z, 9);

		pairs = Pbindef(\test).source.pairs;
		this.assertEquals(pairs[0,2..], [\y, \a, \x, \b, \c, \z],
			"Pbindef key order should correspond to interleaved argument"
			" order after insertion of new key-value pairs"
		);
	}

	test_pbindef_keeps_multiples {
		var pairs;

		Pbindef(\test).clear;
		Pbindef(\test, \x, 6, \x, 7, \y, 8);
		Pbindef(\test, \a, 0, \y, 8, \y, 20, \x, 10);

		pairs = Pbindef(\test).source.pairs;
		(1,3..pairs.lastIndex).do { |i| pairs[i] = pairs[i].source };
		this.assertEquals(pairs, [\a, 0, \y, 8,  \y, 20, \x, 10],
			"Pbindef should keep duplicate key with separate values");
	}

	test_pbindef_arg_order_fromPbind {
		var pairs;

		Pdef(\test).clear;
		Pdef(\test, Pbind(\a, 1, \c, 3));
		Pbindef(\test, \a, 1, \b, 2, \c, 3);

		pairs = Pbindef(\test).source.pairs;
		this.assertEquals(pairs[0,2..], [\a, \b, \c],
			"Pbindef key order should correspond to argument order when"
			" converting from Pdef of a Pbind to a Pbindef"
		);

	}

	test_pbindef_continuity {
		var pairs1, pairs2;

		Pdef(\test).clear;

		Pbindef(\test, \a, 1, \b, 2, \c, 3);
		pairs1 = Pbindef(\test).source.pairs;

		Pbindef(\test, \a, 2, \b, 3, \c, 4, \d, 5);
		pairs2 = Pbindef(\test).source.pairs.keep(6);



		this.assert(
			pairs2.every { |patternproxy, i| patternproxy ===  pairs1[i] },
			"Pbindef should add new values to existing pattern proxy where possible"
		);

	}

}