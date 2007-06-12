
HasSubject : AbstractPlayer {
	
	var <>subject;

	*new { arg subject=0.0;
		^super.new.subject_(subject.loadDocument)
	}
	storeArgs { ^[subject] }

	ar { ^subject.ar }
	
	children { ^this.inputs }
	inputs { ^[subject] }
	
	numChannels { ^subject.tryPerform(\numChannels) ? 1 }
	guiClass { ^HasSubjectGui }
}

AbstractPlayerEffect : HasSubject { // has two groups

	var subjectGroup,effectGroup;
	
	makePatchOut { arg agroup,private = false,bus,bundle;
		subjectGroup = Group.basicNew;
		effectGroup = Group.basicNew;
		NodeWatcher.register(subjectGroup);
		NodeWatcher.register(effectGroup);
		bundle.add( subjectGroup.addToTail(group) );
		bundle.add( effectGroup.addAfterMsg(subjectGroup) );
		super.makePatchOut(agroup,private,bus,bundle);
	}
	freePatchOut { arg bundle;
		subjectGroup.freeToBundle(bundle);
		effectGroup.freeToBundle(bundle);
		subjectGroup = effectGroup = nil;
		super.freePatchOut(bundle);
	}
}

AbstractSinglePlayerEffect : AbstractPlayerEffect {

	var sharedBus;

	childrenMakePatchOut { arg parentGroup,private=true,bundle;
		sharedBus = SharedBus.newFrom(patchOut.bus,this);
		patchOut.bus = sharedBus;
		subject.makePatchOut(subjectGroup,private,sharedBus,bundle);
	}
	freePatchOut { arg bundle;
		super.freePatchOut(bundle);
		if(sharedBus.notNil,{
			bundle.addFunction({
				sharedBus.releaseBus(this);
				sharedBus = nil;
			})
		})
	}	
}

PlayerAmp : AbstractSinglePlayerEffect {
	
	var <amp=1.0;

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
	defName { ^this.class.name.asString ++ this.numChannels.asString }
	synthDefArgs { ^[\i_bus,patchOut.synthArg,\amp,amp] }

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

EnvelopedPlayer : AbstractSinglePlayerEffect {
	
	var <>env,<>numChannels;
	
	*new { arg player,env,numChannels;
		^super.new(player).env_(env).numChannels_(numChannels)
	}
	storeArgs { ^[subject,env,numChannels] }
	asSynthDef {
		^SynthDef(this.defName,{ arg i_bus,gate;
			var in,pnc;
			pnc = subject.numChannels;
			in = In.ar(i_bus,pnc) * EnvGen.kr(env,gate,doneAction: 2);
			if(numChannels.notNil,{
				in = NumChannels.ar(in,numChannels,true);
			});
			ReplaceOut.ar(i_bus,in)
		})
	}
	defName { ^this.class.name.asString ++ numChannels.asString ++ env.asCompileString.hash.asFileSafeString }
	synthDefArgs { ^[\i_bus,patchOut.synthArg,\gate,1.0] }
	releaseToBundle { arg releaseTime,bundle;
		if(releaseTime.isNil,{ releaseTime = env.releaseTime; });
		if(synth.notNil,{
			bundle.add(synth.releaseMsg(releaseTime));
		});
		if(releaseTime > 0.01,{
			bundle.addFunction({
				SystemClock.sched(releaseTime,{ this.stop; nil; })
			});
		},{
			this.stopToBundle(bundle);
		});
	}
}


