

TestSequenceableCollection : UnitTest {

	test_insertAll {
		var list1, list2;
		list1 = (0..5);
		list2 = [\x, \y, \z];
		list1.size.do { |i|
			this.assertEquals(
				list1.copy.insert(i, list2).flat,
				list1.copy.insertAll(i, list2),
				"insert all items of a sublist should correspond to insert a single item"
			)
		};

	}
}
