

+ Object { // string, symbol, array
	asInstr {
		^Instr.at(this)
	}
}

+ Instr {
	asInstr {}
}

+ Class {

	asInstr {
		^UGenInstr(this)	
	}
	
}

+ Function {
	asInstr {
		^Instr("f" ++ this.hash,this)
	}
}
