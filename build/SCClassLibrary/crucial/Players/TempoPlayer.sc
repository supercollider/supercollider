
// i'm a KrPlayer, TempoBus IS my bus
// my initDefArg is the tempo bus index

TempoPlayer : KrPlayer { //Synthless
	var tempoBus,<tempo;

	*new { arg tempo;
		^super.newCopyArgs(tempo)
	}
	
	prepareForPlay { arg group,bundle;
		if(patchOut.isNil,{
			tempoBus = TempoBus(group.asGroup.server,tempo);
			patchOut = PatchOut.control(this,group,tempoBus);
			// may not have to do anything
			tempoBus.prepareForPlay(group,bundle);
		});
		readyForPlay = true;
	}
	// nothing more to do
	spawnAtTime {}
	spawnToBundle {}
	loadDefFileToBundle {}
	asSynthDef { ("shouldn't call this" + thisMethod).die; }
}




