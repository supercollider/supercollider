Pluck : UGen {

	*ar { arg in = 0.0, trig = 1.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0,
			coef = 0.5, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, trig, maxdelaytime, delaytime, decaytime, coef).madd(mul, add)}
	}
