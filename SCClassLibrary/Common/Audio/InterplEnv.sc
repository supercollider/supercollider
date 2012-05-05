
// InterplEnvs are a fixed duration
// Envelope specification for an IEnvGen, InterplEnv is not a UGen itself

InterplEnv : Env {
	
	*new { arg levels=#[0,1,0], times=#[1,1], curve='lin', offset = 0.0;
		"InterplEnv is deprecated, please use Env.new instead.".warn;
		^super.new(levels, times, curve, offset: offset)
	}

}

// InterplXYC([0, 0, \lin], [1, 2, \sin], [2, 0])
// at time 0, value 0, lin to time 1, value 2, sin to time 2, value 0

InterplXYC : Env {
	
	*new { arg ... xyc;
		"InterplXYC is deprecated, please use Env.xyc instead.".warn;
		^super.xyc(xyc)
	}
}

InterplPairs : Env {
	
	*new { arg pairs, curve;
		"InterplPairs is deprecated, please use Env.pairs instead.".warn;
		^super.pairs(pairs, curve)
	}
}

InterplChord : Env {
	
	*new { arg pairs;
		"InterplChord is deprecated, please use Env.pairs instead.".warn;
		^super.pairs(pairs)
	}

}
