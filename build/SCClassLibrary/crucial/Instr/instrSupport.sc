

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


/** addToDefName **/



/** building the Instr def name **/
+ SynthDef {
	longName { ^name }
}

+ Editor {
	addToDefName { arg stream;
		^value.addToDefName(stream)
	}
}
+ KrNumberEditor {
	addToDefName { arg stream;
		^1
	}
}
+ IrNumberEditor {
	addToDefName { arg stream;
		^0
	}
}
+ SimpleNumber {
	addToDefName { arg stream;
		stream << this.asFileSafeString;
		^2
	}
}
+ Sample {
	addToDefName { arg stream;
		// beatsizek
		var sum;
		sum = numChannels - 1; // assumes no quad samples
		if(beatsizek.notNil,{ sum = sum + 2 });
		stream << sum;
		^2
	}
}
+ AbstractPlayer {
	addToDefName {
		^0
	}
}

// the big one, espec. Env
+ Object {
	addToDefName { arg stream;
		stream << this.asCompileString.hash.asFileSafeString;
		^2
	}
}




