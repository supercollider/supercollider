
HasSubject : AbstractPlayer {
	
	var <>subject;

	*new { arg subject=0.0;
		^super.new.subject_(subject.loadDocument)
	}
	storeArgs { ^[subject] }

	ar { ^subject.ar }
	
	stop { super.stop; subject.stop }
	children { ^this.inputs }
	inputs { ^[subject] }
	
	numChannels { ^subject.tryPerform(\numChannels) ? 1 }
	guiClass { ^HasSubjectGui }
}

AbstractPlayerEffect : HasSubject { // has two groups

	var subjectGroup,effectGroup;
	
	prepareToBundle { arg agroup,bundle;
		readyForPlay = false;
		
		// more of a PatchOut thing, 
		// problems on restarting
		group = agroup.asGroup;
		subjectGroup = Group.basicNew;
		effectGroup = Group.basicNew;
		NodeWatcher.register(subjectGroup);
		NodeWatcher.register(effectGroup);
		bundle.add( subjectGroup.addToTail(group) );
		bundle.add( effectGroup.addAfterMsg(subjectGroup) );

		this.loadDefFileToBundle(bundle,effectGroup.server);

		subject.prepareToBundle(subjectGroup,bundle);

		readyForPlay = true;
	}

	freePatchOut {
		subjectGroup.free;
		effectGroup.free;
		subjectGroup = effectGroup = nil;
		super.freePatchOut;
	}
}

AbstractSinglePlayerEffect : AbstractPlayerEffect {

	var sharedBus;

	makePatchOut { arg parentGroup,private,bus;
		server = group.server;
		this.topMakePatchOut(effectGroup,private,bus);
		// share my bus
		sharedBus = SharedBus.from(patchOut.bus,this);
		patchOut.bus = sharedBus;
		subject.makePatchOut(subjectGroup,private,sharedBus);
	}
	freePatchOut {
		super.freePatchOut;
		if(sharedBus.notNil,{
			sharedBus.releaseBus(this);
			sharedBus = nil;
		})
	}	
}



PlayerAmp : AbstractSinglePlayerEffect {
	
	// spawn the subject
	// spawn after it, multiplying on same bus
	
	var <amp=1.0;
	// change to a multiplier (anything) ?
	*new { arg player,amp=1.0;
		^super.new(player).amp_(amp)
	}
	asSynthDef {
		^SynthDef(this.defName,{ arg i_bus=0,amp=1.0;
			ReplaceOut.ar(i_bus,
					In.ar(i_bus,this.numChannels) * amp
				)
		})
	}

//	spawnToBundle { arg b;
//		this.children;
//		super.spawnToBundle(b);
//	}

	defName { ^this.class.name.asString ++ this.numChannels.asString }
	synthDefArgs { ^[0,patchOut.synthArg,1,amp] }
	amp_ { arg v; 
		amp = v; 
		if(synth.isPlaying,{
			synth.set(\amp,amp)
		})
	}
	value_ { arg v;
		amp = v; 
		if(synth.isPlaying,{
			synth.set(\amp,amp)
		})
	}		
	guiClass { ^PlayerAmpGui }
}


/*
AsStream : HasSubject {  // this is for passing in patterns or streams to Patches

	ar {
		^subject.asStream
	}
	
	next { ^this.value }

	doesNotUnderstand { arg selector ... args;
		^subject.performList(selector,args)
	}

}

*/


