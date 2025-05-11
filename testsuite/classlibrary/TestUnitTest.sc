TestUnitTest : UnitTest {

	var setUpHappened = false;
	classvar <>classSetUpHappened = false;
	classvar <>classTearDownHappened = false;

	*setUpClass {
		this.classSetUpHappened = true;
	}

	*tearDownClass {
		this.classTearDownHappened = true;
	}

	setUp {
		setUpHappened = true;
	}

	test_setUpClass_already_happened {
		this.assert(this.class.classSetUpHappened, "setUpClass should have happened");
	}

	test_setUpClass {
		this.class.classSetUpHappened = false;
		this.class.prRunWithinSetUpClass {
			this.assert(this.class.classSetUpHappened,
				"setUpClass should have happened before prRunWithinSetUpClass function is called"
			);
		}
	}

	test_tearDownClass {
		this.class.classTearDownHappened = false;
		this.class.prRunWithinSetUpClass;
		this.assert(this.class.classTearDownHappened,
			"tearDownClass should have happened after prRunWithinSetUpClass function is called"
		);
	}

	test_setUp {
		this.assert(setUpHappened, "setUp should have happened")
	}

	test_bootServer {
		var server = Server(this.class.name);
		server.bootSync;
		this.assert(server.serverRunning, "The test's Server should be booted while we waited");
		server.quitSync.remove;
	}

	test_assert {
		this.assert(true, "assert(true) should certainly work")
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

	test_wait {
		var unblock = false;
		var r = Routine {
			0.01.yield;
			unblock = true;
		};
		r.play;
		this.wait({ unblock }, maxTime: 0.02);
		this.assertEquals(unblock, true, "UnitTest.wait should continue when test is true");
	}



}
