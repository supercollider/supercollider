// Testing Condition with Condition, but that should still catch
// most bugs.

TestSync : UnitTest {
	const waitTime = 0.05;

	test_basic {
		var sync = Sync();
		var done = false;

		fork {
			sync.wait;
			done = true;
		};

		{ sync.unhang }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Sync was released");
		});
	}
}

TestCondition : UnitTest {
	var waitTime = 0.05;

	test_basic_unhang {
		var cond = Condition();
		var done = false;

		fork {
			cond.wait;
			done = true;
			"done".postln;
		};

		{ "unhanging".postln; cond.unhang }.defer(waitTime);

		this.asynchAssert({ done.postln }, {
			this.assertEquals(done, true, "Condition was released");
		});
	}

	test_basic_signal {
		var cond = Condition(false);
		var done = false;

		fork {
			cond.wait;
			done = true;
		};

		{ cond.test_(true).signal }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Condition was released");
			this.assertEquals(cond.test, true, "Condition:test was set");
		});
	}

	test_timeout {
		var cond;
		var done = false;

		fork {
			Condition.timeout(waitTime).wait;
			done = true;
		};

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Condition was released");
		});
	}

	test_wait_with_timeout {
		var cond;
		var done = false;

		fork {
			Condition(false).wait(waitTime);
			done = true;
		};

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Condition was released after timeout");
		});
	}

	test_operators_and {
		var a = Condition();
		var b = Condition();
		var done = false;

		fork {
			(a && b).wait.
			done = true;
		};

		{ b.unhang; a.unhang }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Both conditions were released");
		});
	}

	test_operators_or {
		var a = Condition();
		var b = Condition();
		var done = false;

		fork {
			(a || b).wait.
			done = true;
		};

		{ b.unhang; }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "One condition was released");
		});
	}

	test_operators_then {
		var cond = Condition();
		var done = false;

		cond.then({
			done = true;
		});

		{ cond.unhang; }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Condition then clause was run");
		});
	}

	test_operators_poll {
		var cond = Condition().poll(waitTime);
		var done = false;

		fork {
			cond.wait();
			done = true;
		};

		{ cond.test = true }.defer(waitTime);

		this.asynchAssert({ done }, {
			this.assertEquals(done, true, "Condition was signaled via polling");
		});
	}
}

