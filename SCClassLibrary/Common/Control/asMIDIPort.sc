+ SequenceableCollection {
	asMIDIInPortUID {
		^this.collect(_.asMIDIInPortUID)
	}
}

+ SimpleNumber {
	asMIDIInPortUID {
		// a low positive number is assumed to be an index to the desired port
		if(this.isPositive and: { this < MIDIClient.sources.size }, {
			^MIDIClient.sources[this].uid
		});
		// a high-magnitude number is a UID
		^this
	}
}

+ MIDIEndPoint {
	asMIDIInPortUID {
		^this.uid
	}
}

+ Nil {
	asMIDIInPortUID {}
}
