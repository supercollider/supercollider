
+ Object {

	stopToBundle {}
	freeToBundle {}

	rate { ^\noncontrol } // my bad. was \scalar, this should have been \noncontrol

	makePatchOut {}
	patchOut { ^ObjectPatchOut(this) }
	connectToPatchIn {}

	prepareToBundle { arg  group,bundle;
		this.makePatchOut
	}
	prepareForPlay { arg group,private,bus;
		var bundle;
		bundle = AbstractPlayer.bundleClass.new;
		group = group.asGroup;
		this.prepareToBundle(group,bundle);
		bundle.send(group.server)
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
+ SequenceableCollection {
	asBufnum {
		var bp;
		bp = ArrayBuffer(this);
		// UGen.buildSynthDef.addSecretObject(bp);
		^bp.bufnumIr
	}
}

+ Bus {
	prepareToBundle { arg group,bundle;
		if(index.isNil,{
			bundle.addMessage(this,\alloc)
		})
	}
	addToSynthDef { arg synthDef, name;
		// my index is not modulatable
		// actual index to be determined at play time
		synthDef.addIr(name,0)
	}
	instrArgFromControl { arg control;
		if(rate == \audio,{
			^In.ar(control,numChannels)
		},{
			^In.kr(control,numChannels)
		})
	}
	synthArg {
		^index
	}
}

+ SynthDef {
	prepareToBundle { arg group,bundle;
		bundle.addPrepare(["/d_recv", this.asBytes]);
	}
}

+ Pattern {
	rate { ^\stream }
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	// no idea until the pattern starts, so
	// give some answer to build the synth def with
	synthArg { ^1 }
	instrArgFromControl { arg control; ^1 }
}
+ Stream {
	rate { ^\stream }
	addToSynthDef {  arg synthDef,name;
		synthDef.addIr(name,this.synthArg);
	}
	// no idea until the pattern starts, so
	// give some answer to build the synth def with
	synthArg { ^1 }
	instrArgFromControl { arg control; ^1 }
}
+ Function { // treat it like a stream or transform into a Patch ?
	rate { ^\stream }
}

+ Spec {
	canAccept { ^false } // for canReceiveDragHandler or other uses
}

+ ControlSpec {
	canAccept { arg thing;
		var thingSpec;
		^if(thing.isNumber,{
			thing.inclusivelyBetween(clipLo,clipHi)
		},{
			thingSpec = thing.tryPerform(\spec);
			if(thingSpec.isNil,{ ^false });
			if(thingSpec.class !== this.class, { ^false });
			if(thingSpec == this,{ ^true });
			// if thingSpec is within my bounds
			// TODO check if my minval is greater than my maxval
			if(thingSpec.minval >= this.minval and:
				{thingSpec.maxval <= this.maxval},{
					^true
			});
			^false
		});
	}
}


