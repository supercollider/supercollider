PlayBuf : MultiOutUGen {
	resourceManagers {
        ^if(this.hasObservableEffect){
            [UGenBufferResourceManager, UGenDoneResourceManager]
        } {
            [UGenBufferResourceManager]
        }
    }
	bufferAccessType { ^\read }
	hasObservableEffect { ^this.implHasObservableEffectViaDoneAction(6) }

	*ar { arg numChannels, bufnum=0, rate=1.0, trigger=1.0, startPos=0.0, loop = 0.0, doneAction=0;
		^this.multiNew('audio', numChannels, bufnum, rate, trigger, startPos, loop, doneAction)
	}

	*kr { arg numChannels, bufnum=0, rate=1.0, trigger=1.0, startPos=0.0, loop = 0.0, doneAction=0;
		^this.multiNew('control', numChannels, bufnum, rate, trigger, startPos, loop, doneAction)
	}

	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }
}

TGrains : MultiOutUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg numChannels, trigger=0, bufnum=0, rate=1, centerPos=0,
		dur=0.1, pan=0, amp=0.1, interp=4;
		^this.multiNew('audio', numChannels, trigger, bufnum, rate, centerPos,
			dur, pan, amp, interp)
	}
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	argNamesInputsOffset { ^2 }

}

BufRd : MultiOutUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

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
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*ar { arg inputArray, bufnum=0, phase=0.0, loop=1.0;
		^this.multiNewList(['audio', bufnum, phase,
			loop] ++ inputArray.asArray)

	}
	*kr { arg inputArray, bufnum=0, phase=0.0, loop=1.0;
		^this.multiNewList(['control', bufnum, phase,
			loop] ++ inputArray.asArray)
	}
	checkInputs {
		if (rate == 'audio') {
			if(inputs.at(1).rate != 'audio') {
				^"phase input is not audio rate: % %".format(inputs.at(1), inputs.at(1).rate)
			} {
				if(inputs[3..].any { |x| x.rate != 'audio' }) {
					^"inputArray input is not audio rate: % %".format(inputs[3..], inputs[3..].collect(_.rate))
				}
			}
		};
		^this.checkValidInputs
	}
}

RecordBuf : UGen {
	resourceManagers {
        ^if(this.hasObservableEffect){
            [UGenBufferResourceManager, UGenDoneResourceManager]
        } {
            [UGenBufferResourceManager]
        }
    }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*ar { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0,
		run=1.0, loop=1.0, trigger=1.0, doneAction=0;
		^this.multiNewList(
			['audio', bufnum, offset, recLevel, preLevel, run, loop, trigger, doneAction ]
			++ inputArray.asArray
		)
	}
	*kr { arg inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0,
		run=1.0, loop=1.0, trigger=1.0, doneAction=0;
		^this.multiNewList(
			['control', bufnum, offset, recLevel, preLevel, run, loop, trigger, doneAction ]
			++ inputArray.asArray
		)
	}
}

ScopeOut : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg inputArray , bufnum=0;
		this.multiNewList(['audio', bufnum] ++ inputArray.asArray);
		^0.0
	}
	*kr { arg inputArray , bufnum=0;
		this.multiNewList(['control', bufnum] ++ inputArray.asArray);
		^0.0
	}
}

ScopeOut2 : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg inputArray, scopeNum=0, maxFrames = 4096, scopeFrames;
		this.multiNewList(['audio', scopeNum, maxFrames, scopeFrames ? maxFrames] ++ inputArray.asArray);
		^0.0
	}
	*kr { arg inputArray, scopeNum=0, maxFrames = 4096, scopeFrames;
		this.multiNewList(['control', scopeNum, maxFrames, scopeFrames ? maxFrames] ++ inputArray.asArray);
		^0.0
	}
}

Tap : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }

	*ar { arg bufnum = 0, numChannels = 1, delaytime = 0.2;
		var scale = BufRateScale.kr(bufnum);
		var n = delaytime * (SampleRate.ir.neg * scale);
		^PlayBuf.ar(numChannels, bufnum, scale, 0, n, 1);
	}
}

LocalBuf : WidthFirstUGen {
	resourceManagers { ^[] } // This depends on nothing, and is depended by its usage in code.
	hasObservableEffect { ^false } // If you don't use the buffer, it can be deleted.

	*new { arg numFrames = 1, numChannels = 1;
		^this.multiNew('scalar', numChannels, numFrames)
	}

	*new1 { arg rate ... args;
		var maxLocalBufs = UGen.buildSynthDef.maxLocalBufs;
		if(maxLocalBufs.isNil) {
			maxLocalBufs = MaxLocalBufs.new;
			UGen.buildSynthDef.maxLocalBufs = maxLocalBufs;
		};
		maxLocalBufs.increment;
		^super.new.rate_(rate).addToSynth.init( *args ++ maxLocalBufs )
	}

	*newFrom { arg list;
		var shape, buf;
		shape = list.shape;
		if(shape.size == 1) { shape = [1, list.size] };
		if(shape.size > 2) { Error("LocalBuf: list has not the right shape").throw };
		buf = this.new(*shape.reverse);
		buf.set(list.flop.flat);
		^buf
	}

	numFrames { ^inputs[1] }
	numChannels { ^inputs[0] }

	set { arg values, offset = 0;
		SetBuf(this, values.asArray, offset);
	}
	clear {
		ClearBuf(this);
	}

}

MaxLocalBufs : UGen {
	hasObservableEffect { ^false }
	resourceManagers { ^[] }

	*new {
		^this.multiNew('scalar', 0);
	}
	increment {
		inputs[0] = inputs[0] + 1;
	}
}

SetBuf : WidthFirstUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*new { arg buf, values, offset = 0;
		values = values.asArray;
		^this.multiNewList(['scalar', buf, offset, values.size] ++ values)
	}
}

ClearBuf : WidthFirstUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }

	*new { arg buf;
		^this.multiNew('scalar', buf)
	}
}
