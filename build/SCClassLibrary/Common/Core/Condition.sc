
Condition {
	var <>test, waitingThreads;
	
	*new { arg test=false;
		^super.newCopyArgs(test, Array(8))
	}
	wait { arg value;
		if (test.value.not, {
			waitingThreads = waitingThreads.add(thisThread);
			value.yield;
		});
	}
	hang { arg value;
		// ignore the test, just wait
		waitingThreads = waitingThreads.add(thisThread);
		value.yield;
	}
	
	signal {
		var tempWaitingThreads, time;
		if (test.value, {		
			time = thisThread.seconds;
			tempWaitingThreads = waitingThreads;
			waitingThreads = nil;
			tempWaitingThreads.do({ arg thread; 
				thread.seconds = time;
				thread.resume;
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
			thread.seconds = time;
			thread.resume;
		});
	}
}


