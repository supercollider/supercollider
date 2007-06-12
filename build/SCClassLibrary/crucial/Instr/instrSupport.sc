

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
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
	ar { arg ... args;
		^this.asInstr.ar(*args)
	}
}

+ Class { // eg. SinOsc the class becomes a UGenInstr
	// any class that responds to .ar or .kr
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

+ SynthDef {
	/* 
	// doesnt hang onto its function
	*def { arg name, ugenGraphFunc, rates, prependArgs, variants;
		var new;
		new = this.new(name, ugenGraphFunc, rates, prependArgs, variants);
		Instr.put(new);
		^new
	}
	asInstr {
		^Instr(name, ugenGraphFunc)
	} */
	*at { arg name;
		var search;
		search = Instr.objectAt(name);
		if(search.isNil,{ ^nil });
		^search.asSynthDef
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
//+ SynthDef {
//	longName { ^name }
//}

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




