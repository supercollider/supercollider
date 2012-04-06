/*
	Osc - oscillator
	arguments :
		bufnum - an index to a buffer
		freq - frequency in cycles per second
		pm - phase modulation
		mul - multiply by signal or scalar
		add - add to signal or scalar
*/

Osc : PureUGen {
	*ar {
		arg bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', bufnum, freq, phase).madd(mul, add)
	}
	*kr {
		arg bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', bufnum, freq, phase).madd(mul, add)
	}
}

SinOsc : PureUGen {
	*ar {
		arg freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, phase).madd(mul, add)
	}
	*kr {
		arg freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', freq, phase).madd(mul, add)
	}
}

SinOscFB : PureUGen {
	*ar {
		arg freq=440.0, feedback=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, feedback).madd(mul, add)
	}
	*kr {
		arg freq=440.0, feedback=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', freq, feedback).madd(mul, add)
	}
}

OscN : PureUGen {
	*ar {
		arg bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', bufnum, freq, phase).madd(mul, add)
	}
	*kr {
		arg bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', bufnum, freq, phase).madd(mul, add)
	}
}


VOsc : PureUGen {
	*ar {
		arg bufpos, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('audio', bufpos, freq, phase).madd(mul, add)
	}
	*kr {
		arg bufpos, freq=440.0, phase=0.0, mul=1.0, add=0.0;
		^this.multiNew('control', bufpos, freq, phase).madd(mul, add)
	}
}

VOsc3 : PureUGen {
	*ar {
		arg bufpos, freq1=110.0, freq2=220.0, freq3=440.0, mul=1.0, add=0.0;
		^this.multiNew('audio', bufpos, freq1, freq2, freq3).madd(mul, add)
	}
	*kr {
		arg bufpos, freq1=110.0, freq2=220.0, freq3=440.0, mul=1.0, add=0.0;
		^this.multiNew('control', bufpos, freq1, freq2, freq3).madd(mul, add)
	}
}

COsc : PureUGen {
	*ar {
		arg bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;
		^this.multiNew('audio', bufnum, freq, beats).madd(mul, add)
	}
	*kr {
		arg bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;
		^this.multiNew('control', bufnum, freq, beats).madd(mul, add)
	}
}

Formant : PureUGen {
	*ar {
		arg fundfreq = 440.0, formfreq = 1760.0, bwfreq = 880.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', fundfreq, formfreq, bwfreq).madd(mul, add)
	}
}

LFSaw : PureUGen {
	*ar {
		arg freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, iphase).madd(mul, add)
	}
	*kr {
		arg freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, iphase).madd(mul, add)
	}
}

LFPar : LFSaw
{
}

LFCub : LFSaw
{
}

LFTri : LFSaw
{
}

LFGauss : UGen {
	*ar {
		arg duration = 1, width = 0.1, iphase = 0.0, loop = 1, doneAction = 0;
		^this.multiNew('audio', duration, width, iphase, loop, doneAction)
	}
	*kr {
		arg duration = 1, width = 0.1, iphase = 0.0, loop = 1, doneAction = 0;
		^this.multiNew('control', duration, width, iphase, loop, doneAction)
	}
	range { arg min = 0, max = 1;
		^this.linlin(this.minval, 1, min, max)
	}
	minval {
		var width = inputs[1];
		^exp(1.0 / (-2.0 * squared(width)))
	}

}

LFPulse : PureUGen {
	*ar {
		arg freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, iphase, width).madd(mul, add)
	}
	*kr {
		arg freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, iphase, width).madd(mul, add)
	}
	signalRange { ^\unipolar }
}

VarSaw : PureUGen {
	*ar {
		arg freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, iphase, width).madd(mul, add)
	}
	*kr {
		arg freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, iphase, width).madd(mul, add)
	}
}

Impulse : PureUGen {
	*ar {
		arg freq = 440.0, phase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, phase).madd(mul, add)
	}
	*kr {
		arg freq = 440.0, phase = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, phase).madd(mul, add)
	}
	signalRange { ^\unipolar }
}


