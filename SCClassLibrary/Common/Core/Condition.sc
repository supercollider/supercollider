Condition {
	var <>test, waitingThreads;

	*new { arg test=false;
		^super.newCopyArgs(test, Array(8))
	}
	wait {
		if (test.value.not, {
			waitingThreads = waitingThreads.add(thisThread.threadPlayer);
			\hang.yield;
		});
	}
	hang { arg value = \hang;
		// ignore the test, just wait
		waitingThreads = waitingThreads.add(thisThread.threadPlayer);
		value.yield;
	}

	hangWithTimeout { arg value = \hang, timeout, timeoutFunction;
		var func;
		func = {
			this.test = true;
			this.signal;
			timeoutFunction.value(this);
			func = nil;
		};
		thisThread.clock.sched(timeout, { func.value; nil });
		this.hang(value);
		func = nil;
	}

	signal {
		var tempWaitingThreads, time;
		if (test.value, {
			time = thisThread.seconds;
			tempWaitingThreads = waitingThreads;
			waitingThreads = nil;
			tempWaitingThreads.do({ arg thread;
				thread.clock.sched(0, thread);
			});
		});
	}
	unhang {
		var tempWaitingThreads, time;
		// ignore the test, just resume all waiting threads
		time = thisThread.seconds;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do({ arg thread;
			thread.clock.sched(0, thread);
		});
	}
}

FlowVar {
	var value = \unbound;
	var condition;

	*new { arg inVal = \unbound;
		^super.new.init(inVal)
	}
	init { arg inVal;
		value = inVal;
		condition = Condition { value != \unbound };
	}
	value_ { arg inVal;
		if (value != \unbound) {
			Error("cannot rebind a FlowVar").throw
		};
		value = inVal;
		condition.signal;
	}
	value {
		condition.wait
		^value
	}
}
