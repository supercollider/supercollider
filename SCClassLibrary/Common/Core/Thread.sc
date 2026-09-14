// you must not make any change at all to the order or number of
// instance variables in these classes!
// You should also not muck with the contents of the instance
// variables unless you are sure you know what you are doing.
// You may add methods.

// Thread inherits from Stream for the benefit of its subclass Routine which can
// behave like a Stream. Thread itself is not used like a Stream.

Thread : Stream {
	var <state=0, func, stack, method, block, frame, ip=0, sp=0;
	var numpop=0, receiver, numArgsPushed=0;
	var <parent, terminalValue;
	var <primitiveError=0, <primitiveIndex=0, randData=0;
	var <beats=0.0, <seconds=0.0, <clock, <nextBeat, <>endBeat, <>endValue;
	var environment;
	var <>exceptionHandler, >threadPlayer;
	var <executingPath, <oldExecutingPath;
	var rescheduledTime;

	*new { |func, stackSize(512), exceptionHandler([])|
		^super.new.init(func, stackSize).exceptionHandler_(exceptionHandler)
	}

	init { arg argFunc, argStackSize = 512;
		_Thread_Init
		^this.primitiveFailed
	}

	copy { ^this } // sorry cannot copy

	play { |clock, quant|
		this.exceptionHandler_(thisThread.exceptionHandler.asArray.copy);
		clock = clock ? TempoClock.default;
		clock.play(this, quant.asQuant);
	}

	clock_ { arg inClock;
		clock = inClock;
		beats = clock.secs2beats(seconds);
	}
	seconds_ { arg inSeconds; seconds = inSeconds; beats = clock.secs2beats(inSeconds); }
	beats_ { arg inBeats; beats = inBeats; seconds = clock.beats2secs(inBeats); }
	isPlaying { ^state == 5 }

	threadPlayer { ^threadPlayer ?? { this.findThreadPlayer } }
	findThreadPlayer {
		var parent = this.parent;
		^if(parent.notNil and: { parent !== thisProcess.mainThread }) {
			parent.threadPlayer
		} {
			this
		}
	}

	deferAwayFrom { |func, delta = 0|
		if(this === thisThread or: { delta > 0 }) {
			func.defer(delta)
		} {
			func.value
		}
	}

	randSeed_ { arg seed;
		// You supply an integer seed.
		// This method creates a new state vector and stores it in randData.
		// A state vector is an Int32Array of three 32 bit words.
		// SuperCollider uses the taus88 random number generator which has a
		// period of 2**88, and passes all standard statistical tests.
		// Normally Threads inherit the randData state vector from the Thread that created it.
		_Thread_RandSeed
		^this.primitiveFailed;
	}
	randData_ {
		arg data;
		_Thread_SetRandData
		^this.primitiveFailed
	}
	randData {
		_Thread_GetRandData
		^this.primitiveFailed
	}
	failedPrimitiveName {
		_PrimName
		^this.primitiveFailed
	}

	handleError { |error| 
		^if (this.exceptionHandler.size != 0)  {
			this.exceptionHandler.pop.handleError(error)
		} {
			this.parent.handleError(error);
		}
	}

	// these make Thread act like an Object not like Stream.
	next { ^this }
	value { ^this }
	valueArray { ^this }

	*primitiveError {
		_PrimitiveError
		^this.primitiveFailed
	}
	*primitiveErrorString {
		_PrimitiveErrorString
		^this.primitiveFailed
	}

	storeOn { arg stream; stream << "nil"; }
	archiveAsCompileString { ^true }
	checkCanArchive { "cannot archive Threads".warn }
}

Routine : Thread {

	*run { arg func, stackSize, clock, quant;
		var routine = super.new(func, stackSize);
		^routine.play(clock ? SystemClock, quant);
	}

	// resume, next, value, run are synonyms
	next { arg inval;
		_RoutineResume
		^this.primitiveFailed
	}
	value { arg inval;
		_RoutineResume
		^this.primitiveFailed
	}
	resume { arg inval;
		_RoutineResume
		^this.primitiveFailed
	}
	reschedule { arg argClock, quant;
		deferAwayFrom(this) {
			// Thread:isPlaying only answers if the thread is waiting
			// It *doesn't* confirm that it is actually scheduled on a clock
			if(this.nextBeat.isNil) {
				Error("% can't be rescheduled when idle; use 'play' instead".format(this.class.name)).throw;
			};
			rescheduledTime = quant.asQuant.nextTimeOnGrid(clock, this.nextBeat);
			if(argClock.isNil) { argClock = clock };
			if(argClock !== clock) {
				// convert to new clock's time
				rescheduledTime = argClock.secs2beats(clock.beats2secs(rescheduledTime));
			};
			clock = argClock;
		}
	}


	reset {
		_RoutineReset
		^this.primitiveFailed
	}

	// The _RoutineStop primitive can't stop the currently running Routine
	// but a user should be able to use .stop anywhere
	stop {
		if(this === thisThread) { nil.alwaysYield } { this.prStop };
	}
	prStop {
		_RoutineStop
		^this.primitiveFailed
	}

	p { ^Prout(func) }

	storeArgs { ^[func] }
	storeOn { arg stream;
		stream << this.class.name;
		this.storeParamsOn(stream);
		this.storeModifiersOn(stream);
	}

	// PRIVATE
	// This method is called in the interpreter from C++ code whenever any thread/clock (e.g., LinkClock) wishes to awaken a task.
	awake { arg inBeats, inSeconds, inClock;
		if(rescheduledTime.isNil) {
			clock = inClock;
			^this.next(inBeats)
		} {
			// rescheduling, possibly on a new clock
			clock.schedAbs(rescheduledTime, this);
			rescheduledTime = nil;
			^nil
		}
	}
	// This is called by prRoutineResume and is called once *this* thread has become *thisThread*.
	prStart { arg inval;
		func.value(inval);
		// if the user's function returns then always yield nil
		nil.alwaysYield;
	}
}
