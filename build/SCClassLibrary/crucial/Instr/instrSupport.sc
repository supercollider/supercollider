

+ Instr {
	asInstr {}
}

+ SequenceableCollection {
	asInstr {
		^Instr.at(this)
	}
}

+ Symbol {
	asInstr {
		^Instr.at(this)
	}
}

+ String {
	asInstr {
		^Instr.at(this.asSymbol)
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
}





+ Nil {
	asSpec {
		^ControlSpec.new;
	}
}

+ Spec {
	rate { ^\scalar }
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name
		]	
	}
}


+ ControlSpec 	{

	defaultControl { arg val;
		^KrNumberEditor.new(this.constrain(val ? this.default),this); 
	}

	rate { ^\control }
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"minval" -> this.minval,
			"maxval" -> this.maxval,
			"warp" -> this.warp.asPropertyList,
			"step" -> this.step,
			"default" -> this.default
		]
	}
}

+ Warp {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name
		]
	}
}

// CurveWarp

+ AudioSpec {
	
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numChannels" -> this.numChannels
		]	
	}
}


+ MultiTrackAudioSpec {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numTracks" -> this.tracks,
			"numChannels" -> this.numChannels
		]	
	}
}

// EnvSpec



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




