TestDoesNotUnderstandWithKeysWorking {
	doesNotUnderstandWithKeys{|selector, argsArray, keywordArrayPair|
		^[selector, argsArray, keywordArrayPair]
	}
}

TestDoesNotUnderstandWithKeys : UnitTest {
	test_object_throws {
		this.assertException(
			{Object().forSomeMethodThatObjectDoesNotUnderstand(1, 2, argA: 3)},
			DoesNotUnderstandWithKeysError
		)
	}

	test_can_be_caught {
		var ret = TestDoesNotUnderstandWithKeysWorking().
		forSomeMethodThatObjectDoesNotUnderstand(1, 2, 3, d: 4, e: 5);

		this.assertEquals(ret[0], \forSomeMethodThatObjectDoesNotUnderstand,
			"Selector should be passed to doesNotUnderstandWithKeys.");
		this.assertEquals(ret[1], [1, 2, 3],
			"Args without keywords should be passed to doesNotUnderstandWithKeys."
		);
		this.assertEquals(ret[2], [d: 4, e: 5],
			"Keyword args should be passsed to doesNotUnderstandWithKeys as an Array of keypairs."
		);
	}
}