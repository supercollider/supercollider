PitchShift : UGen {
 	checkInputs { ^this.checkSameRateAsFirstInput }
	*ar { arg in = 0.0, windowSize = 0.2, pitchRatio = 1.0,
			pitchDispersion = 0.0, timeDispersion = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, windowSize, pitchRatio,
			pitchDispersion, timeDispersion).madd(mul, add)
	}
}
