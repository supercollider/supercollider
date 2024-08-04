// fft uses a local buffer for holding the buffered audio.
// wintypes are defined in the C++ source. 0 is default, Welch; 1 is Hann; -1 is rect.

// Depreciated, do not use.
WidthFirstUGen : UGen {
	addCopiesIfNeeded { }
}

FFT : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { | buffer, in = 0.0 , hop = 0.5, wintype = 0 , active = 1, winsize=0|
		^this.multiNew('control', buffer, in, hop, wintype, active, winsize)
	}

	fftSize { ^BufFrames.ir(inputs[0]) }
}

// Prepares a buffer for FFT, but doesn't read from an audio stream.
FFTTrigger : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { | buffer, hop = 0.5, polar = 0.0|
		^this.multiNew('control', buffer, hop, polar)
	}

	fftSize { ^BufFrames.ir(inputs[0]) }
}

IFFT : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { | buffer, wintype = 0, winsize=0|
		^this.ar(buffer, wintype, winsize)
	}

	*ar { | buffer, wintype = 0, winsize=0|
		^this.multiNew('audio', buffer, wintype, winsize)
	}

	*kr { | buffer, wintype = 0, winsize=0|
		^this.multiNew('control', buffer, wintype, winsize)
	}

}

PV_MagAbove : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}

PV_MagBelow : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}
PV_MagClip : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}
PV_LocalMax : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}

PV_MagSmear : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, bins = 0.0;
		^this.multiNew('control', buffer, bins)
	}
}

PV_BinShift : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, stretch = 1.0, shift = 0.0, interp = 0;
		^this.multiNew('control', buffer, stretch, shift, interp)
	}
}

PV_MagShift : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, stretch = 1.0, shift = 0.0;
		^this.multiNew('control', buffer, stretch, shift)
	}
}

PV_MagSquared : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}

PV_MagNoise : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}
PV_PhaseShift90 : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}
PV_PhaseShift270 : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}
PV_Conj : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}

PV_PhaseShift : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, shift, integrate=0;
		^this.multiNew('control', buffer, shift, integrate)
	}
}

PV_BrickWall : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, wipe = 0.0;
		^this.multiNew('control', buffer, wipe)
	}
}

PV_BinWipe : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB, wipe = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe)
	}
}

PV_MagMul : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

PV_CopyPhase : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Copy  : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Max : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Min : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Mul : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Div : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}
PV_Add : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

PV_MagDiv : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB, zeroed = 0.0001;
		^this.multiNew('control', bufferA, bufferB, zeroed)
	}
}

PV_RandComb : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', buffer, wipe, trig)
	}
}

PV_RectComb : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', buffer, numTeeth, phase, width)
	}
}


PV_RectComb2 : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', bufferA, bufferB, numTeeth, phase, width)
	}
}

PV_RandWipe : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg bufferA, bufferB, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe, trig)
	}
}

PV_Diffuser : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, trig = 0.0;
		^this.multiNew('control', buffer, trig)
	}
}

PV_MagFreeze : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, freeze = 0.0;
		^this.multiNew('control', buffer, freeze)
	}
}

PV_BinScramble : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { arg buffer, wipe = 0.0, width = 0.2, trig = 0.0;
		^this.multiNew('control', buffer, wipe, width, trig)
	}
}


