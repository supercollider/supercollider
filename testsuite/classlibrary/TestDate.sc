/*
Date.test
UnitTest.gui
*/
TestDate : UnitTest {

	test_creation {
		var d = Date(2017, 10, 25);
		this.assertEquals(d.rawSeconds, 1508882400, "Computed rawSeconds");
		this.assertEquals(d.dayOfWeek, 3, "Computed dayOfWeek");

		try {
			// This should fail
			d = Date(1977, 5, 25, 13, 30, 22, 2);
		} { |error|
			this.assertEquals(error.class, PrimitiveFailedError,
				"Date with dayOfWeek but no rawSeconds");
		};

		try {
			// This should fail
			d = Date(1977, 5, 25, 13, 30, 22, rawSeconds: 1234567.8);
		} { |error|
			this.assertEquals(error.class, PrimitiveFailedError,
				"Date with rawSeconds but no dayOfWeek");
		};

		d = Date.getDate;
		this.assertEquals(Date(d.year, d.month, d.day, d.hour, d.minute, d.second, d.dayOfWeek, d.rawSeconds), d, "Creation from all properties");
		this.assertEquals(Date(d.year, d.month, d.day, d.hour, d.minute, d.second).rawSeconds, d.rawSeconds.floor, "Creation from all properties except dayOfWeek and rawSeconds");
	}

	test_comparison {
		this.assert(Date(1999, 12, 31, 23, 59, 59) < Date(2000, 1, 1, 0, 0, 0),
			"Comparison");
		this.assert(Date(2016, 11, 4) == Date(2016, 10, 4 + 31),
			"Equality");
		this.assertEquals(Date(2010, 12, 25 - 100), Date(2010, 9, 16), "Day subtraction");
	}

	test_strings {
		var d = Date(2017, 6, 5, 4, 3, 2);
		this.assertEquals(d.stamp, "170605_040302", "stamp");
		this.assertEquals(d.asSortableString, "20170605040302", "sortableString");
		this.assertEquals(d.format("%F %T"), "2017-06-05 04:03:02", "format");
	}
}
