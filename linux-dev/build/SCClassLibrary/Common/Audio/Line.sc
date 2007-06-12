Line : UGen {	
	*ar { arg start=0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0;
		^this.multiNew('audio', start, end, dur, doneAction).madd(mul, add)
	}
	*kr { arg start=0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0;
		^this.multiNew('control',  start, end, dur, doneAction).madd(mul, add)
	}
}

XLine : UGen {	
	*ar { arg start=1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0;
		^this.multiNew('audio', start, end, dur, doneAction).madd(mul, add)
	}
	*kr { arg start=1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0;
		^this.multiNew('control',  start, end, dur, doneAction).madd(mul, add)
	}
}

LinExp : UGen {
	*ar { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('audio', in, srclo, srchi, dstlo, dsthi)
	}
	*kr { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('control',  in, srclo, srchi, dstlo, dsthi)
	}
}	
// not installed
K2A : UGen { // control rate to audio rate converter - used internally on outputs to Synth
	*ar { arg in = 0.0;
		^this.multiNew('audio', in)
	}
}
