

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

	defaultControl { arg val=0.0; 
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


