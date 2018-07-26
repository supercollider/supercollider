
TestUnitTest : UnitTest {

	var someVar, setUp = false;

	setUp {
		setUp = true;
	}

	test_setUp {
		this.assert( setUp, "setUp should have happened" )
	}

	test_assert {
		this.assert(true, "assert(true) should certainly work");
	}

	test_isolation_first {
		this.assertEquals(someVar, nil, "methods in UnitTests should be isolated");
		someVar = 2;
	}

	test_isolation_second {
		this.assertEquals(someVar, nil, "methods in UnitTests should be isolated");
		someVar = 2;
	}

	test_findTestedClass {
		this.assertEquals( TestMixedBundleTester.findTestedClass, MixedBundleTester)
	}

}

