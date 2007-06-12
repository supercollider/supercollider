
// i'm a KrPlayer, TempoBus IS my bus
// my initDefArg is the tempo bus index

TempoPlayer : KrPlayer { //Synthless
	var tempoBus,<>tempo;

	*new { arg tempo;
		^super.new.tempo_(tempo ?? {Tempo.default})
	}
	
	 /*add a secret arg
	kr {
		^In.kr( tempoBus.index, 1)
	}*/
	
	prepareToBundle { arg group,bundle;
		readyForPlay = true;
		tempoBus.prepareToBundle(group,bundle);	
	}
	makePatchOut { arg group;
		group = group.asGroup;
		tempoBus = TempoBus(group.server,tempo);
		patchOut = PatchOut.control(this,group,tempoBus);
		// may not have to do anything
	}

	// nothing more to do
	spawnAtTime {}
	spawnToBundle {}
	loadDefFileToBundle {}
	asSynthDef { ("shouldn't call this" + thisMethod).die; }
	
}




