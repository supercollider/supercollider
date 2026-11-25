Condition {
	var <>test, waitingThreads;

	*new { |test = false|
		^super.newCopyArgs(test, Array(8))
	}
	wait {
		if(test.value.not) {
			waitingThreads = waitingThreads.add(thisThread.threadPlayer);
			\hang.yield
		}
	}
	hang { |value = \hang|
		// ignore the test, just wait
		waitingThreads = waitingThreads.add(thisThread.threadPlayer);
		value.yield
	}

	signal {
		var tempWaitingThreads, time;
		if(test.value) {
			time = thisThread.seconds;
			tempWaitingThreads = waitingThreads;
			waitingThreads = nil;
			tempWaitingThreads.do { |thread|
				thread.clock.sched(0, thread)
			}
		}
	}
	unhang {
		var tempWaitingThreads, time;
		// ignore the test, just resume all waiting threads
		time = thisThread.seconds;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do { |thread|
			thread.clock.sched(0, thread)
		}
	}
}

FlowVar {
	var value = \unbound;
	var condition;

	*new { |inVal = \unbound|
		^super.new.init(inVal)
	}
	init { |inVal|
		value = inVal;
		condition = Condition { value != \unbound }
	}
	value_ { |inVal|
		if(value != \unbound) {
			Error("cannot rebind a FlowVar").throw
		};
		value = inVal;
		condition.signal
	}
	value {
		condition.wait
		^value
	}
}
