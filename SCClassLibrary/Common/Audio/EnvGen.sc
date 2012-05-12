Done : UGen {
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
		if(env.isSequenceableCollection) { ^env.reference }; // raw envelope data
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
