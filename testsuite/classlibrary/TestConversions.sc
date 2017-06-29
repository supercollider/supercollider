TestCollectionConversions : UnitTest {


	test_newFrom_identity {
		var from, classes;
		from = [1, 2, 3, 4]; // sorted.
		classes = [Array, Int8Array, List, Order, Set, IdentitySet, Bag, IdentityBag];
		classes.do { |class|
			this.assertEquals(class, from.as(class).class, "as(%) should return an object of that class".format(class));
		};
	}

	test_newFrom_collection_reversible {
		var from, classes;
		from = [1, 2, 3, 4]; // sorted.
		classes = [Array, Int8Array, List, Order, Set, IdentitySet, Bag, IdentityBag];
		classes.do { |class|
			this.assertEquals(from, from.as(class).as(Array).sort,
				"as(%) should be reversible for an array like %".format(class, from));
		};
	}

	test_newFrom_dictionary_reversible {
		var from, classes;

		from = Dictionary[1 -> 2, 3 -> 5];
		classes = [Dictionary, IdentityDictionary]; // TwoWayIdentityDictionary fails.
		classes.do { |class|
			this.assertEquals(from, from.as(class).as(Dictionary),
				"as(%) should be reversible for a dictionary like %".format(class, from));
		};
	}

	test_asDict_reversible {
		var functions, object;

		functions = [
			{ |x| var class = x.class; x.asPairs.asDict(class: class) },
			{ |x| var class = x.class; x.asAssociations.asDict(class: class) },
			{ |x| var class = x.class; x.asDict(class: class) },
		];

		object = (a: 3, b: 4);
		functions.do { |test|
			this.assertEquals(object, test.value(object),
				"asDict should be compatible with key value pairs interface: " ++ test.asCompileString)
		};
	}

	test_asPairs_reversible {
		var functions, object;

		functions = [
			{ |x| var class = x.class; x.asDict.asPairs(class: class) },
			{ |x| var class = x.class; x.asAssociations.asPairs(class: class) },
			{ |x| var class = x.class; x.asPairs(class: class) },
		];

		object = [\a, 3, \b, 4];
		functions.do { |test|
			this.assertEquals(object, test.value(object),
				"asPairs should be compatible with key value pairs interface: " ++ test.asCompileString)
		};
	}

	test_asAssociations_reversible {
		var functions, object;

		functions = [
			{ |x| var class = x.class; x.asAssociations.asDict.asAssociations(class: class) },
			{ |x| var class = x.class; x.asAssociations.asPairs.asAssociations(class: class) },
			{ |x| var class = x.class; x.asAssociations(class: class) },
		];

		object = [\a -> 3, \b -> 4];
		functions.do { |test|
			this.assertEquals(object, test.value(object),
				"asAssociations should be compatible with key value pairs interface: " ++ test.asCompileString)
		};
	}


}

