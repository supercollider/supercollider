// TestSimpleNumber.sc
// Tests for the SimpleNumber class
TestSimpleNumber : UnitTest {

	//////////////////////
	// asTimeString tests
	//////////////////////

	test_asTimeString_zero {
		var expected = "00:00:00.000";
		var actual = 0.asTimeString;
		this.assertEquals(actual, expected, "0.asTimeString");
	}

	test_asTimeString_zeroNotDroppingDays {
		var expected = "000:00:00:00.000";
		var actual = 0.asTimeString(dropDaysIfPossible: false);
		this.assertEquals(actual, expected, "0.asTimeString(dropDaysIfPossible: false)");
	}

	test_asTimeString_nearZeroRoundDown {
		var expected = "00:00:00.000";
		var actual = 0.0001.asTimeString;
		this.assertEquals(actual, expected, "0.0001.asTimeString should round down");
	}

	test_asTimeString_nearZeroRoundUp {
		var expected = "00:00:00.001";
		var actual = 0.0009.asTimeString;
		this.assertEquals(actual, expected, "0.0009.asTimeString should round up");
	}

	test_asTimeString_milliseconds {
		var expected = "00:00:00.015";
		var actual = 0.015.asTimeString;
		this.assertEquals(actual, expected, "0.015.asTimeString");
	}

	test_asTimeString_millisecondPrecisionTenths {
		var expected = "00:00:05.500";
		var actual = 5.478.asTimeString(precision: 0.1);
		this.assertEquals(actual, expected, "5.478.asTimeString(precision: 0.1) should round the time appropriately");
	}

	test_asTimeString_millisecondPrecisionHundredths {
		var expected = "00:00:05.480";
		var actual = 5.478.asTimeString(precision: 0.01);
		this.assertEquals(actual, expected, "5.478.asTimeString(precision: 0.01) should round the time appropriately");
	}

	test_asTimeString_millisecondPrecisionFiftieths {
		var expected = "00:00:05.480";
		var actual = 5.471.asTimeString(precision: 0.02);
		this.assertEquals(actual, expected, "5.471.asTimeString(precision: 0.02) should round the time appropriately");
	}

	test_asTimeString_millisecondPrecisionRounding {
		var expected = "00:00:01.000";
		var actual = 0.9999.asTimeString(precision: 0.001);
		this.assertEquals(actual, expected, "0.9999.asTimeString(precision: 0.001) (close to next integer second) should round up");
	}

	test_asTimeString_seconds {
		var expected = "00:00:07.015";
		var actual = 7.015.asTimeString;
		this.assertEquals(actual, expected, "7.015.asTimeString");
	}

	test_asTimeString_minutes {
		var expected = "00:02:07.015";
		var totalTime = 127.015;
		var actual = totalTime.asTimeString;
		this.assertEquals(actual, expected, "%.asTimeString (2+ minutes)".format(totalTime));
	}

	test_asTimeString_hours {
		var expected = "03:02:07.015";

		var threeHoursInSeconds = 3600 * 3;
		var totalTime = threeHoursInSeconds + 127.015;
		var actual = totalTime.asTimeString;
		this.assertEquals(actual, expected, "%.asTimeString (3+ hours)".format(totalTime));
	}

	test_asTimeString_days {
		var expected = "002:03:02:07.015";

		var twoDaysInSeconds = 24 * 3600 * 2;
		var threeHoursInSeconds = 3600 * 3;
		var totalTime = twoDaysInSeconds + threeHoursInSeconds + 127.015;
		var actual = totalTime.asTimeString;
		this.assertEquals(actual, expected, "%.asTimeString (2+ days)".format(totalTime));
	}

	test_asTimeString_maxDays {
		var expected = "365:00:00:00.000";

		var numSecondsIn400Days = 400 * 24 * 3600;
		var actual = numSecondsIn400Days.asTimeString;
		this.assertEquals(actual, expected, "%.asTimeString (number of days > 365)".format(numSecondsIn400Days));
	}

	test_asTimeString_maxDaysWithExtraSeconds {
		var expected = "365:00:00:10.000";

		var numSecondsIn400Days = 400 * 24 * 3600;
		var totalTime = numSecondsIn400Days + 10;
		var actual = totalTime.asTimeString;
		this.assertEquals(actual, expected, "%.asTimeString (number of days > 365 and 10 seconds)".format(totalTime));
	}

	test_asTimeString_negativeZero {
		var expected = "00:00:00.000";
		var actual = -0.0.asTimeString;
		this.assertEquals(actual, expected, "-0.0.asTimeString (negative zero)");
	}

	test_asTimeString_negativeOne {
		var expected = "-00:00:01.000";
		var actual = -1.asTimeString;
		this.assertEquals(actual, expected, "-1.asTimeString (negative one)");
	}

	test_asTimeString_negativeFraction {
		var expected = "00:00:00.000";
		var actual = -0.001.asTimeString(precision: 0.1);
		this.assertEquals(actual, expected, "-0.001.asTimeString(precision: 0.1) (negative number smaller than precision)");
	}

	test_asTimeString_negativeOneDontDropDays {
		var expected = "-000:00:00:01.000";
		var actual = -1.asTimeString(dropDaysIfPossible: false);
		this.assertEquals(actual, expected, "-1.asTimeString(dropDaysIfPossible: false) (negative value)");
	}

	test_softRound {
		var val;
		var testF = {|vals, g, t, s| vals.collect({|num| num.softRound(g, t, s)})};

		val = [-1.3, -1, -0.7, -0.5, -0.3, 0, 0.2, 0.5, 0.7, 1, 1.2, 1.8, 2];
		this.assertEquals(testF.(val, 1,0,1), [ -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2 ], "Test 1: softRound(1, 0, 1)");

		val = [-1, -0.7, -0.5, -0.3, -0.25, -0.2, 0, 0.2, 0.25, 0.3, 0.6, 0.8, 1];
		this.assertEquals(testF.(val,0.5,0,1), [ -1, -0.5, -0.5, -0.5, 0, 0, 0, 0, 0.5, 0.5, 0.5, 1, 1 ], "Test 2: softRound(0.5, 0, 1)");

		val = [-0.8, -0.7, -0.6 -0.4, -0.3, -0.2, 0, 0.2, 0.3, 0.4, 0.6, 0.7, 0.8, 1];
		this.assertEquals(testF.(val,1,0.3,1), [ -0.8, -1, -1, -0.3, -0.2, 0, 0.2, 0.3, 0, 1, 1, 0.8, 1 ], "Test 3: softRound(1, 0.3, 1)");

		val = [-1, -0.8, -0.2, 0, 0.2, 0.8, 1];
		this.assertEquals(testF.(val,1,0,0.5), [ -1, -0.9, -0.1, 0, 0.1, 0.9, 1 ], "Test 4: softRound(1, 0, 0.5)");

		// Testing weird edge cases that are probably not typical use cases for backwards compatability purposes.

		// resolution value of 0 leave the original array unchanged.
		val = [-1, -0.5, 0, 1, 0.5, 1];
		this.assertEquals(testF.(val,0,0,1), val, "Test 5 (edge case): softRound(0, 0, 1)");

		// negative resolution values result in very strange behavior...
		this.assertEquals(testF.(val,-1,0,1), [1, -1, 0, 1, 0, 1], "Test 6 (edge case): softRound(-1, 0, 1)");

		// negative margin value also results in very strange behavior.
		val = [-1, -0.8, -0.7, -0.4, -0.1, 0, 0.2, 0.5, 0.9, 1];
		this.assertEquals(testF.(val,1,-0.3,1),  [-1, -1, -1, 0, 0, 0, 0, 1, 1, 1] , "Test 7 (edge case): softRound(1, -0.3, 1)");

		val = [-1, -0.5, 0, 1, 0.5, 1];
		this.assertEquals(testF.(val,1,0,-0.5), [-1, -0.75, 0, 1, 0.25, 1], "Test 8 (edge case): softRound(1, 0, -0.5)");

		this.assertEquals(testF.(val,1,0,0), val, "Test 9 (edge case): softRound(1, 0, 0)");
		this.assertEquals(testF.(val,1,2,1), val, "Test 10 (edge case): softRound(1, 2, 1)");

	}

	test_snap {
		var val;
		var testF = {|vals, g, t, s| vals.collect({|num| num.snap(g, t, s)})};

		val = [-1.3, -1, -0.7, -0.5, -0.3, 0, 0.2, 0.5, 0.7, 1, 1.2, 1.8, 2];
		this.assertEquals(testF.(val, 1,1,1), [-1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2], "Test 1: snap(1, 1, 1)");

		val = [-1, -0.7, -0.5, -0.3, -0.25, -0.2, 0, 0.2, 0.25, 0.3, 0.6, 0.8, 1];
		this.assertEquals(testF.(val,0.5,1,1), [ -1, -0.5, -0.5, -0.5, 0, 0, 0, 0, 0.5, 0.5, 0.5, 1, 1 ], "Test 2: snap(0.5, 1, 1)");

		val = [-0.8, -0.7, -0.6, -0.4, -0.3, -0.2, 0, 0.2, 0.3, 0.4, 0.6, 0.7, 0.8, 1];
		this.assertEquals(testF.(val,1,0.3,1), [-1, -0.7, -0.6, -0.4, -0.3, 0, 0, 0, 0.3, 0.4, 0.6, 0.7, 1, 1], "Test 3: snap(1, 0.3, 1)");

		val = [-1, -0.8, -0.2, 0, 0.2, 0.8, 1];
		this.assertEquals(testF.(val,1,1,0.5), [-1, -0.9, -0.1, 0, 0.1, 0.9, 1], "Test 4: snap(1, 1, 0.5)");

		// Testing edge cases that are strictly speaking not supported by this function.
		val = [-1, -0.5, 0, 1, 0.5, 1];
		// resolution value of 0 leave the original array unchanged.
		this.assertEquals(testF.(val,0,1,1), val, "Test 5 (edge case): snap(0, 1, 1)");

		// negative resolution values result in very strange behavior...
		this.assertEquals(testF.(val,-1,1, 1), [-1, -1, 0, 1, 0, 1], "Test 6 (edge case): snap(-1, 1, 1)");

		val = [-1, -0.8, -0.7, -0.4, -0.1, 0, 0.2, 0.5, 0.9, 1];
		// negative margin value has no effect.
		this.assertEquals(testF.(val,1,-0.3,1),  val  , "Test 7 (edge case): snap(1, -0.3, 1)");

		val = [-1, -0.5, 0, 1, 0.5, 1];
		this.assertEquals(testF.(val,1,1,-0.5), [ -1, -0.75, 0, 1, 0.25, 1 ], "Test 8 (edge case): snap(1, 1, -0.5)");

		this.assertEquals(testF.(val,1,1,0), val, "Test 9 (edge case): snap(1, 1, 0)");
		this.assertEquals(testF.(val,1,0,1), val, "Test 10 (edge case): snap(1, 0, 1)");
		this.assertEquals(testF.(val,1,2,1), [ -1, 0, 0, 1, 1, 1 ] , "Test 11 (edge case): snap(1, 2, 1)");
	}

	test_series {
		var first = 0;
		var step = 2.0001;
		var last = 8;
		var arr = first.series(step, last);
		this.assert(arr.last <= last, "SimpleNumber:series should not produce an array whose last value is greater than the specified 'last' argument.");

		first = 1;
		arr = first.series(first, first);
		this.assert(arr.size == 1, "SimpleNumber:series Int types with first == last and step == 0 should return an array of [ first ]");

		first = 1.1;
		arr = first.series(first, first);
		this.assert(arr.size == 1, "SimpleNumber:series Float types with first == last and step == 0 should return an array of [ first ]");
	}
}
