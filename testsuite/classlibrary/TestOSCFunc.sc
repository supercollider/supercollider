TestOSCFunc : UnitTest {

	test_argSizeMatching_Succeeds {
		var cond = Condition();
		var oscfunc;
		var result = 0;

		oscfunc = OSCFunc({ result = 1; cond.unhang; }, "/test_argSizeMatchingSucceeds", argTemplate: [2, 3]);

		NetAddr.localAddr.sendMsg( "/test_argSizeMatchingSucceeds", 2, 3); // this should match
		fork { 0.1.wait; cond.unhang };

		cond.hang;

		this.assertEquals(result, 1);
	}

	test_argSizeMatching_Fails {
		var cond = Condition();
		var oscfunc;
		var result = 0;

		oscfunc = OSCFunc({ result = 1; cond.unhang; }, "/test_argSizeMatchingFails", argTemplate: [2, 3]);

		NetAddr.localAddr.sendMsg( "/test_argSizeMatchingFails", 2); // we need at least the same numArgs in the template
		fork { 0.1.wait; cond.unhang };

		cond.hang;

		this.assertEquals(result, 0);
	}
}
