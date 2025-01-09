TestThunk : UnitTest {

	test_callOnce_kwargs {
		var a = CallOnce { |x, y| x + y };
		var val = a.value(3, y:4);
		var val2 = a.value(3, y: 1000);
		this.assertEquals(val, 3 + 4, "CallOnce should receive keyword arguments.");
		this.assertEquals(val2, 3 + 4, "CallOnce should keep value from first call also when called with different arguments later.");
	}


}
