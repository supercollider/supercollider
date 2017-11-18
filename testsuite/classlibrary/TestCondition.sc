

TestCondition : UnitTest {


	test_hangWithTimeout {

		var dt = 0.1;
		var hasContinued = false;
		var timeoutFunctionWasCalled = false;
		Routine.run {
			var condition;

			condition = Condition.new;
			condition.test = false;
			condition.hangWithTimeout(\hang, dt, { timeoutFunctionWasCalled = true });
			hasContinued = true;

		};

		SystemClock.sched(0.001, {
			this.assert(hasContinued.not, "hangWithTimeout should hang");
		});

		SystemClock.sched(0.001, {
			this.assert(timeoutFunctionWasCalled.not, "hangWithTimeout should not call timeout Function while hanging");
		});

		SystemClock.sched(dt + 0.001, {
			this.assert(hasContinued, "hangWithTimeout should time out");
			this.assert(timeoutFunctionWasCalled, "hangWithTimeout should call timeout Function after time out");
		})

	}


}