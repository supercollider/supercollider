
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

	test_assertException_implicitThrow {
		this.assertException({ 1789.monarchy }, DoesNotUnderstandError, "assertException should return true for any error")
	}

	test_assertException_explicitThrow {
		this.assertException({ BinaryOpFailureError("I prefer ternary").throw },
			BinaryOpFailureError,
			"assertException should return true for specific error",
		)
	}

	test_assertNoException_nonExceptionThrow {
		this.assertNoException({ \stone.throw }, "assertException should return false for thrown object")
	}

	test_assertNoException_nonThrowingFunction {
		this.assertNoException({ try { 1789.monarchy } }, "assertNoThrow should return true for not an error")
	}

	/*** IF YOU ADD MORE TESTS, UPDATE THE numTestMethods var ***/
	// test_findTestMethods {
	// 	var numTestMethods = 7;
	// 	this.assert( this.findTestMethods.size == numTestMethods, "should be " + numTestMethods + " test methods");
	// }

}

