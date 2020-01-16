TestUnitTest : UnitTest {

	var someVar;

	setUp {
		someVar = \setUp;
	}

	test_setUp {
		this.assertEquals(someVar, \setUp, "someVar be set in setUp");
	}

	test_bootServer {
		var server = Server(this.class.name);
		server.bootSync;
		this.assert(server.serverRunning, "The test's Server should be booted while we waited");
		server.quit.remove;
	}

	test_assert {
		this.assert(true, "assert(true) should certainly work");
	}

	test_findTestedClass {
		this.assertEquals(TestMixedBundleTester.findTestedClass, MixedBundleTester)
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
}
