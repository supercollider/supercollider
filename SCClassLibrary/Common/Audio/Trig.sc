Trig1 : UGen {

	*ar { arg in = 0.0, dur = 0.1;
		^this.multiNew('audio', in, dur)
	}
	*kr { arg in = 0.0, dur = 0.1;
		^this.multiNew('control', in, dur)
	}
	signalRange { ^\unipolar }
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
 	checkInputs { ^this.checkSameRateAsFirstInput }
	numOutputs { ^0 }
	writeOutputSpecs {}
}

SendReply : SendTrig {
	*kr { arg trig = 0.0, cmdName = '/reply', values, replyID = -1;
		if(values.containsSeqColl.not) { values = values.bubble };
		[trig, cmdName, values, replyID].flop.do { |args|
			this.new1('control', *args);
		};
		^0.0		// SendReply has no output
	}

	*ar { arg trig = 0.0, cmdName = '/reply', values, replyID = -1;
		if(values.containsSeqColl.not) { values = values.bubble };
		[trig, cmdName, values, replyID].flop.do { |args|
			this.new1('audio', *args);
		};
		^0.0		// SendReply has no output
	}

	*new1 { arg rate, trig = 0.0, cmdName = '/reply', values, replyID = -1;
		var ascii = cmdName.ascii;
		^super.new1(*[rate, trig, replyID, ascii.size].addAll(ascii).addAll(values));
	}
}

TDelay : Trig1 {
 	checkInputs { ^this.checkSameRateAsFirstInput }
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
 	checkInputs { ^this.checkSameRateAsFirstInput }
}

Peak : UGen {
	*ar { arg in = 0.0, trig = 0.0;
		^this.multiNew('audio', in, trig)
	}
	*kr { arg in = 0.0, trig = 0.0;
		^this.multiNew('control', in, trig)
	}
	checkInputs {
		if (rate == 'control' && inputs.at(0).rate == 'audio', {
			^this.checkValidInputs
		}, {
			^this.checkSameRateAsFirstInput
		});
	}
}


RunningMin : Peak {
}

RunningMax : Peak {
}


Stepper : UGen {

	*ar { arg trig=0, reset=0, min=0, max=7, step=1, resetval;
		^this.multiNew('audio', trig, reset, min, max, step, resetval ? min)
	}
	*kr { arg trig=0, reset=0, min=0, max=7, step=1, resetval;
		^this.multiNew('control', trig, reset, min, max, step, resetval ? min)
	}
 	checkInputs { ^this.checkSameRateAsFirstInput }
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
 	checkInputs { ^this.checkSameRateAsFirstInput }
}

Timer : UGen {
	// output is the time between two triggers
	*ar { arg trig = 0.0;
		^this.multiNew('audio', trig)
	}
	*kr { arg trig = 0.0;
		^this.multiNew('control', trig)
	}
 	checkInputs { ^this.checkSameRateAsFirstInput }
}

Sweep : UGen {
	// output sweeps up in value at rate per second
	// the trigger resets to zero
	*ar { arg trig = 0.0, rate = 1.0;
		^this.multiNew('audio', trig, rate)
	}
	*kr { arg trig = 0.0, rate = 1.0;
		^this.multiNew('control', trig, rate)
	}
}

Phasor : UGen {
	*ar { arg trig = 0.0, rate = 1.0, start = 0.0, end = 1.0, resetPos = 0.0;
		^this.multiNew('audio', trig, rate, start, end, resetPos)
	}
	*kr { arg trig = 0.0, rate = 1.0, start = 0.0, end = 1.0, resetPos = 0.0;
		^this.multiNew('control', trig, rate, start, end, resetPos)
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

	*kr { arg in = 0.0, initFreq = 440.0, minFreq = 60.0, maxFreq = 4000.0,
			execFreq = 100.0, maxBinsPerOctave = 16, median = 1,
			ampThreshold = 0.01, peakThreshold = 0.5, downSample = 1, clar=0;
		^this.multiNew('control', in, initFreq, minFreq, maxFreq, execFreq,
			maxBinsPerOctave, median, ampThreshold, peakThreshold, downSample, clar)
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
	*ir { arg in = 0.0, lo = 0.0, hi = 1.0;
		^this.multiNew('scalar', in, lo, hi)
	}
}

InRect : UGen
{
	*ar { arg x = 0.0, y = 0.0, rect;
		^this.multiNew('audio', x, y, rect.left, rect.top,
			rect.right, rect.bottom)
	}
	*kr { arg x = 0.0, y = 0.0, rect;
		^this.multiNew('control', x, y, rect.left, rect.top,
			rect.right, rect.bottom)
	}
}


//Trapezoid : UGen
//{
//	*ar { arg in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;
//		^this.multiNew('audio', in, a, b, c, d)
//	}
//	*kr { arg in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;
//		^this.multiNew('control', in, a, b, c, d)
//	}
//}

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

LastValue : UGen {

	*ar { arg in=0.0, diff=0.01;
		^this.multiNew('audio', in, diff)
	}
	*kr { arg in=0.0, diff=0.01;
		^this.multiNew('control', in, diff)
	}
}

SendPeakRMS : UGen {

	*kr { arg sig, replyRate = 20.0, peakLag = 3, cmdName = '/reply', replyID = -1;
		this.new1('control', sig.asArray, replyRate, peakLag, cmdName, replyID);
		^0.0        // SendPeakRMS has no output
	}

	*ar { arg sig, replyRate = 20.0, peakLag = 3, cmdName = '/reply', replyID = -1;
		this.new1('audio', sig.asArray, replyRate, peakLag, cmdName, replyID);
		^0.0        // SendPeakRMS has no output
	}

	*new1 { arg rate, sig, replyRate, peakLag, cmdName, replyID;
		var ascii = cmdName.ascii;
		var args = [rate, replyRate, peakLag, replyID, sig.size]
			.addAll(sig.flatten)
			.add(ascii.size)
			.addAll(ascii);
		^super.new1(*args);
	}

	numOutputs { ^0 }
	writeOutputSpecs {}
}
