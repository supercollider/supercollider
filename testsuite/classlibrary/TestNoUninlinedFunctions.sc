TestNoUninlinedFunctions : UnitTest {
	prGet { _NumUninlinedFunctionInClassLib }

	test_valid {
		this.assertEquals(this.prGet, 0, "There should not be uninlined functions in the class library.");
	}
}
