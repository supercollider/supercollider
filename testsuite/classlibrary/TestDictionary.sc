TestDictionary : UnitTest {

	test_keysValuesArrayDo {
		var result;

		this.assert(Dictionary[].keysValuesArrayDo([]) == Dictionary[], "keysValuesArrayDo on an empty dictionary should do nothing");

		result = try { Dictionary[].keysValuesArrayDo(nil) } { |err|
				err.isKindOf(PrimitiveFailedError)
			};
		this.assert(
			result,
			"calling keysValuesArrayDo on nil should throw a PrimitiveFailedError"
		)

	}

}
