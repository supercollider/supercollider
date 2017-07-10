/*

Unit tests for lcd and gcd, including their extension to numbers <= 0

*/


TestLcmGcd : UnitTest {

	// equalities from lattice theoretic laws
	// https://en.wikipedia.org/wiki/Least_common_multiple#Lattice-theoretic

	callTest_commutative_lcm { |a, b|
		var x = lcm(a, b);
		var y = lcm(b, a);
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_commutative_lcm(a, b)) };
		^pass
	}

	callTest_commutative_gcd { |a, b|
		var x = gcd(a, b);
		var y = gcd(b, a);
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_commutative_gcd(a, b)) };
		^pass
	}

	callTest_associative_lcm { |a, b, c|
		var x = lcm(a, lcm(b, c));
		var y = lcm(lcm(a, b), c);
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_associative_lcm(a, b, c)) };
		^pass
	}

	callTest_associative_gcd { |a, b, c|
		var x = gcd(a, gcd(b, c));
		var y = gcd(gcd(a, b), c);
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_associative_gcd(a, b, c)) };
		^pass
	}

	callTest_absorption_lcm { |a, b|
		var x = lcm(a, gcd(a, b));
		var pass = a == x;
		if(pass.not) { this.failed(thisMethod, this.assertion_absorption_lcm(a, b)) };
		^pass
	}

	callTest_absorption_gcd { |a, b|
		var x = gcd(a, lcm(a, b));
		var pass = a == x;
		if(pass.not) { this.failed(thisMethod, this.assertion_absorption_gcd(a, b)) };
		^pass
	}

	callTest_idempotence_lcm { |a|
		var x = lcm(a, a);
		var pass = a == x;
		if(pass.not) { this.failed(thisMethod, this.assertion_idempotence_lcm(a)) };
		^pass
	}

	callTest_idempotence_gcd { |a|
		var x = gcd(a, a);
		var pass = a == x;
		if(pass.not) { this.failed(thisMethod, this.assertion_idempotence_gcd(a)) };
		^pass
	}

	callTest_distributive_lcm { |a, b, c|
		var x = lcm(a, gcd(b, c));
		var y = gcd(lcm(a, b), lcm(a, c));
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_distributive_lcm(a, b, c)) };
		^pass
	}

	callTest_distributive_gcd { |a, b, c|
		var x = gcd(a, lcm(b, c));
		var y = lcm(gcd(a, b), gcd(a, c));
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_distributive_gcd(a, b, c)) };
		^pass
	}

	callTest_selfDuality { |a, b, c|

		var x = gcd(gcd(lcm(a, b), lcm(b, c)), lcm(a, c));
		var y = lcm(lcm(gcd(a, b), gcd(b, c)), gcd(a, c));
		var pass = x == y;
		if(pass.not) { this.failed(thisMethod, this.assertion_selfDuality(a, b, c)) };
		^pass
	}


	// equalities expressed as formulas

	assertion_commutative_lcm { |a, b|
		^"lcm(%, %) = lcm(%, %)".format(a, b, b, a)
	}
	assertion_commutative_gcd { |a, b|
		^"gcd(%, %) = gcd(%, %)".format(a, b, b, a)
	}
	assertion_associative_lcm { |a, b, c|
		^"lcm(%, lcm(%, %)) = lcm(lcm(%, %), %)".format(a, b, c, a, b, c)
	}
	assertion_associative_gcd { |a, b, c|
		^"gcd(%, gcd(%, %)) = gcd(gcd(%, %), %)".format(a, b, c, a, b, c)
	}
	assertion_absorption_lcm { |a, b|
		^"% = lcm(%, gcd(%, %))".format(a, a, b, b, a)
	}
	assertion_absorption_gcd { |a, b|
		^"% = gcd(%, lcm(%, %))".format(a, a, b, b, a)
	}
	assertion_idempotence_lcm { |a|
		^"% = lcm(%, %)".format(a, a, a)
	}
	assertion_idempotence_gcd { |a|
		^"% = gcd(%, %)".format(a, a, a)
	}
	assertion_distributive_lcm { |a, b, c|
		^"lcm(%, gcd(%, %)) = gcd(lcm(%, %), lcm(%, %))".format(a, b, c, a, b, a, c)
	}
	assertion_distributive_gcd { |a, b, c|
		^"gcd(%, lcm(%, %)) = lcm(gcd(%, %), gcd(%, %))".format(a, b, c, a, b, a, c)
	}
	assertion_selfDuality { |a, b, c|
		^"gcd(gcd(lcm(%, %), lcm(%, %)), lcm(%, %)) = lcm(lcm(gcd(%, %), gcd(%, %)), gcd(%, %))".format(
			a, b, b, c, a, c,
			a, b, b, c, a, c
		)
	}


	// lcm is equvalent to logical and
	// gcd is equivalen to logical or
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



	// the tests

	test_commutative {

		var operands = (-1..1).dup(2).allTuples;
		this.assert(
			operands.every { |pair| this.callTest_commutative_lcm(*pair) },
			this.assertion_commutative_lcm("x", "y") + "is valid in the range (-1..1)"
		);
		this.assert(
			operands.every { |pair| this.callTest_commutative_gcd(*pair) },
			this.assertion_commutative_gcd("x", "y") + "is valid in the range (-1..1)"
		);

	}

	test_associative {

		var operands = (-1..1).dup(3).allTuples;
		this.assert(
			operands.every { |triple| this.callTest_associative_lcm(*triple) },
			this.assertion_associative_lcm("x", "y", "z") + "is valid in the range (-1..1)"
		);
		this.assert(
			operands.every { |triple| this.callTest_associative_gcd(*triple) },
			this.assertion_associative_gcd("x", "y", "z") + "is valid in the range (-1..1)"
		);

	}

	test_idempotence {

		var operands = (-1..1);
		this.assert(
			operands.every { |item| this.callTest_idempotence_lcm(item) },
			this.assertion_idempotence_lcm("x") + "is valid in the range (-1..1)"
		);
		this.assert(
			operands.every { |item| this.callTest_idempotence_gcd(item) },
			this.assertion_idempotence_gcd("x") + "is valid in the range (-1..1)"
		);
	}

	test_absorption_lcm {

		var operands = (-1..1).dup(2).allTuples;
		this.assert(
			operands.every { |pair| this.callTest_absorption_lcm(*pair) },
			this.assertion_absorption_lcm("x", "y") + "is valid in the range (-1..1)"
		);
		this.assert(
			operands.every { |pair| this.callTest_absorption_gcd(*pair) },
			this.assertion_absorption_gcd("x", "y") + "is valid in the range (-1..1)"
		);

	}

	test_distributive {

		var operands = (-1..1).dup(3).allTuples;
		this.assert(
			operands.every { |triple| this.callTest_distributive_lcm(*triple) },
			this.assertion_distributive_lcm("x", "y", "z") + "is valid in the range (-1..1)"
		);
		this.assert(
			operands.every { |triple| this.callTest_distributive_gcd(*triple) },
			this.assertion_distributive_gcd("x", "y", "z") + "is valid in the range (-1..1)"
		);

	}

	test_selfDuality {

		var operands = (-1..1).dup(3).allTuples;
		this.assert(
			operands.every { |triple| this.callTest_selfDuality(*triple) },
			this.assertion_selfDuality("x", "y", "z") + "is valid in the range (-1..1)"
		);

	}


	test_booleanLattice {

		var operands = [true, false].dup(2).allTuples;
		operands.do { |pair| this.callTest_booleanLattic_lcm_and(*pair) };
		operands.do { |pair| this.callTest_booleanLattic_gcd_or(*pair) };

	}
}

