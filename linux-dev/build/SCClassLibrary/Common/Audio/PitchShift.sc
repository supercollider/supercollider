
// not installed yet
PitchShift : UGen {	
	*ar { arg in = 0.0, winSize = 0.2, pchRatio = 1.0, 
			pchDispersion = 0.0, timeDispersion = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, winSize, pchRatio, 
			pchDispersion, timeDispersion).madd(mul, add)
	}
}
