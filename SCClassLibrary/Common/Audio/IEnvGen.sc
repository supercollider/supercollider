IEnvGen : UGen { // envelope index generator

	*ar { arg envelope, index, mul = 1, add = 0;
		^this.multiNewList(['audio', index, envelope]).madd(mul, add)
	}

	*kr { arg envelope, index, mul = 1, add = 0;
		^this.multiNewList(['control', index, envelope]).madd(mul, add)
	}

	*new1 { arg rate, index, envelope;
		^super.new.rate_(rate).addToSynth.init([index] ++ envelope.asArrayForInterpolation)
	}

 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }

}
