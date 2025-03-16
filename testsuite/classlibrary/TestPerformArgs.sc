TestPerformArgs_O {
	foo { |a, b, c, d, e, f... args, kwargs |
		^(a: a, b: b, c: c, d: d, e: e, f: f, args: args, kwargs: kwargs)
	}
	doesNotUnderstand { |selector... args, kwargs|
		^if (selector == \abc) {
			true
		} {
			this.superPerformArgs(\doesNotUnderstand, [selector] ++ args, kwargs)
		}
	}
}

TestPerformArgs : UnitTest {
	test_correct_behaviour {
		this.assertEquals(
			TestPerformArgs_O().performArgs(\foo, [1, 2], [\e, 10, \f, 11, \bar, 12]),
			(a: 1, b: 2, e: 10, f: 11, args: [], kwargs: [\bar, 12]),
			"performArgs should pass arguments correctly."
		);
		this.assertEquals(
			TestPerformArgs_O().performArgs(\foo, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]),
			(a: 1, b: 2, c: 3, d: 4, e: 5, f: 6, args: [7, 8, 9, 10], kwargs: []),
			"performArgs should pass arguments correctly to variable arguments."
		);
		this.assertEquals(
			TestPerformArgs_O().performArgs(\foo, nil, [\a, 10, \b, 20]),
			(a: 10, b: 20, args: [], kwargs: []),
			"performArgs should work with only keyword arguments."
		);
	}

	test_incorrect_behaviour {
		this.assertException(
			{ TestPerformArgs_O().performArgs(()) },
			PrimitiveFailedError,
			"Passing incorrect selector should throw."
		);
		this.assertException(
			{ TestPerformArgs_O().performArgs(\foo, [1, 2, 3], [4, 5, 6]) },
			PrimitiveFailedError,
			"Passing incorrect kwargs should throw."
		);
		this.assertException(
			{ TestPerformArgs_O().performArgs(\foo, (a: 1, b: 2, c: 3), [4, 5, 6]) },
			PrimitiveFailedError,
			"Passing incorrect args should throw."
		);
	}

	test_duplicates {
		this.assertEquals(
			TestPerformArgs_O().performArgs(\foo, [1, 2], [\a: 10, \b: 20, \a: 30]),
			(a: 30, b: 20, args: [], kwargs: []),
			"performArgs should accept last value in cases of collision."
		);
	}

	test_correct_superPerformArgsImpl {
		this.assert(TestPerformArgs_O().abc, report: false);
		this.assertException(
			{ TestPerformArgs_O().asd  }, // This will infinitly loop if wrong.
			DoesNotUnderstandError,
			"superPerformArgs works as expected"
		);
	}
}
