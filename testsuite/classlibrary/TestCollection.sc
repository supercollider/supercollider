TestCollection : UnitTest {

	test_asPairs {

		this.assert([2,10,100,1000].collect{ |size| size.collect{ |i| i -> i }.asPairs.size >= (size * 2) }.reduce('&&')
, "asPairs on an array of associations should create an array with size higher or equal to twice the number of elements in the original array");

	}

}
