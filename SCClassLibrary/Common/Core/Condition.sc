// Basic synchronization object
Sync {
	var waitingThreads;

	*new {
		^super.newCopyArgs(Array(8));
	}

	hang { |value = \hang|
		this.prAddThread(thisThread);
		value.yield;
	}

	wait {
		this.hang;
	}

	unhang {
		var tempWaitingThreads;

		tempWaitingThreads = waitingThreads;
		waitingThreads = nil;

		tempWaitingThreads.do({ arg thread;
			thread.clock.sched(0, thread);
		});
	}

	prAddThread { |thread|
		waitingThreads = (waitingThreads ?? { IdentitySet() }).add(thread.threadPlayer);
	}

	prResumeThread { |thread|
		if (waitingThreads.remove(thread.threadPlayer).notNil) {
			thread.clock.sched(0, thread.threadPlayer);
		}
	}
}

Condition : Sync {
	var <>test, pollRoutine;

	*new { arg test=false;
		^super.new.test_(test)
	}

	*timeout { |seconds=1, clock=(SystemClock)|
		var time = (clock.seconds + seconds);
		var new = Condition({ clock.seconds >= time });
		clock.schedAbs(time, { new.unhang() });
		^new
	}

	wait { |timeout, clock=(SystemClock)|
		var waitingThread = thisThread;

		if (test.value.not) {
			timeout !? {
				clock.schedAbs(clock.seconds + timeout, {
					this.prResumeThread(waitingThread);
				})
			};

			this.hang
		};
	}

	signal {
		if (test.value) {
			this.unhang
		}
	}

	then { |function|
		fork {
			this.wait();
			function.value(this);
		}
	}

	update { this.signal() }

	poll { |delta=1|
		pollRoutine.stop();
		pollRoutine = Routine({
			while { test.value.not } {
				delta.wait;
			};
			this.unhang();
		});
		pollRoutine.play();
	}

	whenAny {
		|...args| // function, condition, function, condition
		var thread, finished;

		args = [this] ++ args;
		args = args.clump(2);

		finished = args.detect({ |pair| pair[0].test.value });

		if (finished.notNil) {
			finished[1].value()
		} {
			thread = fork({
				var finished;
				\hang.yield;
				finished = args.detect({ |pair| pair[0].test.value });
				finished !? { finished[1].value() };
			});

			args.do {
				|pair|
				pair[0].prAddThread(thread);
			};
		};
	|| { |other|
		var new = Condition({ this.test.value or: { other.test.value } });
		fork { this.wait(); new.signal(); };
		fork { other.wait(); new.signal(); };
		^new
	}

	&& { |other|
		var new = Condition({ this.test.value and: { other.test.value } });
		fork { this.wait(); other.wait(); new.signal(); };
		^new
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
