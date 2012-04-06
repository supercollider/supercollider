Filter : PureUGen {
 	checkInputs { ^this.checkSameRateAsFirstInput }
}

Resonz : Filter {

	*ar { arg in = 0.0, freq = 440.0, bwr = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, bwr).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, bwr = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, bwr).madd(mul, add)
	}
}

OnePole : Filter {

	*ar { arg in = 0.0, coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, coef).madd(mul, add)
	}
	*kr { arg in = 0.0, coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, coef).madd(mul, add)
	}
}

OneZero : OnePole {}

TwoPole : Filter {

	*ar { arg in = 0.0, freq = 440.0, radius = 0.8, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, radius).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, radius = 0.8, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, radius).madd(mul, add)
	}
}

TwoZero : TwoPole {}
APF : TwoPole {}

Integrator : Filter {

	*ar { arg in = 0.0, coef = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, coef).madd(mul, add)
	}
	*kr { arg in = 0.0, coef = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, coef).madd(mul, add)
	}
}

Decay : Filter {

	*ar { arg in = 0.0, decayTime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, decayTime).madd(mul, add)
	}
	*kr { arg in = 0.0, decayTime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, decayTime).madd(mul, add)
	}
}

Decay2 : Filter {

	*ar { arg in = 0.0, attackTime = 0.01, decayTime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, attackTime, decayTime).madd(mul, add)
	}
	*kr { arg in = 0.0, attackTime = 0.01, decayTime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, attackTime, decayTime).madd(mul, add)
	}
}

Lag : Filter {

	*ar { arg in = 0.0, lagTime = 0.1, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, lagTime).madd(mul, add)
	}
	*kr { arg in = 0.0, lagTime = 0.1, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, lagTime).madd(mul, add)
	}
}

Lag2 : Lag {}
Lag3 : Lag {}
Ramp : Lag {}

/// added by nescivi - 15 may 2007
LagUD : Filter {

	*ar { arg in = 0.0, lagTimeU = 0.1, lagTimeD = 0.1,  mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, lagTimeU, lagTimeD).madd(mul, add)
	}
	*kr { arg in = 0.0, lagTimeU = 0.1, lagTimeD = 0.1, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, lagTimeU, lagTimeD).madd(mul, add)
	}
}

Lag2UD : LagUD {}
Lag3UD : LagUD {}

VarLag : Filter {
	*ar { arg in = 0.0, time = 0.1, curvature = 0, warp = 5, start, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, time, curvature, warp, start).madd(mul, add);
	}
	*kr { arg in = 0.0, time = 0.1, curvature = 0, warp = 5, start, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, time, curvature, warp, start).madd(mul, add);
	}
	// FIXME: Implement 'curve' input on VLag ugen instead of using EnvGen.
	// Then \exp warp should probably behave as Lag ugen.
	*new1 { arg rate, in, time, curvature, warp, start;
		var e, curve, trig, sel = if(rate==\audio,\ar,\kr);
		start = start ? in;
		curve = Env.shapeNames[warp] ? warp;
		^if(curve != 1) {
			e = Env([start, in], [time], warp).asArray;
			e[6] = curve;
			e[7] = curvature;
			trig = Changed.perform(sel, in) + Impulse.perform(sel, 0);
			if(time.rate != \scalar) { trig = trig + Changed.kr(time) };
			EnvGen.perform(sel, e, trig);
		} {
			^super.new.rate_(rate).addToSynth.init(in, time, start);
		}
	}
}

LeakDC : Filter {

	*ar { arg in = 0.0, coef = 0.995, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, coef).madd(mul, add)
	}
	*kr { arg in = 0.0, coef = 0.9, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, coef).madd(mul, add)
	}
}


RLPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, rq).madd(mul, add)
	}
}

RHPF : RLPF {}


LPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq).madd(mul, add)
	}
}

HPF : LPF {}

BPF : Filter {

	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, rq).madd(mul, add)
	}
}

BRF : BPF {}

MidEQ : Filter {

	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, rq, db).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, rq, db).madd(mul, add)
	}
}

LPZ1 : Filter {

	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
}

HPZ1 : LPZ1 {}

Slope : Filter {

	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
}

Changed : Filter {
		*kr { arg input, threshold = 0;
			^HPZ1.kr(input).abs > threshold
		}
		*ar { arg input, threshold = 0;
			^HPZ1.ar(input).abs > threshold
		}
}

