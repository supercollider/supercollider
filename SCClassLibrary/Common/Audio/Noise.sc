/*
	Noise Generators

	WhiteNoise.ar(mul, add)
	BrownNoise.ar(mul, add)
	PinkNoise.ar(mul, add)
	Crackle.ar(chaosParam, mul, add)
	LFNoise0.ar(freq, mul, add)
	LFNoise1.ar(freq, mul, add)
	LFNoise2.ar(freq, mul, add)
	Dust.ar(density, mul, add)
	Dust2.ar(density, mul, add)

	White, Brown, Pink generators have no modulatable parameters
	other than multiply and add inputs.

	The chaos param for ChaosNoise should be from 1.0 to 2.0

*/

RandSeed : WidthFirstUGen {
	*ar { arg trig = 0.0, seed=56789;
		this.multiNew('audio', trig, seed)
		^0.0		// RandSeed has no output
	}
	*kr { arg trig = 0.0, seed=56789;
		this.multiNew('control', trig, seed)
		^0.0		// RandSeed has no output
	}
	*ir { arg trig = 0.0, seed=56789;
		this.multiNew('scalar', trig, seed)
		^0.0		// RandSeed has no output
	}
}

RandID : WidthFirstUGen {
	// choose which random number generator to use for this synth .
	*kr { arg id=0;
		this.multiNew('control', id)
		^0.0		// RandID has no output
	}
	*ir { arg id=0;
		this.multiNew('scalar', id)
		^0.0		// RandID has no output
	}
}


Rand : UGen {
	// uniform distribution
	*new { arg lo = 0.0, hi = 1.0;
		^this.multiNew('scalar', lo, hi)
	}
}

IRand : UGen {
	// uniform distribution of integers
	*new { arg lo = 0, hi = 127;
		^this.multiNew('scalar', lo, hi)
	}
}


TRand : UGen {
	// uniform distribution
	*ar { arg lo = 0.0, hi = 1.0, trig = 0.0;
		^this.multiNew('audio', lo, hi, trig)
	}
	*kr { arg lo = 0.0, hi = 1.0, trig = 0.0;
		^this.multiNew('control', lo, hi, trig)
	}
}

TIRand : UGen {
	// uniform distribution of integers
	*kr { arg lo = 0, hi = 127, trig = 0.0;
		^this.multiNew('control', lo, hi, trig)
	}	*ar { arg lo = 0, hi = 127, trig = 0.0;		^this.multiNew('audio', lo, hi, trig)	}
}



LinRand : UGen {
	// linear distribution
	// if minmax <= 0 then skewed towards lo.
	// else skewed towards hi.
	*new { arg lo = 0.0, hi = 1.0, minmax = 0;
		^this.multiNew('scalar', lo, hi, minmax)
	}
}

NRand : UGen {
	// sum of N uniform distributions.
	// n = 1 : uniform distribution - same as Rand
	// n = 2 : triangular distribution
	// n = 3 : smooth hump
	// as n increases, distribution converges towards gaussian
	*new { arg lo = 0.0, hi = 1.0, n = 0;
		^this.multiNew('scalar', lo, hi, n)
	}
}

ExpRand : UGen {
	// exponential distribution
	*new { arg lo = 0.01, hi = 1.0;
		^this.multiNew('scalar', lo, hi)
	}
}

TExpRand : UGen {
	// uniform distribution
	*ar { arg lo = 0.01, hi = 1.0, trig = 0.0;
		^this.multiNew('audio', lo, hi, trig)
	}
	*kr { arg lo = 0.01, hi = 1.0, trig = 0.0;
		^this.multiNew('control', lo, hi, trig)
	}
}


CoinGate : UGen {
	*ar { arg prob, in;
		^this.multiNew('audio', prob, in)
	}
	*kr { arg prob, in;
		^this.multiNew('control', prob, in)
	}
}

TWindex : UGen {
	*ar {
		arg in, array, normalize=0;
		^this.multiNewList(['audio', in, normalize] ++ array)
	}
	*kr {
		arg in, array, normalize=0;
		^this.multiNewList(['control', in, normalize] ++ array)
	}
}

