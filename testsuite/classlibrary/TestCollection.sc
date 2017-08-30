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

test_asPairs {
		this.assert(
    	[2,10,100,1000].collect{ |size| size.collect{ |i| i -> i }.asPairs.size >= (size * 2) }.reduce('&&'), 
    	"asPairs on an array of associations should create an array with size higher or equal to twice the number of elements in the original array"
    )
}

}
