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
	checkInputs { ^this.checkSameRateAsFirstInput }
	*ar { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('audio', in, srclo, srchi, dstlo, dsthi)
	}
	*kr { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('control',  in, srclo, srchi, dstlo, dsthi)
	}
}

LinLin : UGen {
	checkInputs { ^this.checkSameRateAsFirstInput }
	*ar { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('audio', in, srclo, srchi, dstlo, dsthi)
	}
	*kr { arg in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;
		^this.multiNew('control',  in, srclo, srchi, dstlo, dsthi)
	}
}

AmpComp : UGen {
	*ir { arg freq = 60.midicps, root = 60.midicps, exp = 0.3333;
		^this.multiNew('scalar', freq, root, exp)
	}
	*ar { arg freq = 60.midicps, root = 60.midicps, exp = 0.3333;
		^this.multiNew('audio', freq, root, exp)
	}
	*kr { arg freq = 60.midicps, root = 60.midicps, exp = 0.3333;
		^this.multiNew('control', freq, root, exp)
	}
	checkInputs { ^if(rate === \audio) { this.checkSameRateAsFirstInput } }
}

AmpCompA : AmpComp {
	*ir { arg freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0;
		^this.multiNew('scalar', freq, root, minAmp, rootAmp)
	}
	*ar { arg freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0;
		^this.multiNew('audio', freq, root, minAmp, rootAmp)
	}
	*kr { arg freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0;
		^this.multiNew('control', freq, root, minAmp, rootAmp)
	}
}

K2A : UGen { // control rate to audio rate converter
	*ar { arg in = 0.0;
		^this.multiNew('audio', in)
	}
}

A2K : UGen { // audio rate to control rate converter. only needed in specific cases
	*kr { arg in = 0.0;
		^this.multiNew('control', in)
	}
}

T2K : A2K { // audio rate to control rate trigger converter.
	checkInputs {
		if(inputs.at(0).rate != \audio) {
			^"first input is not audio rate"
		};
		^nil
	}
}

T2A : K2A { // control rate to audio rate trigger converter.
	*ar { arg in = 0.0, offset = 0;
		^this.multiNew('audio', in, offset)
	}
}

DC : MultiOutUGen {
	*ar { arg in=0.0;
		^this.multiNew('audio', in)
	}
	*kr { arg in=0.0;
		^this.multiNew('control', in)
	}
	init { arg ... argInputs;
		inputs = argInputs;
		^this.initOutputs(inputs.size, rate)
	}
}

Silent : MultiOutUGen {
	*ar { arg numChannels = 1;
		^this.multiNew('audio', numChannels)
	}
	init { arg numChannels;
		inputs = [];
		^this.initOutputs(numChannels, rate)
	}
}
