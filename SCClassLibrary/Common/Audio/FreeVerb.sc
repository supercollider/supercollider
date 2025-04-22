// blackrain's freeverb ugen.

FreeVerb : UGen {
	resourceDependencies { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	checkInputs { ^this.checkSameRateAsFirstInput }

	*ar { arg in, mix = 0.33, room = 0.5, damp = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, mix, room, damp).madd(mul, add)
	}
}

FreeVerb2 : MultiOutUGen {
	resourceDependencies { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	checkInputs { ^this.checkNInputs(2) }

	*ar { arg in, in2, mix = 0.33, room = 0.5, damp = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, in2, mix, room, damp).madd(mul, add)
	}

	init { arg ... theInputs;
		inputs = theInputs;
		channels = [
			OutputProxy(rate, this, 0),
			OutputProxy(rate, this, 1)
		];
		^channels
	}
}
