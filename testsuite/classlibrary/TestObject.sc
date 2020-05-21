TestObject : UnitTest {

	test_writeDefFile_emptyName {
		this.assertException({ nil.writeDefFile("") }, Error, "nil.writeDefFile(\"\") should fail");
	}

}
