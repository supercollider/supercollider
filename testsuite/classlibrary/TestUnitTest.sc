
TestUnitTest : UnitTest {

	var someVar,toreDown,count = 0;

	setUp {
		someVar = "setUp";
		count = count + 1;
	}
	tearDown {
		someVar = "tearDown";
		toreDown = true;
	}

	test_setUp {
		this.assert( count == 1, "count should be on 1");
		this.assert( someVar == "setUp", "someVar be set in setUp" );
	}
	test_toreDown{
		this.assert( toreDown, "toreDown should be set at the end of the last test" );
		this.assert( count == 2, "count should be on 2");
	}
	test_setUp2 {
		this.assert( count == 3, "count should be on 3");
	}
	test_assert {
		this.assert(true, "assert(true) should certainly work");
	}
/*
	test_failure {
		this.assert( false, "should fail")
	}
*/

/*	test_assertAsynch {
		Server.default.boot;
		this.assertAsynch( Server.default.serverRunning, {
			this.assert( Server.default.serverRunning,"server is indeed running");
			}, "assert asynch should have triggered the server to boot and then run the test block");
	}
*/

	test_findTestedClass {
		this.assertEquals( TestMixedBundleTester.findTestedClass, MixedBundleTester)
	}




	/*** IF YOU ADD MORE TESTS, UPDATE THE numTestMethods var ***/
	// test_findTestMethods {
	// 	var numTestMethods = 7;
	// 	this.assert( this.findTestMethods.size == numTestMethods, "should be " + numTestMethods + " test methods");
	// }
}

