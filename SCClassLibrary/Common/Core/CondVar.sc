CondVar {
	var waitingThreads;

	*new { ^super.newCopyArgs(Array.new) }

	wait { |predicate|
		if(predicate.isNil) {
			this.prWait
		} {
			// If the predicate is already true, we return immediately. If not,
			// we block until the predicate is true after being signalled.
			// The waiting thread may be signaled several times before the predicate
			// becomes true, hence the loop is necessary.
			while { predicate.value.not } { this.prWait }
		}
	}

	waitFor { |timeoutBeats, predicate|
		var endingBeats;

		// May throw if preconditions are not met.
		timeoutBeats = this.prConvertTimeoutBeatsToSafeValue(timeoutBeats);

		// This is a slightly more complex version of the body of `wait`. The
		// extra complexity comes from maintaining the timeout requirement.
		if(predicate.isNil) {
			// Interface requirement: immediately return if timeout is non-positive
			if(timeoutBeats <= 0) { ^false };
			^this.prWaitFor(timeoutBeats)
		} {
			endingBeats = thisThread.beats + timeoutBeats;

			// The waiting thread may be signaled several times before the predicate
			// becomes true, so the loop is necessary.
			while {
				predicate.value.not
			} {
				// We only get here when the predicate is not true, so we can return
				// false immediately if we are timed out.
				if(thisThread.beats >= endingBeats) { ^false };
				// If the timeout expires, return the value of the predicate; otherwise,
				// we were signalled, so go back through the loop.
				if(this.prWaitFor(endingBeats - thisThread.beats).not) { ^predicate.value };
			};

			^true
		}
	}

	signalOne {
		if (waitingThreads.isEmpty.not) {
			this.prWakeThread(waitingThreads.removeAt(0));
		}
	}

	signalAll {
		waitingThreads.do(this.prWakeThread(_));
		waitingThreads = Array.new;
	}

	// Also disables `copy`, since it redirects to `shallowCopy` on Object
	shallowCopy { this.shouldNotImplement(thisMethod) }
	deepCopy { this.shouldNotImplement(thisMethod) }

	prWait {
		this.prSleepThread(thisThread);
		\wait.yield
	}

	// Returns true iff we were woken via signal (and not timeout)
	prWaitFor { |timeoutBeats|
		// precondition: timeoutBeats is a Float or Integer, positive, and not inf
		var waitingThread = thisThread;
		var didNotTimeout = true;
		var timeoutThread = Routine {
			var wokenThread;

			timeoutBeats.wait;

			didNotTimeout = false;

			// If our waiting thread is signaled before this timeout fires
			// the timeout routine will be stopped after prWait below
			// and we won't get here.
			wokenThread = this.prRemoveWaitingThread(waitingThread);
			// if wokenThread is nil, we asssume a signal happened
			// immediately prior to timeout and the thread is already awake
			// no call to prWakeThread happens in this case
			if(wokenThread.notNil) { this.prWakeThread(wokenThread) };
		};

		timeoutThread.play(thisThread.clock);
		this.prWait;
		timeoutThread.stop;

		^didNotTimeout
	}

	prSleepThread { |t| waitingThreads = waitingThreads.add(t.threadPlayer) }
	prWakeThread { |t| t.clock.sched(0, t) }
	prRemoveWaitingThread { |t| ^waitingThreads.remove(t.threadPlayer) }

	// Precondition checks for `waitFor` timeout. May throw.
	prConvertTimeoutBeatsToSafeValue { |n|
		n = case
			{ n.class === Float or: { n.class === Integer } } { n }
			{ n.respondsTo(\asInteger) } { n.asInteger }
			{ n.respondsTo(\asFloat) } { n.asFloat }
			{ n };

		if(n.class !== Float and: { n.class !== Integer }) {
			Error("Timeout must be a Float or Integer, or convertible to one").throw
		};

		if(n.isNaN) { Error("Timeout must not be NaN").throw };

		// -inf is checked in negative-time check after this.
		if(n === inf) { Error("Timeout must not be inf; use `wait` instead").throw };

		^n
	}
}
