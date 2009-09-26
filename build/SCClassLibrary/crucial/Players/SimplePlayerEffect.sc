
// simple, assumes we are not going to respawn the contents,
// (so no groups)
// that the lifetime of the played and the effected are the same

AbstractSinglePlayerEffect : HasSubject {

/*	makeResourcesToBundle { arg bundle;
		//sharedBus = SharedBus.newFrom(patchOut.bus,this);
		//if(sharedBus.owner === this,{
		//	AbstractPlayer.annotate(sharedBus,"AbstractSinglePlayerEffect");
		//});
		sharedBus = patchOut.bus;// = sharedBus;
	}
*/
	prepareChildrenToBundle { arg bundle;
		subject.prepareToBundle(this.group,bundle, true,this.bus);
	}
	preparePlayer { arg player;
		^player.prepareForPlay(this.group,true,this.bus);
	}
}

PlayerAmp : AbstractSinglePlayerEffect {

	var <amp=1.0,<>spec;

	*new { arg player,amp=1.0,spec=\amp;
		^super.new(player).spec_(spec.asSpec).amp_(amp)
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
		amp = spec.constrain(v);
		if(synth.isPlaying,{
			synth.set(\amp,amp)
		})
	}
	db_ { arg db;
		this.amp = db.dbamp;
	}
	value_ { arg v;
		this.amp = v;
	}

	guiClass { ^PlayerAmpGui }
}

/*
	put an envelope on the player's output bus.
	correct numChannels of the player's output eg. from 1 to 2 if needed
	protect against bad values (nan, inf)
	set limit = true to add a limiter
*/
EnvelopedPlayer : AbstractSinglePlayerEffect {

	var <>env,<>numChannels,<>limit,<>onFuseBlown;
	var fuseListener;

	*new { arg player,env,numChannels=2,limit=false;
		^super.new(player).env_(env).numChannels_(numChannels).limit_(limit)
	}
	storeArgs { ^[subject,env,numChannels] }
	asSynthDef {
		^SynthDef(this.defName,{ arg i_bus,gate;
			var in,pnc;
			var good;
			pnc = subject.numChannels;
			in = In.ar(i_bus,pnc);
			good = BinaryOpUGen('==', CheckBadValues.ar(in, 0, 0), 0);
			 // silence the output if freq is bad
			in = in * good * EnvGen.kr(env,gate,doneAction:0);
			// phone home
			SendTrig.kr(1.0-A2K.kr(good),1,in);
			if(limit,{
				in = Limiter.ar(in)
			});
			if(numChannels.notNil,{
				in = NumChannels.ar(in,numChannels,true);
			});
			ReplaceOut.ar(i_bus,in)
		})
	}
	defName { ^this.class.name.asString ++ numChannels.asString ++ limit.binaryValue.asString ++
				env.asCompileString.hash.asFileSafeString
	}
	synthDefArgs { ^[\i_bus,patchOut.synthArg,\gate,1.0] }
	didSpawn {
		var commandpath = ['/tr', this.synth.nodeID, 1];
		fuseListener = OSCpathResponder(this.server.addr, commandpath,
			{|time,responder,message|
				(onFuseBlown ? {|time,msg| "% got bad value: %".format(this,msg).warn }).value(time,message[3])
			});
		fuseListener.add;
	}
	didStop {
		fuseListener.remove;
	}
	// this is a once-only event; you cannot retrigger it
	releaseToBundle { arg releaseTime,bundle;
		if(releaseTime.isNil,{ releaseTime = env.releaseTime; });

		if(synth.isPlaying,{
			bundle.add(synth.releaseMsg(releaseTime));
			bundle.addFunction({
				AbstractPlayer.bundle(this.server,releaseTime,{ |bundle|
					this.freeToBundle(bundle);
				})
			});
		},{
			this.freeToBundle(bundle)
		});
	}
}

