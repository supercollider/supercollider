
Condition {
	var <>func, waitingThreads;
	
	*new { arg func=false;
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
		// ignore the function, just resume all waiting threads
		time = thisThread.seconds;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do({ arg thread; 
			thread.seconds = time;
			thread.resume;
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
		time = thisThread.seconds;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do({ arg thread; 
			thread.seconds = time;
			thread.resume;
		});
	}
}


WaitFor {
	var waitingThreads;
	
	wait { arg func, value;
		if (func.value.not, {
			waitingThreads = waitingThreads.add([thisThread, func]);
			value.yield;
		});
	}
	signal {
		var tempWaitingThreads, time;
		time = thisThread.seconds;
		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;
		tempWaitingThreads.do({| pair |
			var thread, condition;
			# thread, condition = pair;
			if (condition.value) {
				thread.seconds = time;
				thread.resume;
			}{
				waitingThreads = waitingThreads.add(pair);
			};
		});
	}
}

