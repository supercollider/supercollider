

+ Object {
	
	
	
	stopToBundle {}
	freeHeavyResources {}
	freeToBundle {}
	
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

+ Node {
	freeToBundle { arg bundle;
		bundle.add(this.freeMsg)
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

+ Pattern {
	rate { ^\stream }
}

+ Function {
	rate { ^\stream }
}

+ Spec {
	canAccept { ^false } // canAcceptDrag or other uses
}

+ ControlSpec {
	canAccept { arg thing;
		var ts;
		^if(thing.isNumber,{
			thing.inclusivelyBetween(clipLo,clipHi)
		},{
			if(thing.isKindOf(AbstractPlayer),{
				(thing.spec.class == this.class and: {
					ts = thing.spec;
					// thing does not exceed my bounds
					//ts.clipHi <= clipHi and: {ts.clipLo >= clipLo}
					// need more detail
					true
				})
			},{
				false
			})
		});
	}
}


