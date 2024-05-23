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
		var dict = (k: 1, x:nil, y:false, z:true);
		this.assert(1.falseAt(\y), "falseAt should respond with true for any object by default.");
		this.assert(1.trueAt(\y).not, "trueAt should respond with false for any object by default.");
		this.assert(dict.falseAt(\y), "dictionary falseAt should respond with false if its booleanValue is equivalent to false.");
		this.assert(dict.falseAt(\x), "dictionary falseAt should respond with false if its value is unspecified.");
		this.assert(dict.trueAt(\z), "dictionary trueAt should respond true if its booleanValue is equivalent to true.");
		this.assert(dict.trueAt(\k), "dictionary trueAt should respond true if its booleanValue is equivalent to true.");

	}

}
