Done : UGen {
	// aliases for UGen doneActions
	const <none = 0;
	const <pauseSelf = 1;
	const <freeSelf = 2;
	const <freeSelfAndPrev = 3;
	const <freeSelfAndNext = 4;
	const <freeSelfAndFreeAllInPrev = 5;
	const <freeSelfAndFreeAllInNext = 6;
	const <freeSelfToHead = 7;
	const <freeSelfToTail = 8;
	const <freeSelfPausePrev = 9;
	const <freeSelfPauseNext = 10;
	const <freeSelfAndDeepFreePrev = 11;
	const <freeSelfAndDeepFreeNext = 12;
	const <freeAllInGroup = 13;
	const <freeGroup = 14;

	*kr { arg src;
		^this.multiNew('control', src)
	}
}

FreeSelf : UGen {
	*kr { arg in;
		this.multiNew('control', in);
		^in
	}
}

PauseSelf : UGen {
	*kr { arg in;
		this.multiNew('control', in);
		^in
	}
}

FreeSelfWhenDone : UGen {
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

PauseSelfWhenDone : UGen {
	*kr { arg src;
		^this.multiNew('control', src)
	}
}

Pause : UGen {
	*kr { arg gate, id;
		^this.multiNew('control', gate, id)
	}
}

Free : UGen {
	*kr { arg trig, id;
		^this.multiNew('control', trig, id)
	}
}

EnvGen : UGen { // envelope generator
	*ar { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['audio', gate, levelScale, levelBias, timeScale, doneAction, envelope])
	}
	*kr { arg envelope, gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale = 1.0, doneAction = 0;
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['control', gate, levelScale, levelBias, timeScale, doneAction, envelope])
	}
	*convertEnv { arg env;
		if(env.isSequenceableCollection) {
			if (env.shape.size == 1) {
				^env.reference
			} {
				// multi-channel envelope
				^env.collect(_.reference)
			};
		};
		^env.asMultichannelArray.collect(_.reference).unbubble
	}
	*new1 { arg rate, gate, levelScale, levelBias, timeScale, doneAction, envArray;
		^super.new.rate_(rate).addToSynth.init([gate, levelScale, levelBias, timeScale, doneAction]
			++ envArray.dereference);
	}
	init { arg theInputs;
		// store the inputs as an array
		inputs = theInputs;
	}
	argNamesInputsOffset { ^2 }
}

Linen : UGen {
	*kr { arg gate = 1.0, attackTime = 0.01, susLevel = 1.0, releaseTime = 1.0, doneAction = 0;
		^this.multiNew('control', gate, attackTime, susLevel, releaseTime, doneAction)
	}
}
