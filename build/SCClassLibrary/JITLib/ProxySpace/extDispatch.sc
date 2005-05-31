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
		^this
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