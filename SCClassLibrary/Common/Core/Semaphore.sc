Semaphore {
	var <count, waitingThreads;

	*new { | count=1 |
		^super.newCopyArgs(count, LinkedList.new)
	}
	clear {
		waitingThreads = LinkedList.new;
	}
	wait {
		count = count - 1;
		if (count < 0) {
			waitingThreads.add(thisThread.threadPlayer);
			\hang.yield;
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
