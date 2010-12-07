IEnvGen : UGen { // envelope index generator

	*ar { arg ienvelope, index, mul = 1, add = 0;
		var offset;
		ienvelope = ienvelope.isKindOf(Env).if({
			InterplEnv.new(ienvelope.levels, ienvelope.times, ienvelope.curves);
			}, {
			ienvelope;
			});
		^this.multiNewList(['audio', index, `ienvelope]).madd(mul, add);
	}

	*kr { arg ienvelope, index, mul = 1, add = 0;
		var offset;
		ienvelope = ienvelope.isKindOf(Env).if({
			InterplEnv.new(ienvelope.levels, ienvelope.times, ienvelope.curves)
			}, {
			ienvelope
			});
		^this.multiNewList(['control', index, `ienvelope]).madd(mul, add);
	}

	*new1 { arg rate, index, ienvelope, mul = 1, add = 0;
		^super.new.rate_(rate).addToSynth.init([index]
			++ ienvelope.dereference.asArray).madd(mul, add);
	}

 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }

}