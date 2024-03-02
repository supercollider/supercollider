TestDoesNotUnderstandAboutWorking {
	doesNotUnderstandAbout{|selector, argsArray, keywordArrayPair|
	    "bug protection".postln;
		^[selector, argsArray, keywordArrayPair]
	}
}

Test_DoesNotUnderstandAbout_OnlyNonKW {
	doesNotUnderstand { |selector ...args|
	    "bug protection".postln;
         ^args
	  }
}

Test_DoesNotUnderstandAbout_OnlyKW {
	doesNotUnderstandAbout { |selector, argumentsArray, keywordArgumentPairs|
	    "bug protection".postln;
		^argumentsArray
	}
}

TestDoesNotUnderstandAbout : UnitTest {
	test_object_throws {
		this.assertException(
			{Object().forSomeMethodThatObjectDoesNotUnderstand(1, 2, argA: 3)},
			DoesNotUnderstandError
		)
	}

	test_can_be_caught {
		var ret;
		ret = TestDoesNotUnderstandAboutWorking().forSomeMethodThatObjectDoesNotUnderstand(1, 2, 3, d: 4, e: 5);

		this.assertEquals(ret[0], \forSomeMethodThatObjectDoesNotUnderstand,
			"Selector should be passed to doesNotUnderstandAbout.");
		this.assertEquals(ret[1], [1, 2, 3],
			"Args without keywords should be passed to doesNotUnderstandAbout."
		);
		this.assertEquals(ret[2], [d: 4, e: 5],
			"Keyword args should be passed to doesNotUnderstandAbout as an Array of keypairs."
		);
	}

	test_respects_existing_doesNotUnderstand {
		this.assertEquals(Test_DoesNotUnderstandAbout_OnlyNonKW().asdf('hurray!', someArg: 1), ['hurray!']);
	}
	test_respects_only_doesNotUnderstandAbout {
		this.assertEquals(Test_DoesNotUnderstandAbout_OnlyKW().asdf('hurray!'), ['hurray!']);
	}
}



