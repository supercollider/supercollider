
DiskOut : UGen {
	*ar { arg bufnum, channelsArray;
		this.multiNewList(['audio', bufnum] ++ channelsArray.asArray)
		^0.0		// DiskOut has no output
	}
	numOutputs { ^0 }
	writeOutputSpecs {}
 	checkInputs {
 		if (rate == 'audio', {
 			for(1, inputs.size - 1, { arg i;
 				if (inputs.at(i).rate != 'audio', {
 					^("input was not audio rate: " + inputs.at(i));
 				});
 			});
 		});
 		^this.checkValidInputs
 	}
}

DiskIn : MultiOutUGen {
	*ar { arg numChannels, bufnum, loop = 0;
		^this.multiNew('audio', numChannels, bufnum, loop)
	}
	init { arg numChannels, bufnum, loop = 0;
		inputs = [bufnum, loop];
		^this.initOutputs(numChannels, rate)
	}
}

VDiskIn : MultiOutUGen {
	*ar { arg numChannels, bufnum, rate = 1, loop = 0, doneAction = 0, sendID = 0;
		^this.multiNew('audio', numChannels, bufnum, rate, loop, doneAction, sendID)
	}
	init { arg numChannels, bufnum, argRate = 1, loop = 0, doneAction = 0, sendID = 0;
		inputs = [bufnum, argRate, loop, doneAction, sendID];
		^this.initOutputs(numChannels, rate)
	}
}

