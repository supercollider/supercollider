
Trig1 : UGen {
	
	*ar { arg in = 0.0, dur = 0.1;
		^this.multiNew('audio', in, dur)
	}
	*kr { arg in = 0.0, dur = 0.1;
		^this.multiNew('control', in, dur)
	}
	
}

Trig : Trig1 {
}


SendTrig : UGen {
	*ar { arg in = 0.0, id = 0, value = 0.0;
		this.multiNew('audio', in, id, value);
		^0.0		// SendTrig has no output
	}
	*kr { arg in = 0.0, id = 0, value = 0.0;
		this.multiNew('control', in, id, value);
		^0.0		// SendTrig has no output
	}
 	checkInputs {
 		if (rate == 'audio', {
 			if (inputs.at(0).rate != 'audio', { ^false });
 		});
 		^true
 	}
	numOutputs { ^0 }
	writeOutputSpecs {}
}

TDelay : Trig1 {
}

Latch : UGen {
	
	*ar { arg in = 0.0, trig = 0.0;
		^this.multiNew('audio', in, trig)
	}
	*kr { arg in = 0.0, trig = 0.0;
		^this.multiNew('control', in, trig)
	}
	
}

Gate : Latch {	
}

PulseCount : UGen {
	
	*ar { arg trig = 0.0, reset = 0.0;
		^this.multiNew('audio', trig, reset)
	}
	*kr { arg trig = 0.0, reset = 0.0;
		^this.multiNew('control', trig, reset)
	}
}

Peak : PulseCount {
}

PulseDivider : UGen {
	
	*ar { arg trig = 0.0, div = 2.0, start = 0.0;
		^this.multiNew('audio', trig, div, start)
	}
	*kr { arg trig = 0.0, div = 2.0, start = 0.0;
		^this.multiNew('control', trig, div, start)
	}
	
}

SetResetFF : PulseCount {
}

ToggleFF : UGen {
	
	*ar { arg trig = 0.0;
		^this.multiNew('audio', trig)
	}
	*kr { arg trig = 0.0;
		^this.multiNew('control', trig)
	}
}


ZeroCrossing : UGen {
	
	*ar { arg in = 0.0;
		^this.multiNew('audio', in)
	}
	*kr { arg in = 0.0;
		^this.multiNew('control', in)
	}
}

PeakFollower : UGen {
	
	*ar { arg in = 0.0, decay = 0.999;
		^this.multiNew('audio', in, decay)
	}
	*kr { arg in = 0.0, decay = 0.999;
		^this.multiNew('control', in, decay)
	}
}

Pitch : MultiOutUGen {
	
	*kr { arg in = 0.0, initFreq = 440.0, minFreq = 60.0, maxFreq = 4000.0, execFreq = 100.0,
		maxBinsPerOctave = 16, median = 1, ampThreshold = 0.01, peakThreshold = 0.5, downSample = 1;
		^this.multiNew('control', in, initFreq, minFreq, maxFreq, execFreq,
				maxBinsPerOctave, median, ampThreshold, peakThreshold, downSample)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

InRange : UGen
{
	*ar { arg in = 0.0, lo = 0.0, hi = 1.0;
		^this.multiNew('audio', in, lo, hi)
	}
	*kr { arg in = 0.0, lo = 0.0, hi = 1.0;
		^this.multiNew('control', in, lo, hi)
	}
}

InRect : UGen
{
	*ar { arg x = 0.0, y = 0.0, rect;
		^this.multiNew('audio', x, y, rect.left, rect.top, rect.right, rect.bottom)
	}
	*kr { arg x = 0.0, y = 0.0, rect;
		^this.multiNew('control', x, y, rect.left, rect.top, rect.right, rect.bottom)
	}
}

Trapezoid : UGen
{
	*ar { arg in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;
		^this.multiNew('audio', in, a, b, c, d)
	}
	*kr { arg in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;
		^this.multiNew('control', in, a, b, c, d)
	}
}

Fold : InRange {}
Clip : InRange {}
Wrap : InRange {}
Schmidt : InRange {}

MostChange : UGen
{
	*ar { arg a = 0.0, b = 0.0;
		^this.multiNew('audio', a, b)
	}
	*kr { arg a = 0.0, b = 0.0;
		^this.multiNew('control', a, b)
	}
}

LeastChange : MostChange {}
