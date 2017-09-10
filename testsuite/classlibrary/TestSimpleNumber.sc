// TestSimpleNumber.sc
// Tests for the SimpleNumber class
TestSimpleNumber : UnitTest {

	//////////////////////
	// asTimeString tests
	//////////////////////

	test_asTimeString_zero {
		var expected = "00:00:00.000";
		var actual = 0.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_zeroNotDroppingDays {
		var expected = "000:00:00:00.000";
		var actual = 0.asTimeString(dropDaysIfPossible: false);
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_nearZeroRoundDown {
		var expected = "00:00:00.000";
		var actual = 0.0001.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_nearZeroRoundUp {
		var expected = "00:00:00.001";
		var actual = 0.0009.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_milliseconds {
		var expected = "00:00:00.015";
		var actual = 0.015.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_millisecondPrecisionTenths {
		var expected = "00:00:05.500";
		var actual = 5.478.asTimeString(precision: 0.1);
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_millisecondPrecisionHundredths {
		var expected = "00:00:05.480";
		var actual = 5.478.asTimeString(precision: 0.01);
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_millisecondPrecisionFiftieths {
		var expected = "00:00:05.480";
		var actual = 5.471.asTimeString(precision: 0.02);
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_seconds {
		var expected = "00:00:07.015";
		var actual = 7.015.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_minutes {
		var expected = "00:02:07.015";
		var actual = 127.015.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_hours {
		var expected = "03:02:07.015";

		var threeHoursInSeconds = 3600 * 3;
		var totalTime = threeHoursInSeconds + 127.015;
		var actual = totalTime.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_days {
		var expected = "002:03:02:07.015";

		var twoDaysInSeconds = 24 * 3600 * 2;
		var threeHoursInSeconds = 3600 * 3;
		var totalTime = twoDaysInSeconds + threeHoursInSeconds + 127.015;
		var actual = totalTime.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_maxDays {
		var expected = "365:00:00:00.000";

		var numSecondsIn400Days = 400 * 24 * 3600;
		var actual = numSecondsIn400Days.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

	test_asTimeString_maxDaysWithExtraSeconds {
		var expected = "365:00:00:10.000";

		var numSecondsIn400Days = 400 * 24 * 3600;
		var totalTime = numSecondsIn400Days + 10;
		var actual = totalTime.asTimeString;
		this.assert(actual == expected, "expected %, got %".format(expected, actual));
	}

}
