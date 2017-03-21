TestDictionary : UnitTest {

	test_keysValuesArrayDo {

		this.assert(().keysValuesArrayDo([]) == (), "keysValuesArrayDo on an empty dictionary should do nothing");

		this.assert(
			try { ().keysValuesArrayDo(nil) } { |err|
				err.isKindOf(PrimitiveFailedError)
			},
			"calling keysValuesArrayDo on nil should throw a PrimitiveFailedError"
		)

	}

}
