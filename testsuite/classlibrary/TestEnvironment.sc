TestEnvironment : UnitTest {

	test_envirPutNilRemovesEntry {

		var parentEnvir =  (test: 42);
		var envir = Environment.new(parent: parentEnvir);

		envir.use {
			this.assertEquals(~test, 42, "parent environment should be in scope");
			~test = 48;
			this.assertEquals(~test, 48, "environment should shadow value in parent");
			~test = nil;
			this.assertEquals(~test, 42, "parent environment should be in scope again if not shadowed any more");
		};
	}

}
