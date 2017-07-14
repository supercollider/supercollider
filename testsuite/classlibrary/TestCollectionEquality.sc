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

	test_unequal_event_parent {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		b.parent = (x: 4);
		this.assert(a != b, "different events should be unequal");
	}

	test_unequal_event_proto {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		b.parent = (x: 4);
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

	test_unequal_dictionary_different_size {
		var a, b;
		a = Dictionary[\x -> 10, \y -> 9, \z -> 8];
		b = Dictionary[\x -> 10, \y -> 9];
		this.assert(a != b, "dictionaries of different size should be unequal");
	}


	// hash

	test_hash_event {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		this.assert(a.hash == b.hash, "same events should have the same hash value");
	}

	test_unhash_event {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 10, y: 9);
		this.assert(a.hash != b.hash, "similar but unequal events should have unequal hash values");
	}

	test_unhash_event_size {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 10, y: 10, z: 8);
		this.assert(a.hash != b.hash, "similar but differently sized events should have unequal hash values");
	}

	test_unhash_event_parent {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		b.parent = (x: 4);
		this.assert(a.hash != b.hash, "similar but unequal events should have unequal hash values");
	}

	test_unhash_event_proto {
		var a, b;
		a = (x: 9, y: 10);
		b = (x: 9, y: 10);
		b.parent = (x: 4);
		this.assert(a.hash != b.hash, "similar but unequal events should have unequal hash values");
	}

	test_hash_dictionary {
		var a, b;
		a = Dictionary[\x -> 9, \y -> 10];
		b = Dictionary[\x -> 9, \y -> 10];
		this.assert(a.hash == b.hash, "same dictionaries should have the same hash value");
	}

	test_unhash_dictionary {
		var a, b;
		a = Dictionary[\x -> 10, \y -> 9];
		b = Dictionary[\x -> 9, \y -> 10];
		this.assert(a.hash != b.hash, "different dictionaries should have a different hash value");
	}


}

