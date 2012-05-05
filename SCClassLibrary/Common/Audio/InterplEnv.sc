
// InterplEnvs are a fixed duration
// Envelope specification for an IEnvGen, InterplEnv is not a UGen itself

InterplEnv {
	
	*new { arg levels=#[0,1,0], times=#[1,1], curve='lin', offset = 0.0;
		"InterplEnv is deprecated, please use Env.new instead.".warn;
		^Env.new(levels, times, curve, nil, nil, offset)
	}

}

// InterplXYC([0, 0, \lin], [1, 2, \sin], [2, 0])
// at time 0, value 0, lin to time 1, value 2, sin to time 2, value 0

InterplXYC {
	
	*new { arg ... xyc;
		"InterplXYC is deprecated, please use Env.xyc instead.".warn;
		^Env.xyc(xyc)
	}
}

InterplPairs {
	
	*new { arg pairs, curve;
		"InterplPairs is deprecated, please use Env.pairs instead.".warn;
		^Env.pairs(pairs, curve)
	}
}

InterplChord {
	
	*new { arg pairs;
		"InterplChord is deprecated, please use Env.pairs instead.".warn;
		^Env.pairs(pairs)
	}

}
