// Writes to disk
DiskOut : UGen {
	resourceManagers { ^[UGenDiskResourceManager] }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

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

// Plays from disk
DiskIn : MultiOutUGen {
	resourceManagers { ^[UGenDiskResourceManager] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg numChannels, bufnum, loop = 0;
		^this.multiNew('audio', numChannels, bufnum, loop)
	}
	init { arg numChannels, bufnum, loop = 0;
		inputs = [bufnum, loop];
		^this.initOutputs(numChannels, rate)
	}
}

VDiskIn : MultiOutUGen {
	resourceManagers { ^[UGenDiskResourceManager] }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*ar { arg numChannels, bufnum, rate = 1, loop = 0, sendID = 0;
		^this.multiNew('audio', numChannels, bufnum, rate, loop, sendID)
	}
	init { arg numChannels, bufnum, argRate = 1, loop = 0, sendID = 0;
		inputs = [bufnum, argRate, loop, sendID];
		^this.initOutputs(numChannels, rate)
	}
}
