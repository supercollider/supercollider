IEnvGen : UGen { // envelope index generator	
	*ar { arg ienvelope, index, mul = 1, add = 0;
		var offset;
		ienvelope = ienvelope.isKindOf(Env).if({
			offset = 0.0;
			InterplEnv.new(ienvelope.levels, ienvelope.times, ienvelope.curves);
			}, {
			offset = ienvelope.offset;
			ienvelope;
			});
		^this.multiNewList(['audio', index, offset, `ienvelope]).madd(mul, add);
	} 
	
	*kr { arg ienvelope, index, mul = 1, add = 0;
		var offset;
		ienvelope = ienvelope.isKindOf(Env).if({
			offset = 0.0;
			InterplEnv.new(ienvelope.levels, ienvelope.times, ienvelope.curves)
			}, {
			offset = ienvelope.offset;
			ienvelope
			});
		^this.multiNewList(['control', index, offset, `ienvelope]).madd(mul, add);
	}
	*new1 { arg rate, index, offset, ienvelope, mul = 1, add = 0;
		^super.new.rate_(rate).addToSynth.init([index, offset] 
			++ ienvelope.dereference.asArray).madd(mul, add); 
	}
 	init { arg theInputs;
 		// store the inputs as an array
 		inputs = theInputs;
 	}
	argNamesInputsOffset { ^2 }
}