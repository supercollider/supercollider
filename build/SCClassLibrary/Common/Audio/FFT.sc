
PhaseVocoderUGen : UGen
{
	var <fftSize, cosTable, inputWindow, outputWindow;
	
	init { arg argFFTSize ... argInputs;
		inputs = argInputs;
		fftSize = argFFTSize;
		cosTable = Library.at(\signal, \cosTable, fftSize);
		inputWindow = outputWindow = Library.at(\signal, \welch, fftSize);
	}
}

PV_Noop : PhaseVocoderUGen {
	// this class just for debugging
	*ar { arg fftSize = 1024, in = 0.0;
		^this.multiNew('audio', fftSize, in)
	}
	*kr { arg fftSize = 1024, in = 0.0;
		^this.multiNew('control', fftSize, in)
	}
}

PV_Scope : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0;
		^this.multiNew('audio', fftSize, in)
	}
	*kr { arg fftSize = 1024, in = 0.0;
		^this.multiNew('control', fftSize, in)
	}
	init { arg argFFTSize ... argInputs;
		inputs = argInputs;
		fftSize = argFFTSize;
		cosTable = Library.at(\signal, \cosTable, fftSize);
		inputWindow = outputWindow = Library.at(\signal, \hanning, fftSize);
	}
}

PV_TimeAverageScope : PV_Scope {}

PV_ApxTest : PhaseVocoderUGen {
	// this class just for debugging
	*ar { arg fftSize = 1024, in = 0.0;
		^this.multiNew('audio', fftSize, in)
	}
	*kr { arg fftSize = 1024, in = 0.0;
		^this.multiNew('control', fftSize, in)
	}
}

PV_MagSquared : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0;
		^this.multiNew('audio', fftSize, in)
	}
	*kr { arg fftSize = 1024, in = 0.0;
		^this.multiNew('control', fftSize, in)
	}
}

PV_MagNoise : PV_MagSquared {}


PV_AboveThresh : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, threshold = 0.0;
		^this.multiNew('audio', fftSize, in, threshold)
	}
	*kr { arg fftSize = 1024, in = 0.0, threshold = 0.0;
		^this.multiNew('control', fftSize, in, threshold)
	}
}

PV_BelowThresh : PV_AboveThresh {}
PV_MagClip : PV_AboveThresh {}
PV_LocalMax : PV_AboveThresh {}

PV_BinShift : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, stretch = 1.0, shift = 0.0;
		^this.multiNew('audio', fftSize, in, stretch, shift)
	}
	*kr { arg fftSize = 1024, in = 0.0, stretch = 1.0, shift = 0.0;
		^this.multiNew('control', fftSize, in, stretch, shift)
	}
}

PV_MagShift : PV_BinShift {}


PV_BrickWall : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, wipe = 0.0;
		^this.multiNew('audio', fftSize, in, wipe)
	}
	*kr { arg fftSize = 1024, in = 0.0, wipe = 0.0;
		^this.multiNew('control', fftSize, in, wipe)
	}
}

PV_MagOnePole : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, feedback = 0.0;
		^this.multiNew('audio', fftSize, in, feedback)
	}
	*kr { arg fftSize = 1024, in = 0.0, feedback = 0.0;
		^this.multiNew('control', fftSize, in, feedback)
	}
}

PV_MagAllTimeAverage : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0;
		^this.multiNew('audio', fftSize, in)
	}
	*kr { arg fftSize = 1024, in = 0.0;
		^this.multiNew('control', fftSize, in)
	}
}

PV_MagPeakDecay : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, decay = 0.0;
		^this.multiNew('audio', fftSize, in, decay)
	}
	*kr { arg fftSize = 1024, in = 0.0, decay = 0.0;
		^this.multiNew('control', fftSize, in, decay)
	}
}

PV_MagSmear : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, bins = 0.0;
		^this.multiNew('audio', fftSize, in, bins)
	}
	*kr { arg fftSize = 1024, in = 0.0, bins = 0.0;
		^this.multiNew('control', fftSize, in, bins)
	}
}

PV_TimeSmear : PV_MagSmear {}

PV_RandComb : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, wipe = 0.0, trig = 0.0;
		^this.multiNew('audio', fftSize, in, wipe, trig)
	}
	*kr { arg fftSize = 1024, in = 0.0, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', fftSize, in, wipe, trig)
	}
}

PV_MagFreeze : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, freeze = 0.0;
		^this.multiNew('audio', fftSize, in, freeze)
	}
	*kr { arg fftSize = 1024, in = 0.0, freeze = 0.0;
		^this.multiNew('control', fftSize, in, freeze)
	}
}

PV_LoBitEncoder : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, levels = 8.0;
		^this.multiNew('audio', fftSize, in, levels)
	}
	*kr { arg fftSize = 1024, in = 0.0, levels = 8.0;
		^this.multiNew('control', fftSize, in, levels)
	}
}

PV_BinScramble : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, wipe = 0.0, width = 0.2, trig = 0.0;
		^this.multiNew('audio', fftSize, in, wipe, width, trig)
	}
	*kr { arg fftSize = 1024, in = 0.0, wipe = 0.0, width = 0.2, trig = 0.0;
		^this.multiNew('control', fftSize, in, wipe, width, trig)
	}
}

PV_Diffuser : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, trig = 0.0;
		^this.multiNew('audio', fftSize, in, trig)
	}
	*kr { arg fftSize = 1024, in = 0.0, trig = 0.0;
		^this.multiNew('control', fftSize, in, trig)
	}
}

PV_MagMultiply : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, inA = 0.0, inB = 0.0;
		^this.multiNew('audio', fftSize, inA, inB)
	}
	*kr { arg fftSize = 1024, inA = 0.0, inB = 0.0;
		^this.multiNew('control', fftSize, inA, inB)
	}
}

PV_PhasePaste : PV_MagMultiply {}
PV_Max : PV_MagMultiply {}
PV_Min : PV_MagMultiply {}

PV_BinWipe : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0;
		^this.multiNew('audio', fftSize, inA, inB, wipe)
	}
	*kr { arg fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0;
		^this.multiNew('control', fftSize, inA, inB, wipe)
	}
}

PV_RandWipe : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0, trig = 0.0;
		^this.multiNew('audio', fftSize, inA, inB, wipe, trig)
	}
	*kr { arg fftSize = 1024, inA = 0.0, inB = 0.0, wipe = 0.0, trig = 0.0;
		^this.multiNew('control', fftSize, inA, inB, wipe, trig)
	}
}


PV_RectComb : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, in = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('audio', fftSize, in, numTeeth, phase, width)
	}
	*kr { arg fftSize = 1024, in = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', fftSize, in, numTeeth, phase, width)
	}
}


PV_RectComb2 : PhaseVocoderUGen {
	*ar { arg fftSize = 1024, inA = 0.0, inB = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('audio', fftSize, inA, inB, numTeeth, phase, width)
	}
	*kr { arg fftSize = 1024, inA = 0.0, inB = 0.0, numTeeth = 0.0, phase = 0.0, width = 0.5;
		^this.multiNew('control', fftSize, inA, inB, numTeeth, phase, width)
	}
}


