
+ Instr {
	asInstr {}
}

+ Object { // symbol, array
	asInstr {
		^Instr.at(this)
	}
}

//+ String {
//	asInstr {
//		^Instr.at(this.asSymbol)
//	}
//}

+ Class { // eg. SinOsc the class becomes a UGenInstr

	asInstr {
		^UGenInstr(this)	
	}
}

+ Function {
	asInstr {
		^Instr("f" ++ this.hash,this)
	}
}
