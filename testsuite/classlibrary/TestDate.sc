/*
Date.test
UnitTest.gui
*/
TestDate : UnitTest {

	test_creation {
		var d = Date.new;
		var e = Date.localtime;
		this.assertFloatEquals(d.rawSeconds, e.rawSeconds,
			"new with no args == localtime", 0.5);

		e = Date(rawSeconds: d.rawSeconds); // create a new Date with the same time
		this.assertEquals(d, e,
			"rawSeconds is used if specified");

		d = Date(2017, 10, 25);
		this.assertEquals(d.dayOfWeek, 3,
			"Computed dayOfWeek");

		this.assertEquals(Date(1977, 5, 25, 13, 30, 22, dayOfWeek: 2).dayOfWeek, 3,
			"corrected incorrect dayOfWeek");

		d = Date(1977, 5, 25, 13, 30, 22, rawSeconds: 1234567.8);
		this.assertEquals(d.rawSeconds, 1234567.8,
			"rawSeconds is used if specified");
		this.assertEquals(Date(2000, 1, 1, rawSeconds: 1234567890),
			Date(rawSeconds: 1234567890),
			"rawSeconds is used if specified");
		this.assertEquals(d.year, 1970,
			"rawSeconds overrides other properties");
		this.assert(Date(2000, 1, 1, rawSeconds: 1234567890) != Date(2000, 1, 1),
			"rawSeconds overrides other properties");

		d = Date.getDate;
		this.assertEquals(
			Date(d.year, d.month, d.day, d.hour, d.minute, d.second, d.dayOfWeek, d.rawSeconds),
			d,
			"Creation from all properties");
		this.assertEquals(
			Date(d.year, d.month, d.day, d.hour, d.minute, d.second).rawSeconds,
			d.rawSeconds.floor,
			"Creation from all properties except dayOfWeek and rawSeconds");

		this.assertEquals(Date(1970, 7, 20, minute: 18).stamp, "700720_001800",
			"new with defaults");
		this.assertEquals(Date(2016).stamp, "160101_000000",
			"new with defaults");
		this.assertEquals(Date(2010, day: 15, minute: 30).asSortableString,
			"20100115003000",
			"new with defaults");

		e = nil;
		try { Date(month: 7) } { |err| e = err }; // ERROR: year must be a valid Integer
		this.assertEquals(e.class, PrimitiveFailedError,
			"missing year");

		d = Date(2017, 10, 25);
		this.assertEquals(
			[d.asSortableString, d.dayOfWeek],
			[ "20171025000000", 3],
			"dayOfWeek is computed");

		this.assertEquals(Date(2017, 10, 31, dayOfWeek: 0).dayOfWeek, 2,
			"dayOfWeek always computed");

		this.assertEquals(Date(2010, 12, 25 - 100).format("%m/%d"), "09/16",
			"One hundred days before Christmas");
		this.assertEquals(Date(2017, 2, 29), Date(2017, 3, 1),
			"invalid dates are corrected (not leap year)");

		this.assertEquals(
			Date.fromString("2017-10-25 13:25:55", "%Y-%m-%d %H:%M:%S"),
			Date(2017, 10, 25, 13, 25, 55),
			"fromString");
		this.assertEquals(
			Date.fromString("2017-10-25 [13:25:55]", "%Y-%m-%d [%H:%M:%S]"),
			Date(2017, 10, 25, 13, 25, 55),
			"fromString");
		this.assertEquals(
			Date.fromString("1981.9.30 @ 17:33", "%Y.%m.%d @ %H:%M"),
			Date(1981, 9, 30, 17, 33),
			"fromString");

		// Round-trip via string, using "stamp"
		{
			var orig = Date(2013, 5, 25, 10, 9, 8);
			var stampString = orig.stamp;
			var d = Date.fromString(stampString, "%y%m%d_%H%M%S");
			this.assertEquals(d, orig,
				"round trip via string using 'stamp'");
		}.value;
	}

	test_comparison {
		var d1 = Date(1999, 12, 31, 23, 59, 59);
		var d2 = Date(2000, 1, 1, 0, 0, 0);
		var d3 = Date(2000, 1, 1, 0, 0, 0 - 1);
		this.assert((d1 == d2).not, "not ==");
		this.assert(d1 != d2, "!=");
		this.assert(d1 == d3, "==");

		this.assert(d1 < d2, "<");
		this.assert((d2 < d1).not, "not <");
		this.assert(d1 <= d2, "<=");
		this.assert((d2 <= d1).not, "<=");

		this.assert(d2 > d1, ">");
		this.assert((d1 > d2).not, "not >");
		this.assert(d2 >= d1, ">=");
		this.assert((d1 >= d2).not, "not >=");

		this.assert(Date(2016, 11, 4) == Date(2016, 10, 4 + 31),
			"Equality");
		this.assertEquals(Date(2010, 12, 25 - 100), Date(2010, 9, 16),
			"Day subtraction");
	}

	test_strings {
		var d = Date(2017, 6, 5, 4, 3, 2);
		this.assertEquals(d.stamp, "170605_040302",
			"stamp");
		this.assertEquals(d.asSortableString, "20170605040302",
			"sortableString");
		this.assertEquals(d.format("%F %T"), "2017-06-05 04:03:02",
			"format");

		// Try a roundtrip to/from string, using "stamp"
		{
			var d = Date(2013, 12, 11, 10, 9, 8);
			var d2 = Date.fromString(d.stamp, "%y%m%d_%H%M%S").postln;
			this.assertEquals(d, d2,
				"roundtrip to/from string using stamp");
		}.value;
	}

	test_resolve {
		var d = Date(2017, 9, 30, 10, 30);
		var e;
		var origVal = [d.dayOfWeek, d.rawSeconds];
		d.hour = d.hour - 13; // -> Sat Sep 30 -03:30:00 2017
		d.month = d.month + 4; // -> Sat ??? 30 -03:30:00 2017
		d.minute = d.minute + 200; // -> Sat ??? 30 -03:230:00 2017
		this.assertEquals(origVal, [d.dayOfWeek, d.rawSeconds],
			"dayOfWeek+rawSeconds haven't updated with property modifications");
		d.resolve;
		e = Date(2018, 1, 30, 0, 50);
		this.assertEquals(d, e,
			"use resolve to fix inconsistencies");
		this.assertEquals(d.stamp, e.stamp,
			"use resolve to fix inconsistencies");
		this.assertEquals(d.dayOfWeek, 2,
			"resolve corrected dayOfWeek");

		d = Date(1999, 12, 31); // -> Fri Dec 31 00:00:00 1999
		origVal = d.asString;
		d.rawSeconds = d.rawSeconds + 1e6;
		this.assertEquals(d.asString, origVal,
			"rawSeconds changes don't update other properties");
		d = Date(rawSeconds: d.rawSeconds);
		this.assertEquals(d, Date(2000, 1, 11, 13, 46, 40),
			"resolved rawSeconds using Date constructor");
	}
}
