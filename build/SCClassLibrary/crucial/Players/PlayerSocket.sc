
/*
	a player that can switch to different players
	
	should all be same rate
	numChannels will adapt
*/

PlayerSocket : AbstractPlayerProxy {

	var <>round,<>rate,<>numChannels;
	var <>env,socketGroup,sched;
	var isWaking = false;
	var <lastPlayer,envdSource,onBat, dee,dum;
	
	*new { arg rate=\audio,numChannels=2,round=0.0,env;
		^super.new.round_(round)
			.rate_(rate).numChannels_(numChannels)
			.env_(env ?? {Env.asr(0.01,1.0,0.1)})
			.psinit
	}
	psinit {
		sched = OSCSched.new;
	}
	topMakePatchOut { arg group,private=false,bus;
		super.topMakePatchOut(group,private,bus);
		sharedBus = SharedBus.newFrom(patchOut.bus,this);
		patchOut.bus = sharedBus;
	}
	prepareAndSpawn { arg player,releaseTime=0.0;
		var bsize;
		bsize = player.prepareForPlay(onBat.group,bus: sharedBus);
		SystemClock.sched(bsize / 7.0,{
			this.spawnPlayer(player,releaseTime);
			nil
		})
	}
	prepareAndQSpawn { arg player,releaseTime=0.0;
		var bsize;
		bsize = player.prepareForPlay(onBat.group,bus: sharedBus);
		SystemClock.sched(bsize / 7.0,{
			this.qspawnPlayer(player,releaseTime);
			nil
		})
	}
	preparePlayer { arg player;
		^onBat.preparePlayer(player);
	}

	spawnPlayer { arg player,releaseTime=0.0,beatDelta=0.0;
		var bundle;
		if((player !== lastPlayer) or: isSleeping,{ // freePatchOut overlaps
			bundle = CXBundle.new;
			this.setSourceToBundle(player,bundle,releaseTime);
			sched.xschedCXBundle(beatDelta,this.server,bundle);
		});
	}
	qspawnPlayer { arg player,releaseTime=0.0;
		isWaking = true;
		// TODO switch to TempoClock so tempo can change after qspawn sent
		// for now it still needs xblock
		this.spawnPlayer(player,releaseTime,sched.deltaTillNext(round))
	}
	
	releaseVoice { arg releaseTime=0.0;
		var bundle;
		if(isWaking,{
			sched.xblock;
			isWaking = false;
			^this.changed;
		});
		if(isSleeping.not,{
			if(envdSource.notNil,{
				bundle = CXBundle.new;
				envdSource.releaseToBundle(releaseTime,bundle);
				bundle.addFunction({ isSleeping = true; });
				bundle.clumpedSendNow(this.server);
			});
		});
	}
	isSleeping {
		^(isSleeping and: isWaking.not)
	}
	
//	prepareToBundle { arg agroup,bundle;
//		group = agroup.asGroup;
//		server = group.server;	
//		// children should be taken care of
//		super.prepareToBundle(group,bundle);
//	}
	makeResourcesToBundle { arg bundle;
		socketGroup = Group.basicNew(server: server);
		this.annotate(socketGroup,"socketGroup");
		NodeWatcher.register(socketGroup);
		bundle.add( socketGroup.addToTailMsg(group) );

		dee = EnvelopedPlayer(Patch({NumChannels.ar(SinOsc.ar,this.numChannels)}),env,this.numChannels);
		dum = EnvelopedPlayer(Patch({NumChannels.ar(SinOsc.ar,this.numChannels)}),env,this.numChannels);
		dee.prepareToBundle(socketGroup,bundle,bus: sharedBus);
		dum.prepareToBundle(socketGroup,bundle,bus: sharedBus);
		envdSource = dee;
		onBat = dum;
	}
	
	// no synth of my own
	loadDefFileToBundle { arg bundle,server;
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});
	}
	
	instrArgFromControl { arg control;
		^if(source.notNil,{
			source.instrArgFromControl(control)
		},{
			if(this.rate == \audio,{
				In.ar(control,this.numChannels)
			},{
				if(this.rate == \control,{
					In.kr(control,this.numChannels)
				},{
					control
				})
			})
		})
	}
	spawnToBundle { arg bundle;
		if(source.notNil,{
			this.setSourceToBundle(source,bundle);
		});
		bundle.addMessage(this,\didSpawn);
	}
	synthArg { ^sharedBus.index }
	
	setSource { arg s,atTime,releaseTime;
		var bundle;
		if(this.server.notNil,{ // else not even playing, or not loaded
			bundle = CXBundle.new;
			this.setSourceToBundle(s,bundle,releaseTime);
			bundle.sendAtTime(this.server,atTime);
		},{
			source = s;
		});
	}
	setSourceToBundle { arg s,bundle,releaseTime;
		if(envdSource.isPlaying,{
			envdSource.releaseToBundle(releaseTime,bundle);

// release happening later
// removing patch out
// after next event has already created a new one
// next event fails with no patch out

		//	"releaseing".debug(envdSource.identityHash);
			if(envdSource === dee,{
				envdSource = dum;
				onBat = dee;
			},{
				envdSource = dee;
				onBat = dum;
			});
		});
		source = s;
		envdSource.subject = source;
		if(sharedBus.rate != source.rate,{
			("PlayerSocket-setSourceToBundle sharedBus.rate + source.rate:" 
				+ sharedBus.rate + source.rate).error(this,source);
		});
		// "spawingin on".debug(envdSource.identityHash);
		envdSource.spawnOnToBundle(socketGroup,sharedBus,bundle);
		bundle.addFunction({
			isSleeping = isWaking = false;
			lastPlayer = source;
			{ this.changed; nil; }.defer;
			nil
		});
	}
	freeToBundle { arg bundle;
		super.freeToBundle(bundle);
		socketGroup.freeToBundle(bundle);
		socketGroup = nil;
		bundle.addMessage(this,\didFree);
	}
	didFree {
		isPlaying = false;
		isSleeping = true;
		if(sharedBus.notNil,{
			sharedBus.releaseBus(this);
			sharedBus = nil;
		});
		readyForPlay = false;
	}
	
	// emergency kill off contents
	socketFreeAll {
		socketGroup.freeAll
	}
}

PlayerEffectSocket : PlayerSocket {

	var inputBus;

	setInputBus { arg abus;
		// who did we get this from ?
		//"PlayerEffectSocket-setInputBus".debug;
		inputBus = SharedBus.newFrom(abus.asBus,this);
		// assume not playing yet
	}	
	
	setSourceToBundle { arg aplayer,bundle,releaseTime=0.2;
		aplayer.inputProxies.first.setInputBus(inputBus);
		//.insp("setting inputbus",inputBus);
		super.setSourceToBundle(aplayer,bundle,releaseTime);
	}
}