LPZ2 : Filter {

	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
}

HPZ2 : LPZ2 {}

BPZ2 : LPZ2 {}

BRZ2 : LPZ2 {}

Median : Filter {
	*ar { arg length=3, in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', length, in).madd(mul, add)
	}
	*kr { arg length=3, in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', length, in).madd(mul, add)
	}
	checkInputs {
 		if (rate == 'audio', {
 			if (inputs.at(1).rate != 'audio', {
 				^"input was not audio rate";
 			});
 		});
 		^this.checkValidInputs
 	}

}

//exception in GrafDef_Load: UGen 'AvgAbsAmp' not installed.
//AvgAbsAmp : Filter {
//
//	*ar { arg in = 0.0, coef = 0.999, mul = 1.0, add = 0.0;
//		^this.multiNew('audio', in, coef).madd(mul, add)
//	}
//	*kr { arg in = 0.0, coef = 0.999, mul = 1.0, add = 0.0;
//		^this.multiNew('control', in, coef).madd(mul, add)
//	}
//}

Slew : Filter {
	*ar { arg in = 0.0, up = 1.0, dn = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, up, dn).madd(mul, add)
	}
	*kr { arg in = 0.0, up = 1.0, dn = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, up, dn).madd(mul, add)
	}
}

// not installed
//RLPF4 : Filter {
//
//	*ar { arg in = 0.0, freq = 0.5, res = 0.5, mul = 1.0, add = 0.0;
//		^this.multiNew('audio', in, freq, res).madd(mul, add)
//	}
//}

FOS : Filter {
	*ar { arg in = 0.0, a0 = 0.0, a1 = 0.0, b1 = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, a0, a1, b1).madd(mul, add)
	}
	*kr { arg in = 0.0, a0 = 0.0, a1 = 0.0, b1 = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, a0, a1, b1).madd(mul, add)
	}
}

SOS : Filter {
	*ar { arg in = 0.0, a0 = 0.0, a1 = 0.0, a2 = 0.0, b1 = 0.0, b2 = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, a0, a1, a2, b1, b2).madd(mul, add)
	}
	*kr { arg in = 0.0, a0 = 0.0, a1 = 0.0, a2 = 0.0, b1 = 0.0, b2 = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, a0, a1, a2, b1, b2).madd(mul, add)
	}
}


Ringz : Filter {

	*ar { arg in = 0.0, freq = 440.0, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, decaytime).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, decaytime).madd(mul, add)
	}
}

Formlet : Filter {

	*ar { arg in = 0.0, freq = 440.0, attacktime = 1.0, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, freq, attacktime, decaytime).madd(mul, add)
	}
	*kr { arg in = 0.0, freq = 440.0, attacktime = 1.0, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, freq, attacktime, decaytime).madd(mul, add)
	}
}

// the doneAction arg lets you cause the EnvGen to stop or end the
// synth without having to use a PauseSelfWhenDone or FreeSelfWhenDone ugen.
// It is more efficient to use a doneAction.
// doneAction = 0   do nothing when the envelope has ended.
// doneAction = 1   pause the synth running, it is still resident.
// doneAction = 2   remove the synth and deallocate it.
// doneAction = 3   remove and deallocate both this synth and the preceeding node.
// doneAction = 4   remove and deallocate both this synth and the following node.
// doneAction = 5   remove and deallocate this synth and free all children in the preceeding group (if it is a group).
// doneAction = 6   remove and deallocate this synth and free all children in the following group (if it is a group).

DetectSilence : Filter {
	optimizeGraph {
	}

	*ar { arg in = 0.0, amp = 0.0001, time = 0.1, doneAction = 0;
		^this.multiNew('audio', in, amp, time, doneAction)
		//		^0.0		// DetectSilence has no output
	}
	*kr { arg in = 0.0, amp = 0.0001, time = 0.1, doneAction = 0;
		^this.multiNew('control', in, amp, time, doneAction)
		//		^0.0		// DetectSilence has no output
	}
}

//exception in GrafDef_Load: UGen 'FlagNaN' not installed.
//FlagNaN : Filter {
//
//	*ar { arg in = 0.0;
//		^this.multiNew('audio', in)
//	}
//	*kr { arg in = 0.0;
//		^this.multiNew('control', in)
//	}
//}
