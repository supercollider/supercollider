IEnvGen : UGen { // envelope index generator

	*ar { arg envelope, index, mul = 1, add = 0;
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['audio', index, envelope]).madd(mul, add)
	}

	*kr { arg envelope, index, mul = 1, add = 0;
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['control', index, envelope]).madd(mul, add)
	}

	*convertEnv { arg env;
		if(env.isSequenceableCollection) { ^env.reference }; // raw envelope data
		^env.asArrayForInterpolation.collect(_.reference).unbubble
	}

	*new1 { arg rate, index, envArray;
		^super.new.rate_(rate).addToSynth.init([index] ++ envArray.dereference)
	}

 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }

}
