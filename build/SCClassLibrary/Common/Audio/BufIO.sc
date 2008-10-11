/*
	PlayBuf - sample player
*/

PlayBuf : MultiOutUGen {	
	*ar { arg numChannels, bufnum=0, rate=1.0, trigger=1.0, startPos=0.0, loop = 0.0;
		^this.multiNew('audio', numChannels, bufnum, rate, trigger, startPos, loop)
	}

	*kr { arg numChannels, bufnum=0, rate=1.0, trigger=1.0, startPos=0.0, loop = 0.0;
		^this.multiNew('control', numChannels, bufnum, rate, trigger, startPos, loop)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
}

TGrains : MultiOutUGen {
	*ar { arg numChannels, trigger=0, bufnum=0, rate=1, centerPos=0, 
			dur=0.1, pan=0, amp=0.1, interp=4; 
		if (numChannels < 2) {
			 "TGrains needs at least two channels.".error;
			 ^nil
		}
		^this.multiNew('audio', numChannels, trigger, bufnum, rate, centerPos, 
				dur, pan, amp, interp)
	}
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
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
	*kr { arg numChannels, bufnum=0, phase=0.0, loop=1.0, interpolation=2;
		^this.multiNew('control', numChannels, bufnum, phase, loop, interpolation)
	}
	
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
	checkInputs {
 		if (rate == 'audio' and: {inputs.at(1).rate != 'audio'}, { 
 			^("phase input is not audio rate: " + inputs.at(1) + inputs.at(1).rate);
 		});
 		^this.checkValidInputs
 	}
}

BufWr : UGen {	
	*ar { arg inputArray, bufnum=0, phase=0.0, loop=1.0;
		^this.multiNewList(['audio', bufnum, phase, 
			loop] ++ inputArray.asArray)

	}
	*kr { arg inputArray, bufnum=0, phase=0.0, loop=1.0;
		^this.multiNewList(['control', bufnum, phase, 
			loop] ++ inputArray.asArray)
	}
	checkInputs {
 		if (rate == 'audio' and: {inputs.at(1).rate != 'audio'}, { 
 			^("phase input is not audio rate: " + inputs.at(1) + inputs.at(1).rate);
 		});
 		^this.checkValidInputs
 	}
}


RecordBuf : UGen {	
	*ar { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, 
			run=1.0, loop=1.0, trigger=1.0;
		^this.multiNewList(
			['audio', bufnum, offset, recLevel, preLevel, run, loop, trigger ] 
			++ inputArray.asArray
		)
	}
	*kr { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, 
			run=1.0, loop=1.0, trigger=1.0;
		^this.multiNewList(
			['control', bufnum, offset, recLevel, preLevel, run, loop, trigger ] 
			++ inputArray.asArray
		)
	}
}


ScopeOut : UGen {
	*ar { arg inputArray , bufnum=0;
		this.multiNewList(['audio', bufnum] ++ inputArray.asArray);
		^0.0
	}
	*kr { arg inputArray , bufnum=0;
		this.multiNewList(['control', bufnum] ++ inputArray.asArray);
		^0.0
	}
}

Tap : UGen {
	*ar { arg bufnum = 0, numChannels = 1, delaytime = 0.2;
		var n;
		n = delaytime * SampleRate.ir.neg; // this depends on the session sample rate, not buffer.
		^PlayBuf.ar(numChannels, bufnum, 1, 0, n, 1);
	}
}

LocalBuf : UGen {

	*new { arg numFrames = 1, numChannels = 1, maxLocalBufs = 8;
		^this.multiNew('scalar', numChannels, numFrames, maxLocalBufs)
	}
	
	*newFrom { arg list, maxLocalBufs = 8;
		var shape, buf;
		shape = list.shape;
		if(shape.size == 1) { shape = [1, list.size] };
		if(shape.size > 2) { Error("LocalBuf: list has not the right shape").throw };
		buf = this.new(*shape.reverse ++ maxLocalBufs);
		buf.set(list.flop.flat);
		^buf 
	}
	
	set { arg values;
		SetBuf(this, values);
	}
	clear {
		ClearBuf(this);
	}
	
}

SetBuf : UGen {
	*new { arg buf, values;
		^this.multiNewList(['scalar', buf, values.size] ++ values)
	}
}

ClearBuf : UGen {
	*new { arg buf;
		^this.multiNew('scalar', buf)
	}
}
