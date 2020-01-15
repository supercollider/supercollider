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

	test_embedInStream_withNonEventArgument {
		var dict = (a:9);
		var argument = 0; // not an event
		var res = dict.embedInStream(argument);
		this.assertEquals(res, dict, "embedInStream should return the receiver if the argument is a number")
	}

	test_embedInStream_withEventArgument {
		var dict = (a:9);
		var argument = (b:10, a:11);
		var res = dict.embedInStream(argument);
		this.assertEquals(res, (b:10, a:9), "embedInStream should overwrite the keys in the argument")
	}

	test_overwritePairs {
		var a = (x: 1871, y: 1920);
		var b = (y: 1968, z: 2020);
		var c = a.overwritePairs(b);

		this.assertEquals(c, ( x: 1871, y: 1969, z: 2020 ), "in a.overwritePairs(b), b should overwrite keys of a");
	}


}
