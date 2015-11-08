DiskOut : UGen {
	*ar { arg bufnum, channelsArray;
		^this.multiNewList(['audio', bufnum] ++ channelsArray.asArray)
	}

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
	*ar { arg numChannels, bufnum, rate = 1, loop = 0, sendID = 0;
		^this.multiNew('audio', numChannels, bufnum, rate, loop, sendID)
	}
	init { arg numChannels, bufnum, argRate = 1, loop = 0, sendID = 0;
		inputs = [bufnum, argRate, loop, sendID];
		^this.initOutputs(numChannels, rate)
	}
}
