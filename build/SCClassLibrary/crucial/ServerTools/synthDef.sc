

+ Object {
	
	isPlaying { ^false }
	stop {}
	stopToBundle {}
	free {}

	rate { ^\scalar }
	
	makePatchOut {}
	patchOut { ^ScalarPatchOut(this) }
	connectToPatchIn {}

	prepareToBundle {  }
	prepareForPlay {	arg group,private,bus;
		var bundle;
		bundle = CXBundle.new;
		group = group.asGroup;
		this.prepareToBundle(group,bundle);
		^bundle.clumpedSendNow(group.server)
	}
	spawnToBundle {}
	spawnOnToBundle {}
	loadDefFileToBundle {}
	
	addToSynthDef {  arg instrSynthDef,name;
		instrSynthDef.addInstrOnlyArg(name,this.synthArg);
	}
	
	synthArg { ^this }
	initForSynthDef {}
	instrArgFromControl { arg control;
		^this
	}	
}


+ Buffer {
	synthArg {
		^bufnum
	}
}

+ SynthDef {
	prepareToBundle { arg group,bundle;
		bundle.add(["/d_recv", this.asBytes]);
	}
}


