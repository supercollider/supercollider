
TestFloat : UnitTest {
	
	test_storeOn {
		var stored;
		stored = String.streamContents({ arg stream; stream <<< 1e-05});
		this.assert(stored !=  "1e-05.0", "scientific notation should not have a .0 at the end");
		this.assertEquals(stored ,  "1e-05", "scientific notation");

		stored = String.streamContents({ arg stream; stream <<< 2.0 });
		this.assertEquals(stored ,  "2.0", "2.0 should be stored as 2.0");

		stored = String.streamContents({ arg stream; stream <<< 2 });
		this.assertEquals(stored ,  "2", "2 should be stored as 2");

	}
	
}

