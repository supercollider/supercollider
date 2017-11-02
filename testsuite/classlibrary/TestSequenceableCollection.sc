

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

	test_mergeItems_reverse {
		var list1, list2;
		list1 = (0..5);
		list2 = list1.reverse;
		this.assertEquals(
			list1.mergeItems(list2),
			list1,
			"merging a list with its reversed counterpart should not change the order"
		)
	}

	test_mergeItems_keep_equals {
		var list1, list2;
		list1 = [(\a -> 1), (\b -> 2)];
		list2 = [(\b -> 2), (\a -> 10)];
		// note: two associations are euqal if their keys are.
		this.assertEquals(
			list1.mergeItems(list2),
			list1,
			"merging a list with equal items should keep the receiver's items"
		)
	}

	test_mergeItems_insert {
		var list1, list2;
		list1 = [1, 5, 10];
		list2 = [0, 1, 3, 5, 6, 9, 11];
		this.assertEquals(
			list1.mergeItems(list2),
			[0, 1, 3, 5, 6, 9, 11, 10],
			"merging should keep the order of the receiver, and insert the merged list wherever possible"
		)
	}


	test_mergeItems_keep_duplicates {
		var list1, list2;
		list1 = [1, 1, 5, 10, 1];
		list2 = [0, 1, 3, 5, 6, 9, 11];
		this.assertEquals(
			list1.mergeItems(list2),
			[0, 1, 1, 3, 5, 6, 9, 11, 10, 1],
			"merging should keep duplicate items in receiver"
		)
	}



}
