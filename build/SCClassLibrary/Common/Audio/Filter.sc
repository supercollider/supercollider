/*
	OnePole.ar(in, coef, mul, add) // one pole filter
	OneZero.ar(in, coef, mul, add) // one zero filter
	
	TwoPole.ar(in, freq, radius, mul, add) // two pole filter
	TwoZero.ar(in, freq, radius, mul, add) // two pole filter
	
	Integrator.ar(in, coef, mul, add) // integrator
	
	RLPF.ar(in, freq, q, mul, add) // 2nd order Resonant LowPass Filter
	RHPF.ar(in, freq, q, mul, add) // 2nd order Resonant HighPass Filter

	LPF.ar(in, freq, mul, add) // 2nd order butterworth LowPass Filter
	HPF.ar(in, freq, mul, add) // 2nd order butterworth HighPass Filter
	BPF.ar(in, freq, bw, mul, add) // 2nd order butterworth BandPass Filter
	BRF.ar(in, freq, bw, mul, add) // 2nd order butterworth Band Reject Filter
	
	LPZ1.ar(in, mul, add) // two point sum
	HPZ1.ar(in, mul, add) // two point difference
	Slope.ar(in, mul, add) // instantaneous slew rate
	
	LPZ2.ar(in, mul, add) // special case two zero lowpass filter. impulse resp: 1 2 1
	HPZ2.ar(in, mul, add) // special case two zero highpass filter. impulse resp: 1 -2 1
	BPZ2.ar(in, mul, add) // special case two zero midpass filter. impulse resp: 1 0 1
	BRZ2.ar(in, mul, add) // special case two zero midcut filter. impulse resp: 1 0 -1
	
*/

Filter : UGen {
 	checkInputs {
 		if (rate == 'audio', {
 			if (inputs.at(0).rate != 'audio', { ^false });
 		});
 		^true
 	}
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

//exception in GrafDef_Load: UGen 'ParEQ' not installed.
//ParEQ : Filter {
//	
//	*ar { arg in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;
//		^this.multiNew('audio', in, freq, rq, db).madd(mul, add)
//	}
//	*kr { arg in = 0.0, freq = 440.0, rq = 1.0, db = 0.0, mul = 1.0, add = 0.0;
//		^this.multiNew('control', in, freq, rq, db).madd(mul, add)
//	}
//}

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
 			if (inputs.at(1).rate != 'audio', { ^false });
 		});
 		^true
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
}

SOS : Filter {
	*ar { arg in = 0.0, a0 = 0.0, a1 = 0.0, a2 = 0.0, b1 = 0.0, b2 = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, a0, a1, a2, b1, b2).madd(mul, add)
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

//exception in GrafDef_Load: UGen 'EndThresh' not installed.
//EndThresh : Filter {
//
//	*ar { arg in = 0.0, amp = 0.00001, time = 0.2;
//		^this.multiNew('audio', in, amp, time)
//	}
//	*kr { arg in = 0.0, amp = 0.00001, time = 0.2;
//		^this.multiNew('control', in, amp, time)
//	}
//}

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

