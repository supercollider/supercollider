+ Object {
	asNodeArg {^this}
}
+ Node {
	asNodeArg {^this.nodeID}
}

+ Bus {
	asNodeArg {^this.index}
	
	asMap { // copied from James H.
		if(rate == \control) {
			^("c" ++ index).asSymbol
		} {
			MethodError("Cannot map a synth control to a% %-rate bus."
				.format(if(rate.asString[0].isVowel, "n", ""), rate), this).throw;
		}
	}
}

+ Buffer {
	asNodeArg {^this.bufnum}
}

+ SequenceableCollection {
	asNodeArg {^this.collect(_.asNodeArg)}
}
