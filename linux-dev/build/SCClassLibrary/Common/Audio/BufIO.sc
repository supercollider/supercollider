/*
	PlayBuf - sample player
*/

PlayBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, rate=1.0, trigger=1.0, startPos=0.0, loop = 0.0;
		^this.multiNew('audio', numChannels, bufnum, rate, trigger, startPos, loop)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}


/*
// exception in GrafDef_Load: UGen 'SimpleLoopBuf' not installed.

SimpleLoopBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, loopStart=0.0, loopEnd=99999.0, trigger=0.0;
		^this.multiNew('audio', numChannels, bufnum, loopStart, loopEnd, trigger)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}
*/

BufRd : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, phase=0.0, loop=1.0, interpolation=2;
		^this.multiNew('audio', numChannels, bufnum, phase, loop, interpolation)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}

BufWr : UGen {	
	*ar { arg inputArray, bufnum=0, phase=0.0, loop=1.0;
		this.multiNewList(['audio', bufnum, phase, 
			loop] ++ inputArray.asArray)
		^inputArray
	}
}


RecordBuf : UGen {	
	*ar { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, run=1.0, loop=1.0, trigger=1.0;
		this.multiNewList(['audio', bufnum, offset, recLevel, preLevel, run, loop, trigger ] ++ inputArray.asArray);
		^inputArray
	}
}


