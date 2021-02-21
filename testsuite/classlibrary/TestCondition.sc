TestCondition : UnitTest {

	test_condition_awakesBySignal {
		var value = false,
		cond = Condition({ value });
		// fairly sure that review will require this time to be 0
		// but I'm not sure that actually makes sense
		thisThread.clock.sched(0, {
			value = true;
			cond.signal;
		});
		// note, if Condition is ever broken, then we hang here forever
		cond.wait;
		this.assert(value, "Condition should wake up threads when a 'true' condition is signaled");
	}

	test_condition_awakesByUnhang {
		var cond = Condition.new;
		thisThread.clock.sched(0, {
			cond.unhang;
		});
		// note, if Condition is ever broken, then we hang here forever
		cond.hang;
		// if you got this far, then we passed
		this.assert(true, "Condition should wake up threads upon unhang");
	}

	test_timeout {
		var condition = Condition.new;
		var failed = false;
		var lateTask = fork {
			0.12.wait;
			failed = true;
			condition.unhang;
		};
		condition.timeoutAfter(1e-6).hang;
		lateTask.stop;
		this.assert(failed.not, "condition has timed out")
	}

	test_condition_signalsAndBypassesTimeout {
		var result = true,
		value = false,
		cond = Condition({ value }),
		thread = Routine {
			var clock = thisThread.clock;
			cond.timeoutAfter(0.001, { result = false }).wait;
		};
		thread.play(thisThread.clock);
		0.0005.wait;  // no, there is not any other way to do this
		value = true;
		cond.signal;
		0.001.wait;
		this.assert(result, "Successful signal before timeout should cancel the timeout");
	}

	test_condition_unhangsAndBypassesTimeout {
		var result = true,
		cond = Condition.new,
		thread = Routine {
			var clock = thisThread.clock;
			cond.timeoutAfter(0.001, { result = false }).hang;
		};
		thread.play(thisThread.clock);
		0.0005.wait;
		cond.unhang;
		0.001.wait;
		this.assert(result, "Successful unhang before timeout should cancel the timeout");
	}
}
