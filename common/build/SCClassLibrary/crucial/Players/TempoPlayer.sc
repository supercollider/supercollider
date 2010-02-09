
// i'm a KrPlayer, TempoBus IS my bus
// my initDefArg is the tempo bus index

TempoPlayer : KrPlayer { //Synthless
	var tempoBus,<>tempo;

	*new { arg tempo;
		^super.new.tempo_(tempo ?? {Tempo.default})
	}

	/* a TempoPlayer can secretly add itself to the synth def
		without the need of being passed in with an explicit arg to the Instr */
	kr {
		^InstrSynthDef.buildSynthDef.playerIn(this)
	}

	synthArg { ^tempoBus.index }
	makeResourcesToBundle { arg bundle;
		tempoBus = TempoBus(server,tempo);
		this.annotate(tempoBus,"Tempo Bus");
		tempoBus.prepareToBundle(group,bundle);
	}
	makePatchOut {
		patchOut = PatchOut.control(this,group,tempoBus);
	}
	freeResourcesToBundle { arg bundle;
		bundle.addFunction({
			tempoBus.free;
			tempoBus = nil;
		});
	}
	spawnAtTime {}
	spawnToBundle {}
	loadDefFileToBundle {}
	asSynthDef { ("shouldn't call this" + thisMethod).die; }
	spec { ^\tempo.asSpec }
}

