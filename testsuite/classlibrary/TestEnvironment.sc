TestEnvironment : UnitTest {

	test_envirPutNilRemovesEntry {
		var envir = Environment.new(parent: (test: \defaultValue));
		envir.use {

			this.assertEquals(~test, \defaultValue, "parent environment should be in scope if not shadowed");
			~test = \overriddenEntry;
			this.assertEquals(~test, \overriddenEntry, "parent environment should be shadowed");
			~test = nil;
			this.assertEquals(~test, \defaultValue, "parent environment should be in scope again if not shadowed any more");
		};
	}

}