WhiteNoise : UGen {

	*ar { arg mul = 1.0, add = 0.0;
		// support this idiom from SC2.
		if (mul.isArray, {
			^{ this.multiNew('audio') }.dup(mul.size).madd(mul, add)
		},{
			^this.multiNew('audio').madd(mul, add)
		});
	}
	*kr { arg mul = 1.0, add = 0.0;
		if (mul.isArray, {
			^{ this.multiNew('control') }.dup(mul.size).madd(mul, add)
		},{
			^this.multiNew('control').madd(mul, add)
		});
	}

}

BrownNoise : WhiteNoise {
}

PinkNoise : WhiteNoise {
}

ClipNoise : WhiteNoise {
}

GrayNoise : WhiteNoise {
}


//NoahNoise : WhiteNoise {
//}

Crackle : UGen {

	*ar { arg chaosParam=1.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', chaosParam).madd(mul, add)
	}
	*kr { arg chaosParam=1.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', chaosParam).madd(mul, add)
	}
}

Logistic : UGen {

	*ar { arg chaosParam=3.0, freq = 1000.0, init= 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', chaosParam, freq, init).madd(mul, add)
	}
	*kr { arg chaosParam=3.0, freq = 1000.0, init=0.5, mul = 1.0, add = 0.0;
		^this.multiNew('control', chaosParam, freq, init).madd(mul, add)
	}
}
/* not installed
Rossler : UGen {

	*ar { arg chaosParam=1.5, dt = 0.04, mul = 1.0, add = 0.0;
		^this.multiNew('audio', chaosParam, dt).madd(mul, add)
	}
	*kr { arg chaosParam=1.5, dt = 0.04, mul = 1.0, add = 0.0;
		^this.multiNew('control', chaosParam, dt).madd(mul, add)
	}
}
*/

LFNoise0 : UGen {

	*ar { arg freq=500.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq).madd(mul, add)
	}
	*kr { arg freq=500.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq).madd(mul, add)
	}
}

LFNoise1 : LFNoise0 {
}

LFNoise2 : LFNoise0 {
}

LFClipNoise : LFNoise0 {
}

LFDNoise0 : LFNoise0 {
}

LFDNoise1 : LFNoise0 {
}

LFDNoise3 : LFNoise0 {
}

LFDClipNoise : LFNoise0 {
}

Hasher : UGen {
	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
}

MantissaMask : UGen {
	*ar { arg in = 0.0, bits=3, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, bits).madd(mul, add)
	}
	*kr { arg in = 0.0, bits=3, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, bits).madd(mul, add)
	}
}

Dust : UGen {

	*ar { arg density = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', density).madd(mul, add)
	}
	*kr { arg density = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', density).madd(mul, add)
	}
	signalRange { ^\unipolar }

}

Dust2 : UGen {
	*ar { arg density = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', density).madd(mul, add)
	}
	*kr { arg density = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', density).madd(mul, add)
	}
}
/* not installed
LinCong : UGen {
	var iseed, imul, iadd, imod;

	*ar { arg iseed, imul, iadd, imod, mul = 1.0, add = 0.0;
		^this.multiNew('audio', iseed, imul, iadd, imod).madd(mul, add)
	}
	*kr { arg iseed, imul, iadd, imod, mul = 1.0, add = 0.0;
		^this.multiNew('control', iseed, imul, iadd, imod).madd(mul, add)
	}
	init { arg jseed, jmul, jadd, jmod ... theInputs;
 		inputs = theInputs;
 		iseed = jseed;
 		imul = jmul;
 		iadd = jadd;
 		imod = jmod;
 	}
}
*/
//
//Latoocarfian : UGen {
//
//	*ar { arg a, b, c, d, mul = 1.0, add = 0.0;
//		^this.multiNew('audio', a, b, c, d).madd(mul, add)
//	}
//	*kr { arg a, b, c, d, mul = 1.0, add = 0.0;
//		^this.multiNew('control', a, b, c, d).madd(mul, add)
//	}
//}
