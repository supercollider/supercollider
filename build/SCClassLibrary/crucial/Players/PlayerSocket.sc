
/*
	a player that can switch to different players
	
	doesn't crossfade, this is a raw socket
	
	the players should be in a prepared state
		preparePlayer
		prepareAndTrigger
	
	
	should all be same rate
		numChannels should adapt
*/

PlayerSocket : AbstractPlayerProxy {

	var <>round,<>rate,<>numChannels;
	var <>env,socketGroup,sched;
	var isWaking = false;
	var <lastPlayer;
	
	*new { arg rate=\audio,numChannels=2,round=0.0;
		^super.new.round_(round)
			.rate_(rate).numChannels_(numChannels)
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
		bsize = this.preparePlayer(player);
		SystemClock.sched(bsize / 10.0,{
			this.spawnPlayer(player,releaseTime);
			nil
		})
	}
	prepareAndQSpawn { arg player,releaseTime=0.0;
		var bsize;
		bsize = this.preparePlayer(player);
		SystemClock.sched(bsize / 15.0,{
			this.qspawnPlayer(player,releaseTime);
			nil
		})
	}
	preparePlayer { arg player;
		// returns the bundle size
		^player.prepareForPlay(socketGroup,bus: sharedBus)
	}

	spawnPlayer { arg player,releaseTime=0.0,beatDelta=0.0;
		var bundle;
		bundle = CXBundle.new;
		this.setSourceToBundle(player,bundle,releaseTime);
		bundle.addFunction({
			isSleeping = isWaking = false;
			lastPlayer = player;
			{ this.changed; }.defer;
			nil
		});
		sched.xschedCXBundle(beatDelta,this.server,bundle);
	}
	qspawnPlayer { arg player,releaseTime=0.0;
		isWaking = true;
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
			if(source.notNil,{
				bundle = CXBundle.new;
				source.releaseToBundle(releaseTime,bundle);
				bundle.addFunction({ isSleeping = true; });
				sched.xschedCXBundle(0.0,this.server,bundle);
			});
		});
	}
	isSleeping {
		^(isSleeping and: isWaking.not)
	}

	
	//
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		socketGroup = Group.basicNew(server: group.server);
		NodeWatcher.register(socketGroup);
		bundle.add( socketGroup.addToTailMsg(group) );
		if(source.notNil,{
			source.prepareToBundle(socketGroup,bundle)
		})
	}
	childrenMakePatchOut { arg group,private = true;
		if(source.notNil,{
			source.childrenMakePatchOut(socketGroup,private)
		})
	}
	loadDefFileToBundle { arg bundle,server;
		if(source.notNil,{
			source.loadDefFileToBundle(bundle,server)
		})
	}
	spawnToBundle { arg bundle;
		if(source.notNil,{
			source.spawnToBundle(bundle)
		});
		bundle.addAction(this,\didSpawn);
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

	// prepared sources only
	setSource { arg s,atTime;
		var bundle;
		// or this.isPlay
		if(this.server.notNil,{ // else not even playing, or not loaded
			bundle = CXBundle.new;
			this.setSourceToBundle(s,bundle);
			bundle.send(this.server,atTime);
		},{
			source = s;
			rate = source.rate ? rate ? \audio;
			numChannels = source.tryPerform(\numChannels) ? numChannels ? 1;
		});
	}
	qsetSource { arg s,round=4.0;
		this.setSource(s,BeatSched.tdeltaTillNext(round));
	}
	setSourceToBundle { arg s,bundle,releaseTime=0.2;
		if(source.isPlaying,{
			source.releaseToBundle(releaseTime,bundle);
		});
		source = s;
		// possibly should complain if different and we are playing
		rate = source.rate ? rate ? \audio;
		numChannels = source.tryPerform(\numChannels) ? numChannels ? 1;
		
		source.spawnOnToBundle(socketGroup,sharedBus,bundle);
	}

	free {
		isPlaying = false;
		isSleeping = true;
		socketGroup.free;
		sharedBus.releaseBus;
		super.free;
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
		super.setSourceToBundle(aplayer,bundle,releaseTime);
	}
}
