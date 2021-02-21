Condition {
	var <>test, waitingThreads, waitingTimeouts;

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

	setTimeout { |timeout, action|
		var waitingThread = thisThread.threadPlayer;
		var timeoutThread = Routine {
			timeout.wait;
			waitingThreads.remove(waitingThread);
			waitingTimeouts.remove(timeoutThread);
			action.value;
			waitingThread.clock.sched(0, waitingThread);
		};
		waitingTimeouts = waitingTimeouts.add(timeoutThread);
		timeoutThread.play(thisThread.clock);
	}

	signal {
		if(test.value) { this.unhang }
	}
	unhang {
		var tempWaitingThreads;
		// ignore the test, just resume all waiting threads
		waitingTimeouts.do { |thread|
			thread.stop;
		};
		waitingTimeouts = nil;
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
