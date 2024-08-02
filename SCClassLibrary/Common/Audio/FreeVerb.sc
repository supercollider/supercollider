// blackrain's freeverb ugen.

FreeVerb : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	checkInputs { ^this.checkSameRateAsFirstInput }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg in, mix = 0.33, room = 0.5, damp = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, mix, room, damp).madd(mul, add)
	}
}

FreeVerb2 : MultiOutUGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	checkInputs { ^this.checkNInputs(2); }
	canBeReplacedByIdenticalCall { ^true }

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
