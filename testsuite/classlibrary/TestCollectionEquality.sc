TestCollectionEquality : UnitTest {

	test_equal_event {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		this.assert(a == b, "same events should be equal");
	}

	test_unequal_event {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 10, y: 9);
		this.assert(a != b, "different events should be unequal");
	}

	test_equal_dictionary {
		var a, b;
		a = Dictionary[\x -> 9, \y -> 10];
		b = Dictionary[\x -> 9, \y -> 10];
		this.assert(a == b, "same dictionaries should be equal");
	}

	test_unequal_dictionary {
		var a, b;
		a = Dictionary[\x -> 10, \y -> 9];
		b = Dictionary[\x -> 9, \y -> 10];
		this.assert(a != b, "different dictionaries should be unequal");
	}

}

