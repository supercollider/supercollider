Delay1 : PureUGen {

	*ar { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in).madd(mul, add)
	}
	*kr { arg in = 0.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in).madd(mul, add)
	}
}

Delay2 : Delay1 { }

///////////////////////////////////////

// these delays use real time allocated memory.

DelayN : PureUGen {

	*ar { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in.asAudioRateInput, maxdelaytime, delaytime).madd(mul, add)
	}
	*kr { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, maxdelaytime, delaytime).madd(mul, add)
	}
}

DelayL : DelayN { }
DelayC : DelayN { }


CombN : PureUGen {

	*ar { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in.asAudioRateInput(this), maxdelaytime, delaytime, decaytime).madd(mul, add)
	}
	*kr { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, maxdelaytime, delaytime, decaytime).madd(mul, add)
	}
}

CombL : CombN { }
CombC : CombN { }

AllpassN : CombN { }
AllpassL : CombN { }
AllpassC : CombN { }

///////////////////////////////////////

// these delays use shared buffers.

BufDelayN : UGen {

	*ar { arg buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('audio', buf, in.asAudioRateInput, delaytime).madd(mul, add)
	}
	*kr { arg buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('control', buf, in, delaytime).madd(mul, add)
	}
}

BufDelayL : BufDelayN { }
BufDelayC : BufDelayN { }


BufCombN : UGen {

	*ar { arg buf = 0, in = 0.0, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', buf, in.asAudioRateInput(this), delaytime, decaytime).madd(mul, add)
	}
}

BufCombL : BufCombN { }
BufCombC : BufCombN { }

BufAllpassN : BufCombN { }
BufAllpassL : BufCombN { }
BufAllpassC : BufCombN { }

///////////////////////////////////////

/*
GrainTap : MultiOutUGen {
	*ar { arg grainDur = 0.2, pchRatio = 1.0,
			pchDispersion = 0.0, timeDispersion = 0.0, overlap = 2.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', grainDur, pchRatio,
			pchDispersion, timeDispersion, overlap).madd(mul, add)
	}
}
*/

///////////////////////////////////////
DelTapWr : UGen {
	*ar {arg buffer, in;
		^this.multiNew('audio', buffer, in)
	}

	*kr {arg buffer, in;
		^this.multiNew('control', buffer, in)
	}
}

DelTapRd : UGen {
	*ar {arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('audio', buffer, phase, delTime, interp).madd(mul, add)
	}

	*kr {arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('control', buffer, phase, delTime, interp).madd(mul, add)
	}
}
