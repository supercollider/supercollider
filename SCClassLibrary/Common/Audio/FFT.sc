// fft uses a local buffer for holding the buffered audio.
// wintypes are defined in the C++ source. 0 is default, Welch; 1 is Hann; -1 is rect.

WidthFirstUGen : UGen
{
	addToSynth {
		synthDef = buildSynthDef;
		if (synthDef.notNil, {
			synthDef.addUGen(this);
			synthDef.widthFirstUGens = synthDef.widthFirstUGens.add(this);
		});
	}
}


FFT : PV_ChainUGen
{
	*new { | buffer, in = 0.0 , hop = 0.5, wintype = 0 , active = 1, winsize=0|
		^this.multiNew('control', buffer, in, hop, wintype, active, winsize)
	}
}

IFFT : WidthFirstUGen
{
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

PV_MagAbove : PV_ChainUGen
{
	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}

PV_MagBelow : PV_MagAbove {}
PV_MagClip : PV_MagAbove {}
PV_LocalMax : PV_MagAbove {}

PV_MagSmear : PV_ChainUGen
{
	*new { arg buffer, bins = 0.0;
		^this.multiNew('control', buffer, bins)
	}
}

PV_BinShift : PV_ChainUGen
{
	*new { arg buffer, stretch = 1.0, shift = 0.0, interp = 0;
		^this.multiNew('control', buffer, stretch, shift, interp)
	}
}

PV_MagShift : PV_ChainUGen
{
	*new { arg buffer, stretch = 1.0, shift = 0.0;
		^this.multiNew('control', buffer, stretch, shift)
	}
}

PV_MagSquared : PV_ChainUGen
{
	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}

PV_MagNoise : PV_MagSquared {}
PV_PhaseShift90 : PV_MagSquared {}
PV_PhaseShift270 : PV_MagSquared {}
PV_Conj : PV_MagSquared {}

PV_PhaseShift : PV_ChainUGen
{
	*new { arg buffer, shift, integrate=0;
		^this.multiNew('control', buffer, shift, integrate)
	}
}

PV_BrickWall : PV_ChainUGen
{
	*new { arg buffer, wipe = 0.0;
		^this.multiNew('control', buffer, wipe)
	}
}

PV_BinWipe : PV_ChainUGen
{
	*new { arg bufferA, bufferB, wipe = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe)
	}
}

PV_MagMul : PV_ChainUGen
{
	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

PV_CopyPhase : PV_MagMul {}
PV_Copy : PV_MagMul {}
PV_Max : PV_MagMul {}
PV_Min : PV_MagMul {}
PV_Mul : PV_MagMul {}
PV_Div : PV_MagMul {}
PV_Add : PV_MagMul {}

PV_MagDiv : PV_ChainUGen
{
	*new { arg bufferA, bufferB, zeroed = 0.0001;
		^this.multiNew('control', bufferA, bufferB, zeroed)
	}
}

PV_RandComb : PV_ChainUGen
{
	*new { arg buffer, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', buffer, wipe, trig)
	}
}

PV_RectComb : PV_ChainUGen
{
	*new { arg buffer, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', buffer, numTeeth, phase, width)
	}
}


PV_RectComb2 : PV_ChainUGen
{
	*new { arg bufferA, bufferB, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', bufferA, bufferB, numTeeth, phase, width)
	}
}

PV_RandWipe : PV_ChainUGen
{
	*new { arg bufferA, bufferB, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe, trig)
	}
}

PV_Diffuser : PV_ChainUGen
{
	*new { arg buffer, trig = 0.0;
		^this.multiNew('control', buffer, trig)
	}
}

PV_MagFreeze : PV_ChainUGen
{
	*new { arg buffer, freeze = 0.0;
		^this.multiNew('control', buffer, freeze)
	}
}

PV_BinScramble : PV_ChainUGen
{
	*new { arg buffer, wipe = 0.0, width = 0.2, trig = 0.0;
		^this.multiNew('control', buffer, wipe, width, trig)
	}
}

FFTTrigger : PV_ChainUGen
{
	*new { | buffer, hop = 0.5, polar = 0.0|
		^this.multiNew('control', buffer, hop, polar)
	}
}


////////////////////////////////////////////////////
/*
PV_OscBank : PV_ChainUGen
{
	*new { arg buffer, scale;
		^this.multiNew('control', buffer)
	}
}

PV_Scope : PV_ChainUGen {}

PV_TimeAverageScope : PV_Scope {}

PV_MagAllTimeAverage : PV_MagSquared {}





PV_MagOnePole : PV_ChainUGen
{
	*new { arg buffer, feedback = 0.0;
		^this.multiNew('control', buffer, feedback)
	}
}

PV_MagPeakDecay : PV_ChainUGen
{
	*new { arg buffer, decay = 0.0;
		^this.multiNew('control', buffer, decay)
	}
}

PV_TimeSmear : PV_MagSmear {}

PV_LoBitEncoder : PV_ChainUGen
{
	*new { arg buffer, levels = 4.0;
		^this.multiNew('control', buffer, levels)
	}
}
*/
