TestCollection : UnitTest {

	test_removeAllEqual {
		var list1, list2;
		list1 = [(\a -> 1), (\b -> 4), (\b -> 5), (\c -> 5)];
		list2 = [(\b -> 2), (\a -> 10)];
		// note: two associations are euqal if their keys are.
		this.assertEquals(
			list1.removeAllEqual(list2),
			[(\b -> 5), (\c -> 5)],
			"removeAllEqual should remove equal items once."
		)
	}

}
