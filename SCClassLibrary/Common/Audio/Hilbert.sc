Hilbert : MultiOutUGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg in, mul = 1, add = 0;
		^this.multiNew('audio', in).madd(mul, add);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

// class using FFT (with a delay) for better results than the above UGen
// buffer should be 2048 or 1024
// 2048, better results, more delay
// 1024, less delay, little choppier results

// TODO: should this inherit from UGen as it is a psuedo ugen
HilbertFIR : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg in, buffer;
		var fft, delay;
		fft = FFT(buffer, in);
		fft = PV_PhaseShift90(fft);
		delay = BufDur.kr(buffer);
		// return [source, shift90]
		^[DelayN.ar(in, delay, delay), IFFT(fft)];
	}
}

// single sideband amplitude modulation, using optimized Hilbert phase differencing network
// basically coded by Joe Anderson, except Sean Costello changed the word HilbertIIR.ar
// to Hilbert.ar

FreqShift : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar {
		arg in,			// input signal
		freq = 0.0,		// shift, in cps
		phase = 0.0,	// phase of SSB
		mul = 1.0,
		add = 0.0;
		^this.multiNew('audio', in, freq, phase).madd(mul, add)
	}
}
