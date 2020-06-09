TestComplex : UnitTest {


	test_reciprocal {

		var a, c, x, y, test, failed, message;

		test = { |a, b|
			a == b or: {
				a.real.isNaN and: { b.real.isNaN }
				and: { a.imag.isNaN } and: { b.imag.isNaN }
			}
		};

		a = [0, 1, 3, inf, pi, 1.0, 1/3, 0/0, 2147483647];
		c = (a ++ a.neg).dup.allTuples.collect(Complex(*_));
		x = c.collect(1/_);
		y = c.collect(_.reciprocal);

		failed = (0..x.lastIndex).reject { |i| test.value(x[i], y[i]) };
		message = "1/x should be the same as x.reciprocal for all complex numbers.";
		failed.do { |i| message = message ++ "\nfailed:" ++ c[i] };
		this.assert(failed.isEmpty, message);

	}

	// regression test for #5000: UGen:performBinaryOpOnUGen should return even if operand's isValidUGenInput is false
	test_performBinaryOpOnUGen {
		var complex = Complex(1,1), ugen = DC.kr(1);
		this.assertEquals( (ugen * complex).class, (complex * ugen).class, "(UGen * Complex) should return same class as (Complex * UGen)")
	}

}
