
// InterplEnvs are a fixed duration
// Envelope specification for an IEnvGen, InterplEnv is not a UGen itself

InterplEnv {

	*new { arg levels=#[0,1,0], times=#[1,1], curve='lin', offset = 0.0;
		this.deprecated(thisMethod, Env.class.findMethod(\new));
		^Env.new(levels, times, curve, nil, nil, offset)
	}

}

// InterplXYC([0, 0, \lin], [1, 2, \sin], [2, 0])
// at time 0, value 0, lin to time 1, value 2, sin to time 2, value 0

InterplXYC {

	*new { arg ... xyc;
		this.deprecated(thisMethod, Env.class.findMethod(\xyc));
		^Env.xyc(xyc)
	}
}

InterplPairs {

	*new { arg pairs, curve;
		this.deprecated(thisMethod, Env.class.findMethod(\pairs));
		^Env.pairs(pairs, curve)
	}
}

InterplChord {

	*new { arg pairs;
		this.deprecated(thisMethod, Env.class.findMethod(\pairs));
		^Env.pairs(pairs)
	}

}
