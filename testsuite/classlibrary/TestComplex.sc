//# VERSION 3.16
TestComplex : UnitTest {


	test_reciprocal {

		var a;

		var test = { |a, b|
			a == b or: {
				a.real.isNaN and: { b.real.isNaN }
				and: { a.imag.isNaN } and: { b.imag.isNaN }
			}
		};

		a = [0, 1, 3, inf, pi, 1.0, 1/3, 0/0, 2147483647];
		var c = (a ++ a.neg).dup.allTuples.collect(Complex(*_));
		var x = c.collect(1/_);
		var y = c.collect(_.reciprocal);

		var failed = (0..x.lastIndex).reject { |i| test.value(x[i], y[i]) };
		var message = "1/x should be the same as x.reciprocal for all complex numbers.";
		failed.do { |i| message = message ++ "\nfailed:" ++ c[i] };
		this.assert(failed.isEmpty, message);

	}

	// regression test for #5000: UGen:performBinaryOpOnUGen should return even if operand's isValidUGenInput is false
	test_performBinaryOpOnUGen {
		var complex = Complex(1,1), ugen = DC.kr(1);
		this.assertEquals( (ugen * complex).class, (complex * ugen).class, "(UGen * Complex) should return same class as (Complex * UGen)")
	}

}
