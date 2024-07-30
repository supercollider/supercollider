//third party FFT UGens

//sick lincoln remembers complex analysis courses
PV_ConformalMap : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*new { arg buffer, areal = 0.0, aimag = 0.0;
		^this.multiNew('control', buffer, areal, aimag)
	}
}

//in and kernel are both audio rate changing signals
// Kernel is an audio rate signal.
Convolution : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { arg in, kernel, framesize=512,mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernel, framesize).madd(mul, add);
	}
}

// fixed kernel convolver with fix by nescivi to update the kernel on receipt of a trigger message
// Kernel is a Buffer.
Convolution2 : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg in, kernel, trigger = 0, framesize=2048,mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernel, trigger, framesize).madd(mul, add);
	}
}

// fixed kernel convolver with linear crossfade
// Kernel is a Buffer.
Convolution2L : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg in, kernel, trigger = 0, framesize=2048, crossfade=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernel, trigger, framesize, crossfade).madd(mul, add);
	}
}

//fixed kernel stereo convolver with linear crossfade
// Kernel is a Buffer.
StereoConvolution2L : MultiOutUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg in, kernelL, kernelR, trigger=0, framesize=2048, crossfade=1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernelL, kernelR, trigger, framesize, crossfade).madd(mul, add);
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

//time based convolution by nescivi
// Kernel is a Buffer.
Convolution3 : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg in, kernel, trigger=0, framesize=2048, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, kernel, trigger, framesize).madd(mul, add);
	}
	*kr { arg in, kernel, trigger=0, framesize=2048, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, kernel, trigger, framesize).madd(mul, add);
	}
}


//jensen andersen inspired FFT feature detector
PV_JensenAndersen : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*ar { arg buffer, propsc=0.25, prophfe=0.25, prophfc=0.25, propsf=0.25, threshold=1.0, waittime=0.04;
		^this.multiNew('audio', buffer, propsc, prophfe, prophfc, propsf,  threshold, waittime);
	}
}


PV_HainsworthFoote : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*ar { arg buffer, proph=0.0, propf=0.0, threshold=1.0, waittime=0.04;
		^this.multiNew('audio', buffer, proph, propf, threshold, waittime);
	}
}

//not FFT but useful for time domain onset detection
RunningSum : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { |in, numsamp=40| ^this.multiNew('audio', in, numsamp) }
	*kr { |in, numsamp=40| ^this.multiNew('control', in, numsamp) }
	*rms { |in, numsamp=40| ^(RunningSum.ar(in.squared,numsamp)*(numsamp.reciprocal)).sqrt }
}
