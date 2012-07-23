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

	*new { arg func, stackSize = (512);
		^super.new.init(func, stackSize)
	}
	init { arg argFunc, argStackSize = 512;
		_Thread_Init
		^this.primitiveFailed
	}
	copy { ^this } // sorry cannot copy

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
	randData_ { arg data;
		_Thread_SetRandData
		^this.primitiveFailed;
	}
	randData {
		_Thread_GetRandData
	}
	failedPrimitiveName { _PrimName }

	handleError { arg error;
		(exceptionHandler ? parent).handleError(error)
	}

	// these make Thread act like an Object not like Stream.
	next { ^this }
	value { ^this }
	valueArray { ^this }

	*primitiveError { _PrimitiveError }
	*primitiveErrorString { _PrimitiveErrorString; }

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
	run { arg inval;
		_RoutineResume
		^this.primitiveFailed
	}

	valueArray { arg inval;
		^this.value(inval)
	}

	reset {
		_RoutineReset
		^this.primitiveFailed
	}
		// the _RoutineStop primitive can't stop the currently running Routine
		// but a user should be able to use .stop anywhere
	stop {
		if(this === thisThread) { nil.alwaysYield }
			{ this.prStop };
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
	awake { arg inBeats, inSeconds, inClock;
		var temp = inBeats; // prevent optimization

		^this.next(inBeats)
	}
	prStart { arg inval;
		func.value(inval);
		// if the user's function returns then always yield nil
		nil.alwaysYield;
	}
}
