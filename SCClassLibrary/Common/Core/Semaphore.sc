Semaphore {
	var <count, waitingThreads;

	*new { | count=1 |
		^super.newCopyArgs(count, LinkedList.new)
	}
	clear {
		waitingThreads = LinkedList.new;
	}
	wait {
		if (count > 0) {
			count = count - 1;
		} {
			waitingThreads.add(thisThread);
			nil.yield;
		};
	}
	signal {
		var thread;
		count = count + 1;
		thread = waitingThreads.popFirst;
		if (thread.notNil) {
			thread.clock.sched(0, thread);
		};
	}
}