SyncSaw : PureUGen {
	*ar {
		arg syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', syncFreq, sawFreq).madd(mul, add)
	}
	*kr {
		arg syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', syncFreq, sawFreq).madd(mul, add)
	}
}

//
//TPulse : UGen {//exception in GrafDef_Load: UGen 'TPulse' not installed.
//	*ar {
//		arg trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;
//		^this.multiNew('audio', trig, freq, width).madd(mul, add)
//	}
//	*kr {
//		arg trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;
//		^this.multiNew('control', trig, freq, width).madd(mul, add)
//	}
//	signalRange { ^\unipolar }
//}

Index : PureUGen {
	*ar {
		arg bufnum, in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', bufnum, in).madd(mul, add)
	}
	*kr {
		arg bufnum, in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', bufnum, in).madd(mul, add)
	}
}

WrapIndex : Index {
}

IndexInBetween : Index {
}

DetectIndex : Index {
}

Shaper : Index {
}

IndexL : Index {
}

DegreeToKey : PureUGen {
	*ar {
		arg bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', bufnum, in, octave).madd(mul, add)
	}
	*kr {
		arg bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', bufnum, in, octave).madd(mul, add)
	}
}

Select : PureUGen {
	*ar {
		arg which, array;
		^this.multiNewList(['audio', which] ++ array)
	}
	*kr {
		arg which, array;
		^this.multiNewList(['control', which] ++ array)
	}
 	checkInputs {
 		if (rate == 'audio', {
 			for(1, inputs.size - 1, { arg i;
 				if (inputs.at(i).rate != 'audio', {
 					^("input was not audio rate: " + inputs.at(i));
 				});
 			});
 		});
 		^this.checkValidInputs
 	}
}

SelectX {
	*new1 { arg rate, which, array;
		var selector = UGen.methodSelectorForRate(rate);
		^this.crossfadeClass.perform(selector,
			Select.perform(selector, which.round(2), array),
			Select.perform(selector, which.trunc(2) + 1, array),
			(which * 2 - 1).fold2(1)
		);
	}
	*ar { arg which, array, wrap=1;
		^this.new1(\audio, which, array, wrap);
	}
	*kr { arg which, array, wrap=1;
		^this.new1(\control, which, array, wrap);
	}
	*crossfadeClass {
		^XFade2
	}
}

LinSelectX : SelectX {
	*crossfadeClass {
		^LinXFade2
	}
}

SelectXFocus { 	// does not wrap (yet).
	*new { arg which, array, focus=1;
		^Mix(array.collect({ arg in, i;
			(1 - ((which-i).abs * focus) ).max(0) * in;
		}))
	}
	*ar { arg which, array, focus=1;
		^this.new(which, array, focus);
	}
	*kr { arg which, array, focus=1;
		^this.new(which, array, focus);
	}
}

Vibrato : PureUGen {
	*ar {
		arg freq = 440.0, rate = 6, depth = 0.02, delay = 0.0, onset = 0.0,
				rateVariation = 0.04, depthVariation = 0.1, iphase = 0.0;
		^this.multiNew('audio', freq, rate, depth, delay, onset, rateVariation, depthVariation, iphase)
	}
	*kr {
		arg freq = 440.0, rate = 6, depth = 0.02, delay = 0.0, onset = 0.0,
				rateVariation = 0.04, depthVariation = 0.1, iphase = 0.0;
		^this.multiNew('control', freq, rate, depth, delay, onset, rateVariation, depthVariation, iphase)
	}
}


TChoose {
	*ar { arg trig, array;
		^Select.ar(TIRand.kr(0, array.lastIndex, trig), array)

	}
	*kr { arg trig, array;
		^Select.kr(TIRand.kr(0, array.lastIndex, trig), array)

	}
}


TWChoose {
	*ar { arg trig, array, weights, normalize=0;
		^Select.ar(TWindex.ar(trig, weights, normalize), array)
	}
	*kr { arg trig, array, weights, normalize=0;
		^Select.kr(TWindex.kr(trig, weights, normalize), array)
	}

}
