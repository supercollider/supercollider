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

}
