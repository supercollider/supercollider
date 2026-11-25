Line : UGen {
	*ar { |start = 0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0|
		^this.multiNew('audio', start, end, dur, doneAction).madd(mul, add)
	}
	*kr { |start = 0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0|
		^this.multiNew('control', start, end, dur, doneAction).madd(mul, add)
	}
}

XLine : UGen {
	*ar { |start = 1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0|
		^this.multiNew('audio', start, end, dur, doneAction).madd(mul, add)
	}
	*kr { |start = 1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0, doneAction = 0|
		^this.multiNew('control', start, end, dur, doneAction).madd(mul, add)
	}
}

LinExp : PureUGen {
	checkInputs { ^this.checkSameRateAsFirstInput }
	*ar { |in = 0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0|
		^this.multiNew('audio', in, srclo, srchi, dstlo, dsthi)
	}
	*kr { |in = 0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0|
		^this.multiNew('control', in, srclo, srchi, dstlo, dsthi)
	}
}

LinLin {
	*ar { |in = 0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0|
		var scale = (dsthi - dstlo) / (srchi - srclo);
		var offset = dstlo - (scale * srclo);

		^MulAdd(in, scale, offset)
	}
	*kr { |in = 0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0|
		var scale = (dsthi - dstlo) / (srchi - srclo);
		var offset = dstlo - (scale * srclo);

		^(in * scale + offset)
	}
}

AmpComp : PureUGen {
	*ir { |freq = (60.midicps), root = (60.midicps), exp = 0.3333|
		^this.multiNew('scalar', freq, root, exp)
	}
	*ar { |freq, root = (60.midicps), exp = 0.3333|
		^this.multiNew('audio', freq, root, exp)
	}
	*kr { |freq = (60.midicps), root = (60.midicps), exp = 0.3333|
		^this.multiNew('control', freq, root, exp)
	}
	checkInputs {
		if(rate == \audio) { ^this.checkSameRateAsFirstInput };
		^this.checkValidInputs
	}
}

AmpCompA : AmpComp {
	*ir { |freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0|
		^this.multiNew('scalar', freq, root, minAmp, rootAmp)
	}
	*ar { |freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0|
		^this.multiNew('audio', freq, root, minAmp, rootAmp)
	}
	*kr { |freq = 1000, root = 0, minAmp = 0.32, rootAmp = 1.0|
		^this.multiNew('control', freq, root, minAmp, rootAmp)
	}
}

K2A : PureUGen { // control rate to audio rate converter
	*ar { |in = 0.0|
		^this.multiNew('audio', in)
	}
}

A2K : PureUGen { // audio rate to control rate converter. only needed in specific cases
	*kr { |in = 0.0|
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
	*ar { |in = 0.0, offset = 0|
		^this.multiNew('audio', in, offset)
	}
}

DC : PureMultiOutUGen {
	*ar { |in = 0.0|
		^this.multiNew('audio', in)
	}
	*kr { |in = 0.0|
		^this.multiNew('control', in)
	}
	init { |... argInputs|
		inputs = argInputs;
		^this.initOutputs(inputs.size, rate)
	}
}

Silent {
	*ar { |numChannels = 1|
		var sig = DC.ar(0);
		if(numChannels == 1) {
			^sig
		} {
			^(sig ! numChannels)
		}
	}
}
