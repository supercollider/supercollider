/*
	PlayBuf - sample player
*/

PlayBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, rate=1.0, trigger=0.0, startPos=0.0, loop = 0.0;
		^this.multiNew('audio', numChannels, bufnum, rate, trigger, startPos, loop)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}

SimpleLoopBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, loopStart=0.0, loopEnd=99999.0, trigger=0.0;
		^this.multiNew('audio', numChannels, bufnum, loopStart, loopEnd, trigger)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}

ReadBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, offset=0.0;
		^this.multiNew('audio', numChannels, bufnum, offset)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
}

RecordBuf : UGen {	
	*ar { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, run=1.0;
		this.multiNewList('audio', inputArray.asArray ++ [ bufnum, offset, recLevel, preLevel, run ]);
		^inputArray
	}
	init { arg ... theInputs;
		inputs = theInputs;
	}
}


