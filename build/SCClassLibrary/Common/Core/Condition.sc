
Condition {
	var <>func, waitingThreads;
	
	*new { arg func;
		^super.newCopyArgs(func)
	}
	wait { arg value;
		if (func.value.not, {
			waitingThreads = waitingThreads.add(thisThread);
			value.yield;
		});
	}
	signal {
		var tempWaitingThreads, time;
		if (func.value, {		
			time = thisThread.time;
			tempWaitingThreads = waitingThreads;
			waitingThreads = nil;
			tempWaitingThreads.do({ arg thread; 
				thread.time = time;
				thread.resume;
			});
		});
	}
}


Wait {
	var waitingThreads;
	
	wait { arg value;
		waitingThreads = waitingThreads.add(thisThread);
		value.yield;
	}
	signal {
		var tempWaitingThreads, time;
		time = thisThread.time;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do({ arg thread; 
			thread.time = time;
			thread.resume;
		});
	}
}