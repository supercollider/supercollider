
// fft uses a local buffer for holding the buffered audio

FFT : UGen 
{
	*new { arg buffer, in = 0.0;
		^this.multiNew('control', buffer, in)
	}
}	

IFFT : UGen 
{
	*new { arg buffer;
		^this.multiNew('audio', buffer)
	}
}	

PV_MagAbove : UGen
{
	*new { arg buffer, threshold = 0.0;
		^this.multiNew('control', buffer, threshold)
	}
}

PV_MagBelow : PV_MagAbove {}
PV_MagClip : PV_MagAbove {}
PV_LocalMax : PV_MagAbove {}

PV_MagSmear : UGen
{
	*new { arg buffer, bins = 0.0;
		^this.multiNew('control', buffer, bins)
	}
}

PV_BinShift : UGen 
{

	*new { arg buffer, stretch = 1.0, shift = 0.0;
		^this.multiNew('control', buffer, stretch, shift)
	}
}
PV_MagShift : PV_BinShift {}

PV_MagSquared : UGen 
{
	*new { arg buffer;
		^this.multiNew('control', buffer)
	}
}

PV_MagNoise : PV_MagSquared {}
PV_PhaseShift90 : PV_MagSquared {}
PV_PhaseShift270 : PV_MagSquared {}

PV_PhaseShift : UGen 
{
	*new { arg buffer, shift;
		^this.multiNew('control', buffer, shift)
	}
}	

PV_BrickWall : UGen
{
	*new { arg buffer, wipe = 0.0;
		^this.multiNew('control', buffer, wipe)
	}
}

PV_BinWipe : UGen 
{
	*new { arg bufferA, bufferB, wipe = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe)
	}
}

PV_MagMul : UGen
{
	*new { arg bufferA, bufferB;
		^this.multiNew('control', bufferA, bufferB)
	}
}

PV_CopyPhase : PV_MagMul {}
PV_Max : PV_MagMul {}
PV_Min : PV_MagMul {}
PV_Mul : PV_MagMul {}
PV_Add : PV_MagMul {}

PV_RandComb : UGen 
{
	*new { arg buffer, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', buffer, wipe, trig)
	}
}

PV_RectComb : UGen 
{
	*new { arg buffer, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', buffer, numTeeth, phase, width)
	}
}


PV_RectComb2 : UGen 
{
	*new { arg bufferA, bufferB, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', bufferA, bufferB, numTeeth, phase, width)
	}
}

PV_RandWipe : UGen 
{
	*new { arg bufferA, bufferB, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', bufferA, bufferB, wipe, trig)
	}
}

PV_Diffuser : UGen
{
	*new { arg buffer, trig = 0.0;
		^this.multiNew('control', buffer, trig)
	}
}

PV_MagFreeze : UGen
{
	*new { arg buffer, freeze = 0.0;
		^this.multiNew('control', buffer, freeze)
	}
}

PV_BinScramble : UGen
{
	*new { arg buffer, wipe = 0.0, width = 0.2, trig = 0.0;
		^this.multiNew('control', buffer, wipe, width, trig)
	}
}




////////////////////////////////////////////////////
/*
PV_OscBank : UGen 
{
	*new { arg buffer, scale;
		^this.multiNew('control', buffer)
	}
}	

PV_Scope : UGen {}

PV_TimeAverageScope : PV_Scope {}

PV_MagAllTimeAverage : PV_MagSquared {}





PV_MagOnePole : UGen
{
	*new { arg buffer, feedback = 0.0;
		^this.multiNew('control', buffer, feedback)
	}
}

PV_MagPeakDecay : UGen
{
	*new { arg buffer, decay = 0.0;
		^this.multiNew('control', buffer, decay)
	}
}

PV_TimeSmear : PV_MagSmear {}

PV_LoBitEncoder : UGen
{
	*new { arg buffer, levels = 4.0;
		^this.multiNew('control', buffer, levels)
	}
}
*/



