
DiskOut : UGen {
	*ar { arg channelsArray;
		^this.multiNewList('audio', channelsArray)
	}
}

DiskIn : MultiOutUGen {
	*ar { arg numChannels;
		^this.multiNew('audio', numChannels)
	}
	init { arg numChannels;
		inputs = [];
		^this.initOutputs(numChannels, rate)
	}
}

