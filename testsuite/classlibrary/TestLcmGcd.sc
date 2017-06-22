
TestLcmGcd : UnitTest {


	// lattice theoretic laws
	// https://en.wikipedia.org/wiki/Least_common_multiple#Lattice-theoretic

	callTest_commutative_lcm { |a, b|
		var x = lcm(a, b);
		var y = lcm(b, a);
		this.assertEquals(x, y, "lcm(%, %) = lcm(%, %) should be valid"
			.format(a, b, b, a))
	}
	callTest_commutative_gcd { |a, b|
		var x = gcd(a, b);
		var y = gcd(b, a);
		this.assertEquals(x, y, "gcd(%, %) = gcd(%, %) should be valid"
			.format(a, b, b, a))
	}

	callTest_associative_lcm { |a, b, c|
		var x = lcm(a, lcm(b, c));
		var y = lcm(lcm(a, b), c);
		this.assertEquals(x, y, "lcm(%, lcm(%, %)) = lcm(lcm(%, %), %) should be valid"
			.format(a, b, c, a, b, c))
	}

	callTest_associative_gcd { |a, b, c|
		var x = gcd(a, gcd(b, c));
		var y = gcd(gcd(a, b), c);
		this.assertEquals(x, y, "gcd(%, gcd(%, %)) = gcd(gcd(%, %), %) should be valid"
			.format(a, b, c, a, b, c))
	}

	callTest_absorption_lcm { |a, b|
		var x = lcm(a, gcd(a, b));
		this.assertEquals(a, x, "lcm(%, gcd(%, %)) should be valid"
			.format(a, a, b))
	}

	callTest_absorption_gcd { |a, b|
		var x = gcd(a, lcm(a, b));
		this.assertEquals(a, x, "gcd(%, lcm(%, %)) should be valid"
			.format(a, a, b))
	}

	callTest_idempotency_lcm { |a|
		var x = lcm(a, a);
		this.assertEquals(a, x, "% = lcm(%, %) should be valid"
			.format(a, a, a))
	}

	callTest_idempotency_gcd { |a|
		var x = gcd(a, a);
		this.assertEquals(a, x, "% = gcd(%, %) should be valid"
			.format(a, a, a))
	}

	callTest_distributive_lcm { |a, b, c|
		var x = lcm(a, gcd(b, c));
		var y = gcd(lcm(a, b), lcm(a, c));
		this.assertEquals(x, y, "lcm(%, gcd(%, %)) = gcd(lcm(%, %), lcm(%, %)) should be valid"
			.format(a, b, c, a, b, a, c))
	}

	callTest_distributive_gcd { |a, b, c|
		var x = gcd(a, lcm(b, c));
		var y = lcm(gcd(a, b), gcd(a, c));
		this.assertEquals(x, y, "gcd(%, lcm(%, %)) = lcm(gcd(%, %), gcd(%, %)) should be valid"
			.format(a, b, c, a, b, a, c))
	}

	callTest_selfDuality { |a, b, c|

		var x = gcd(gcd(lcm(a, b), lcm(b, c)), lcm(a, c));
		var y = lcm(lcm(gcd(a, b), gcd(b, c)), gcd(a, c));
		this.assertEquals(x, y,
			"gcd(gcd(lcm(%, %), lcm(%, %)), lcm(%, %)) = lcm(lcm(gcd(%, %), gcd(%, %)), gcd(%, %))"
			"should be valid".format(
				a, b, b, c, a, c,
				a, b, b, c, a, c
			)
		)
	}

	// see E.E. McDonnell, A Notation for the GCD and LCM Functions
	// http://www.jsoftware.com/papers/eem/gcd.htm

	callTest_booleanLattic_lcm_and { |a, b|
		var a_bool = booleanValue(a);
		var b_bool = booleanValue(b);
		var a_num = binaryValue(a);
		var b_num = binaryValue(b);
		var x = and(a_bool, b_bool);
		var y = lcm(a_num, b_num);
		this.assertEquals(x, booleanValue(y), "lcm(%, %) should be equivalent to and(%, %)".format(
			a_num, b_num, a_bool, b_bool
		));
	}

	callTest_booleanLattic_gcd_or { |a, b|
		var a_bool = booleanValue(a);
		var b_bool = booleanValue(b);
		var a_num = binaryValue(a);
		var b_num = binaryValue(b);
		var x = or(a_bool, b_bool);
		var y = gcd(a_num, b_num);
		this.assertEquals(x, booleanValue(y), "gcd(%, %) should be equivalent to or(%, %)".format(
			a_num, b_num, a_bool, b_bool
		));
	}


	// tests


	test_commutative {
		var operands = (-4..4).dup(2).allTuples;
		operands.do { |pair| this.callTest_commutative_lcm(*pair) };
		operands.do { |pair| this.callTest_commutative_gcd(*pair) };
	}

	test_associative {
		var operands = (-4..4).dup(3).allTuples;
		operands.do { |triple| this.callTest_associative_lcm(*triple) };
	}

	test_idempotency {
		var operands = (-4..4);
		operands.do { |x| this.callTest_idempotency_lcm(x) };
		operands.do { |x| this.callTest_idempotency_gcd(x) };
	}


	// the following tests exclude zero, until a consistent implementation for zero is found

	test_absorption {
		//var operands = (-4..4).dup(2).allTuples;
		var operands = (-4..4).removing(0).dup(2).allTuples;
		operands.do { |pair| this.callTest_absorption_lcm(*pair) };
		operands.do { |pair| this.callTest_absorption_gcd(*pair) };
	}

	test_absorptionNonNegative {
		var operands = (0..4).dup(2).allTuples;
		operands.do { |pair| this.callTest_absorption_lcm(*pair) };
		operands.do { |pair| this.callTest_absorption_gcd(*pair) };
	}

	test_distributive {
		//var operands = (-4..4).dup(3).allTuples;
		var operands = (-4..4).removing(0).dup(3).allTuples;
		operands.do { |triple| this.callTest_distributive_lcm(*triple) };
		operands.do { |triple| this.callTest_distributive_gcd(*triple) };

	}

	test_distributiveNonNegative {
		var operands = (0..4).dup(3).allTuples;
		operands.do { |triple| this.callTest_distributive_lcm(*triple) };
		operands.do { |triple| this.callTest_distributive_gcd(*triple) };

	}

	test_selfDuality {
		//var operands = (-4..4).dup(3).allTuples;
		var operands = (-4..4).removing(0).dup(3).allTuples;
		operands.do { |triple| this.callTest_selfDuality(*triple) };
	}

	test_selfDualityNonNegative {
		var operands = (0..4).removing(0).dup(3).allTuples;
		operands.do { |triple| this.callTest_selfDuality(*triple) };
	}

	test_booleanLattice {
		var operands = [true, false].dup(2).allTuples;
		operands.do { |pair| this.callTest_booleanLattic_lcm_and(*pair) };
	}

}
