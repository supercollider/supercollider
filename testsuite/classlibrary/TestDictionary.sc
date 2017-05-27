TestDictionary : UnitTest {

	test_keysValuesArrayDo {

		this.assert(Dictionary[].keysValuesArrayDo([]) == (), "keysValuesArrayDo on an empty dictionary should do nothing");

		this.assert(
			try { Dictionary[].keysValuesArrayDo(nil) } { |err|
				err.isKindOf(PrimitiveFailedError)
			},
			"calling keysValuesArrayDo on nil should throw a PrimitiveFailedError"
		)

	}

}
