

+ Object {
	
	rate { ^\scalar }
	
	//play { ^ScalarPatchOut(this) }

	makePatchOut {}
	childrenMakePatchOut {}
	patchOut { ^ScalarPatchOut(this) }
	connectToPatchIn {}
	isPlaying { ^false }

	prepareToBundle {  }
	prepareForPlay {	arg group;
		var bundle;
		bundle = CXBundle.new;
		group = group.asGroup;
		this.prepareToBundle(group,bundle);
		^bundle.clumpedSendNow(group.server)
	}
	spawnToBundle {}
	spawnOnToBundle {}
	loadDefFileToBundle {}
	//writeDefFile {}
	
	stop {}
	free {}
	freeHeavyResources {}
	//didSpawn {}

	addToSynthDef {  arg synthDef,name;
		synthDef.addInstrOnlyArg(name,this.synthArg); // has to be an InstrSynthDef
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


