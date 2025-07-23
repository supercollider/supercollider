TestBag : UnitTest {
	test_Bag_avoidsInPlaceOperations {
		var a, b, saveA;
		['difference', 'union'].do { |op|
			a = Bag[1, 1, 2, 2, 3, 3, 4, 5];
			b = Bag[3, 4, 8];
			saveA = a.deepCopy;
			a.perform(op, b);
			this.assertEquals(a, saveA,
				"Set operation '%' on a Bag should not mutate the original receiver"
				.format(op)
			);
		}
	}
}
