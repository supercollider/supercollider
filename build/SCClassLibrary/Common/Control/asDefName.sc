
+ Object {

	asSynthDef {
		error("Cannot convert this object to a SynthDef");
		this.dump;
		^nil
	}
	asDefName { 
		^this.asSynthDef.name
	}

}
	
+ String {

	asDefName { ^this }
	
}

+ Function {
	
	asSynthDef {
		^SynthDef("def" ++ this.identityHash.asString,this)
	}
	asDefName {
		var def;
		def = this.asSynthDef;
		def.writeDefFile;
		// load to where ?
		^def.name
	}
	
}

+ SynthDef {

	asSynthDef { ^this }
	asDefName {	^name	}
	
}

