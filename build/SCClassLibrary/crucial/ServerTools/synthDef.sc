

+ Object {
	
	//writeDefFile {}
	
	//play { ^ScalarPatchOut(this) }

	patchOut { ^ScalarPatchOut(this) }
	isPlaying { ^false }

	// floats only ?
	readyForPlay { ^true }
	prepareForPlay {  }
	
	spawnToBundle {}
	loadDefFileToBundle {}
	//spawn { ^ScalarPatchOut(this) }
	
	free {}
	didSpawn {}

	addToSynthDef {  arg synthDef,name;
		synthDef.addFixed(name,this.synthArg); // has to be an InstrSynthDef
	}
	
	synthArg { ^this }
	instrArgRate { ^\scalar }
	instrArgFromControl { arg control;
		^this//.insp("Object returns self as instrArg")
	}
	
}

+ Editor {

	prepareForPlay { arg group,bundle;
		if(patchOut.isNil,{ // private out
			patchOut = ScalarPatchOut(this);
		});
	}
	synthArg { ^this.poll }
	instrArgFromControl { arg control;
		^control
	}
}

+ NumberEditor {
		
	addToSynthDef {  arg synthDef,name;
		synthDef.addKr(name,this.synthArg);
	}
	instrArgRate { ^\control }

	didSpawn { arg patchIn,synthi;
		patchOut.connectTo(patchIn,false);

		// am i already connected to this client ?
		if(this.dependants.includes(patchOut.updater).not,{
			patchOut.updater = 
				SimpleController(this)
						.put(\value,{
							patchIn.value = value;
						});
		});
	}
}

+ TempoBus {
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	synthArg { ^this.index }
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		^control
	}
	//didSpawn  set value
}

+ AbstractPlayer {

	addToSynthDef {  arg synthDef,name;
		// value doesn't matter so much, we are going to pass in a real live one
		synthDef.addKr(name,this.synthArg ? 0); // \out is a .kr bus index
	}

	synthArg { ^patchOut.synthArg }
	instrArgRate { ^\audio }
	instrArgFromControl { arg control;
		// a Patch could be either
		^if(this.rate == \audio,{
			In.ar(control,this.numChannels,this)
		},{
			In.kr(control,this.numChannels,this)
		})
	}
}

+ KrPlayer {

	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		^In.kr(control,this.numChannels)
	}
}

+ Buffer {
	synthArg {
		^bufnum
	}
}

+ BufferProxy {

	instrArgFromControl { arg control,argi;
		forArgi = argi; // for buffer etc. to build a control
		^this
	}
}


// trig things, supply an InTrig.kr

