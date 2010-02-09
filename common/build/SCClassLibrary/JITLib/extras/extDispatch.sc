+ Object {
	encodeForOSC {
		^this.asCompileString.asSymbol
	}
	decodeFromOSC {
		^this
	}
}

+ SimpleNumber {
	encodeForOSC {
		^this
	}
}

+ RawArray {
	encodeForOSC {
		^this	// this is a trick. the dispatcher sends the array flat.
				// currently, sc cannot send arrays via OSC
	}
}
+ Symbol {
	decodeFromOSC {
		^this.asString.interpret;
	}
}

+ String {
	encodeForOSC {
		^this.asCompileString.asSymbol // otherwise string is indistinguishable
	}
	decodeFromOSC {
		^this.interpret;
	}
}