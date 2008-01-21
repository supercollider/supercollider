
+ SequenceableCollection {
	asMIDIInPortUID {
		^this.collect(_.asMIDIInPortUID)
	}
}

+ SimpleNumber {
	asMIDIInPortUID {
		// a low number is assumed to be an index to the desired port
		if(this < MIDIClient.sources.size,{
			^MIDIClient.sources[this].uid
		});
		// a high number is a UID
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

