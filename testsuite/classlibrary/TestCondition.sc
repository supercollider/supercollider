

TestCondition : UnitTest {


	test_hangWithTimeout {

		var hangTime = 0.1;
		var hasContinued = false;
		var timeoutFunctionWasCalled = false;
		Routine.run {
			var condition;

			condition = Condition.new;
			condition.test = false;
			condition.hangWithTimeout(\hang, hangTime, { timeoutFunctionWasCalled = true });
			hasContinued = true;

		};

		SystemClock.sched(0.001, {
			this.assert(hasContinued.not, "hangWithTimeout should hang");
		});

		SystemClock.sched(0.001, {
			this.assert(timeoutFunctionWasCalled.not, "hangWithTimeout should not call timeout Function while hanging");
		});

		SystemClock.sched(hangTime + 0.001, {
			this.assert(hasContinued, "hangWithTimeout should time out");
			this.assert(timeoutFunctionWasCalled, "hangWithTimeout should call timeout Function after time out");
		});

	}

	test_hangWithTimeout_unhang {

		var hangTime = 0.1;
		var hasContinued = false;
		var timeoutFunctionWasCalled = false;
		var condition = Condition.new;

		Routine.run {
			condition.test = false;
			condition.hangWithTimeout(\hang, hangTime, { timeoutFunctionWasCalled = true });
			hasContinued = true;
		};

		SystemClock.sched(0.001, {
			this.assert(hasContinued.not, "hangWithTimeout should hang");
		});

		SystemClock.sched(0.001 + (hangTime / 2), {
			condition.test = true;
			condition.signal;
		});

		SystemClock.sched(0.1 + hangTime, {
			this.assert(timeoutFunctionWasCalled.not, "hangWithTimeout should not call timeout Function when unhung externally");
		});

	}


}