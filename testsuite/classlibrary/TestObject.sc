TestObject : UnitTest {

	// not testing Object().writeDefFile("") because it behaves differently
	test_writeDefFile_emptyName {
		this.assertException({ nil.writeDefFile("") }, Error, "nil.writeDefFile(\"\") should fail");
	}

}
