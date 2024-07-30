Delay1 : Filter {

    *ar { arg in = 0.0, mul = 1.0, add = 0.0, x1 = 0.0;
        ^this.multiNew('audio', in, x1).madd(mul, add)
    }

    // Unlike *ar, x1 defaults to the first sample of the input value
    *kr { arg in = 0.0, mul = 1.0, add = 0.0, x1 = (in);
        ^this.multiNew('control', in, x1).madd(mul, add)
    }
}

Delay2 : Filter {

    *ar { arg in = 0.0, mul = 1.0, add = 0.0, x1 = 0.0, x2 = 0.0;
        ^this.multiNew('audio', in, x1, x2).madd(mul, add)
    }

    // Unlike *ar, x1 and x2 default to first sample of the input value
    *kr { arg in = 0.0, mul = 1.0, add = 0.0, x1 = (in), x2 = (in);
        ^this.multiNew('control', in, x1, x2).madd(mul, add)
    }
}

///////////////////////////////////////

// these delays use real time allocated memory.

DelayN : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*ar { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in.asAudioRateInput, maxdelaytime, delaytime).madd(mul, add)
	}
	*kr { arg in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('control', in, maxdelaytime, delaytime).madd(mul, add)
	}
}

DelayL : DelayN { }
DelayC : DelayN { }


CombN : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

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
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('audio', buf, in.asAudioRateInput(this), delaytime).madd(mul, add)
	}
	*kr { arg buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;
		^this.multiNew('control', buf, in, delaytime).madd(mul, add)
	}
}

BufDelayL : BufDelayN { }
BufDelayC : BufDelayN { }


BufCombN : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg buf = 0, in = 0.0, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', buf, in.asAudioRateInput(this), delaytime, decaytime).madd(mul, add)
	}
}

BufCombL : BufCombN { }
BufCombC : BufCombN { }

BufAllpassN : BufCombN { }
BufAllpassL : BufCombN { }
BufAllpassC : BufCombN { }

DelTapWr : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*ar { arg buffer, in;
		^this.multiNew('audio', buffer, in.asAudioRateInput(this))
	}

	*kr { arg buffer, in;
		^this.multiNew('control', buffer, in)
	}
}

DelTapRd : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('audio', buffer, phase, delTime, interp).madd(mul, add)
	}

	*kr { arg buffer, phase, delTime, interp = 1, mul = 1, add = 0;
		^this.multiNew('control', buffer, phase, delTime, interp).madd(mul, add)
	}
}
