IEnvGen : UGen { // envelope index generator

	*ar { |envelope, index, mul = 1, add = 0|
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['audio', index, envelope]).madd(mul, add)
	}

	*kr { |envelope, index, mul = 1, add = 0|
		envelope = this.convertEnv(envelope);
		^this.multiNewList(['control', index, envelope]).madd(mul, add)
	}

	*convertEnv { |env|
		if(env.isSequenceableCollection) { ^env.reference }; // raw envelope data
		^env.asArrayForInterpolation.collect(_.reference).unbubble
	}

	*new1 { |rate, index, envArray|
		^super.new.rate_(rate).addToSynth.init([index] ++ envArray.dereference)
	}

	init { |theInputs|
		// store the inputs as an array
		inputs = theInputs
	}
	argNamesInputsOffset { ^2 }

}
