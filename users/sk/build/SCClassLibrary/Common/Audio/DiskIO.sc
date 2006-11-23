
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
	*ar { arg numChannels, bufnum;
		^this.multiNew('audio', numChannels, bufnum)
	}
	init { arg numChannels, bufnum;
		inputs = [bufnum];
		^this.initOutputs(numChannels, rate)
	}
}

