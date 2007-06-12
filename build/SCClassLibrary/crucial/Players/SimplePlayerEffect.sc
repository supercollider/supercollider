
// simple, assumes we are not going to respawn the contents,
// (so no groups)
// that the lifetime of the played and the effected are the same

AbstractSinglePlayerEffect : HasSubject {

	var sharedBus;

	makeResourcesToBundle { arg bundle;
		sharedBus = SharedBus.newFrom(patchOut.bus,this);
		patchOut.bus = sharedBus;
	}
	prepareChildrenToBundle { arg bundle;
		subject.prepareToBundle(this.group,bundle, true,sharedBus,true);
	}		
	preparePlayer { arg player,bus;
		^player.prepareForPlay(this.group,true,sharedBus);
	}
	freeResourcesToBundle { arg bundle;
		bundle.addFunction({
			if(status == \isStopping or: status == \isFreeing or: status == \isStopped,{
				if(sharedBus.notNil,{
					sharedBus.releaseBus(this);
				});
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
	
	*new { arg player,env,numChannels=2;
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
	defName { ^this.class.name.asString ++ numChannels.asString ++ 
				env.asCompileString.hash.asFileSafeString 
	}
	synthDefArgs { ^[\i_bus,patchOut.synthArg,\gate,1.0] }
	releaseToBundle { arg releaseTime,bundle;
		status = \isReleaseing;
		if(releaseTime.isNil,{ releaseTime = env.releaseTime; });
		if(synth.notNil,{
			bundle.add(synth.releaseMsg(releaseTime + server.latency));
		});
		if(releaseTime > 0.01,{
			bundle.addFunction({
				SystemClock.sched(releaseTime,{ 
					// want everything but the synthFree in here
					this.stop(nil,false);
					nil; 
				})
			});
		},{
			this.stop(stopSynth: false);
		});
	}
}

