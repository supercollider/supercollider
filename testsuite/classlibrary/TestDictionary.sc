TestDictionary : UnitTest {

	test_keysValuesArrayDo {
		var failedAsIntended = false;

		this.assert(Dictionary[].keysValuesArrayDo([]) == Dictionary[], "keysValuesArrayDo on an empty dictionary should do nothing");

		try { Dictionary[].keysValuesArrayDo(nil) } { |err|
			failedAsIntended = err.isKindOf(PrimitiveFailedError)
		};
		this.assert(
			failedAsIntended,
			"calling keysValuesArrayDo on nil should throw a PrimitiveFailedError"
		)

	}

	test_asAssociations {

		this.assert(
			[2,10,100,1000].collect{ |size|
				var dict = Dictionary.new(size);
				size.do{ |i| dict.put(i,i) };
				dict.asAssociations.size >= size
			}.reduce('&&')
			, "asAssociations on a dictionary should create array of associations with size higher or equal to the number of elements in the original dictionary");

	}

	test_trueAt {
		var dict = (k: 1, nothing:nil, no:false, yes:true);
		this.assertEquals(1.falseAt(\no), true, "falseAt should respond with true for any object by default.");
		this.assertEquals(1.trueAt(\no), false, "trueAt should respond with false for any object by default.");
		this.assertEquals(dict.falseAt(\no), true, "dictionary falseAt should respond with true if its booleanValue is false.");
		this.assertEquals(dict.falseAt(\nothing), true, "dictionary falseAt should respond with true if its value is unspecified.");
		this.assertEquals(dict.trueAt(\nothing), false, "dictionary trueAt should respond with false if its value is unspecified.");
		this.assertEquals(dict.trueAt(\yes), true, "dictionary trueAt should respond true if its booleanValue is true.");
		this.assertEquals(dict.trueAt(\k), true, "dictionary trueAt should respond true if its booleanValue is true.");
	}

}
