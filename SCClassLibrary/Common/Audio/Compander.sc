Amplitude : UGen {
	*ar { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, attackTime, releaseTime).madd(mul, add)
	}
	*kr { arg in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, attackTime, releaseTime).madd(mul, add)
	}
}

Compander : UGen {
	*ar { arg in = 0.0, control = 0.0, thresh = 0.5, slopeBelow = 1.0, slopeAbove = 1.0,
		clampTime = 0.01, relaxTime = 0.1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, control, thresh, slopeBelow, slopeAbove,
			clampTime, relaxTime).madd(mul, add)
	}
}


// CompanderD passes the signal directly to the control input,
// but adds a delay to the process input so that the lag in the gain
// clamping will not lag the attacks in the input sound

CompanderD : UGen {
	*ar { arg in = 0.0, thresh = 0.5, slopeBelow = 1.0, slopeAbove = 1.0,
		clampTime = 0.01, relaxTime = 0.01, mul = 1.0, add = 0.0;

		^Compander.ar(DelayN.ar(in, clampTime, clampTime), in, thresh,
				slopeBelow, slopeAbove, clampTime, relaxTime).madd(mul, add)
	}
}


Normalizer : UGen {
	var buffer;
	*ar { arg in = 0.0, level = 1.0, dur = 0.01;
		^this.multiNew('audio', in, level, dur)
	}
}

Limiter : Normalizer {}
