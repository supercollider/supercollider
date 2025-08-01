TestFloat : UnitTest {

	test_storeOn {
		var stored;
		stored = String.streamContents({ arg stream; stream <<< 1e-05});
		this.assert(stored !=  "1e-05.0", "scientific notation should not have a .0 at the end");
		this.assertEquals(stored ,  "1e-05", "scientific notation");

		stored = String.streamContents({ arg stream; stream <<< 2.0 });
		this.assertEquals(stored ,  "2.0", "2.0 should be stored as 2.0");

		stored = String.streamContents({ arg stream; stream <<< 2 });
		this.assertEquals(stored ,  "2", "2 should be stored as 2");

	}

	test_asString_zero {
		this.assertEquals(0.0.asString, "0.0")
	}

	test_asString_one {
		this.assertEquals(1.0.asString, "1.0")
	}

	test_asString_minusOne {
		this.assertEquals(-1.0.asString, "-1.0")
	}

	test_asString_almostTwo_isTwoPointZero {
		this.assertEquals(1.9999999999999999.asString, "2.0")
	}

	test_asString_onePointFifteen {
		this.assertEquals(1.15.asString, "1.15")
	}

	test_asString_inf {
		this.assertEquals(inf.asString, "inf")
	}

	test_asString_minusInf {
		this.assertEquals(-inf.asString, "-inf")
	}

	test_asString_nan {
		var zeroDivString = (0/0).asString;
		this.assertEquals(zeroDivString, "nan");
	}

	test_asString_largeNumberUsesExp {
		this.assertEquals((1e26).asString, "1e+26")
	}

	test_asCompileString {
		this.assertEquals(1.0.asCompileString.compile.(), 1.0, "1.0 as compile string");
		this.assertEquals(inf.asCompileString.compile.(), inf, "inf as compile string");
		this.assertEquals(-inf.asCompileString.compile.(), -inf, "-inf as compile string");

		// Since 'nan' isn't a keyword in sc, there is no way to perform this check.
		// This means the conversion from value to string to value isn't complete over the float type.
	}

}
