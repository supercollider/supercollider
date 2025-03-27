GVerb : MultiOutUGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg in, roomsize = 10, revtime = 3, damping = 0.5, inputbw =  0.5, spread = 15,
			drylevel = 1, earlyreflevel = 0.7, taillevel = 0.5, maxroomsize = 300, mul = 1,
			add = 0;
		^this.multiNew('audio', in, roomsize, revtime, damping, inputbw, spread, drylevel,
			earlyreflevel, taillevel, maxroomsize).madd(mul, add);
	}

	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}

	checkInputs { ^this.checkNInputs(1); }
}
