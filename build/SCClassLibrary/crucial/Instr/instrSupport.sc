

+ Instr {
	asInstr {}
}
+ InterfaceDef {
	asInterfaceDef {}
}
+ SequenceableCollection {
	asInstr {
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
}

+ Symbol {
	asInstr {
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
}

+ String {
	asInstr {
		^Instr.at(this.asSymbol)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
}

+ Class { // eg. SinOsc the class becomes a UGenInstr

	asInstr {
		^UGenInstr(this)	
	}
}

+ Function {
	asInstr {
		^Instr("f" ++ this.hash,this)
	}
	asInterfaceDef {
		^InterfaceDef("f" ++ this.hash,this)
	}
}




/**  asSpec **/

+ Nil {
	asSpec {
		^ControlSpec.new;
	}
}

+ Spec {
	rate { ^\scalar }
}

+ ControlSpec 	{

	defaultControl { arg val;
		^KrNumberEditor.new(this.constrain(val ? this.default),this); 
	}

	rate { ^\control }
}


