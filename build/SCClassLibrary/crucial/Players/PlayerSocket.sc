
/*
	a player that can switch playing different players in its socket

	should all be same rate
	numChannels will adapt
*/

PlayerSocket : AbstractPlayerProxy {

	var <>round,<>rate,<>numChannels;
	var <>env,<socketGroup,sched;
	var <lastPlayer,<envdSource,onBat, dee,dum;

	*new { arg rate=\audio,numChannels=2,round=0.0,env;
		^this.prNew(rate,numChannels,round,env)
	}
	*prNew {  arg rate=\audio,numChannels=2,round=0.0,env;
		^super.new.round_(round)
			.rate_(rate).numChannels_(numChannels)
			.env_(env ?? {Env.asr(0.01,1.0,0.1)})
			.psinit
	}
	psinit {
		sched = OSCSched.new;
		dee = EnvelopedPlayer(PlayerInputProxy(AudioSpec(numChannels)),env,numChannels);
		dum = EnvelopedPlayer(PlayerInputProxy(AudioSpec(numChannels)),env,numChannels);
		// dee and dum are swapped on alternate spawns
		// now playing:
		envdSource = dee;
		// "on bat" is a baseball term : the guy who is up to bat next
		onBat = dum;
		// problem: 
			// the EnvelopedPlayer needs to dynamically change the numChannels
			// it needs to build multiple synth defs or just do some bus copying
	}
	prepareAndSpawn { arg player,releaseTime;
		var bsize=0.4;
		this.preparePlayer(player);
		if((bsize > 0) ,{
			SystemClock.sched(bsize / 7.0,{
				this.spawnPlayer(player,releaseTime);
				nil
			})
		},{
			this.spawnPlayer(player,releaseTime);
		})
	}
	prepareAndQSpawn { arg player,releaseTime;
		var bsize=0.4;
		this.preparePlayer(player);
		if(bsize > 0 ,{
			SystemClock.sched(bsize / 7.0,{
				//player.status.debug(player);
				this.qspawnPlayer(player,releaseTime);
				nil
			})
		},{
			//player.status.debug(player);
			this.qspawnPlayer(player,releaseTime);
		})
	}
	preparePlayer { arg player;
		^player.prepareForPlay(socketGroup,bus: sharedBus);
	}

	spawnPlayer { arg player,releaseTime,beatDelta=0.0,forceRespawn=false;
		var bundle;
		if(status == \isPlaying,{
			if((player !== lastPlayer) or: forceRespawn or: (socketStatus == \isSleeping),{
				bundle = AbstractPlayer.bundleClass.new;
				this.setSourceToBundle(player,bundle,releaseTime);
				sched.xschedBundle(beatDelta,this.server,bundle);
			})
		});
	}
	qspawnPlayer { arg player,releaseTime;
		// TODO switch to TempoClock so tempo can change after qspawn sent
		// for now it still needs xblock
		this.spawnPlayer(player,releaseTime,sched.deltaTillNext(round))
	}
	// respawn the currently playing player
	qrespawnPlayer { arg releaseTime;
		this.spawnPlayer(source,releaseTime,sched.deltaTillNext(round),true)
	}

	releaseVoice { arg releaseTime;
		var bundle;
		if(socketStatus == \isWaking,{
			sched.xblock;
			//"blocked isWaking, setting to isSleeping".debug(thisMethod);
			socketStatus = \isSleeping;
			^this.changed;
		});
		if(socketStatus != \isSleeping,{
			//if(envdSource.notNil,{
				bundle = AbstractPlayer.bundleClass.new;
				envdSource.releaseToBundle(releaseTime,bundle);
				bundle.addFunction({ socketStatus = \isSleeping; });
				bundle.send(this.server);
			//});
		});
	}
	isSleeping {
		^(socketStatus==\isSleeping)
	}
	qwake { this.qspawnPlayer(lastPlayer); }

	makeResourcesToBundle { arg bundle;
		socketGroup = Group.basicNew(server: server);
		this.annotate(socketGroup,"socketGroup");
		NodeWatcher.register(socketGroup);
		bundle.add( socketGroup.addToTailMsg(group) );

		sharedBus = SharedBus.newFrom(patchOut.bus,this);
		this.annotate(sharedBus,"Shared Bus");
		patchOut.bus = sharedBus;
	}
	freeResourcesToBundle { arg bundle;
		socketGroup.freeToBundle(bundle);
		socketGroup = nil;
		
	}

	prepareChildrenToBundle { arg bundle;
		dee.prepareToBundle(socketGroup,bundle, true, sharedBus);
		dum.prepareToBundle(socketGroup,bundle,true,sharedBus);
		if(source.notNil,{ 
			source.prepareToBundle(socketGroup,bundle,true, sharedBus);
		});
	}
	// no synth of my own
	loadDefFileToBundle { arg bundle,server;
		// this should all be done when prepareChildrenToBundle is done !!
		//if(source.notNil,{
		//	source.loadDefFileToBundle(bundle,server)
		//});
		
		//var envPatch;
/*		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});

		dee.loadDefFileToBundle(bundle,server);
		// same same
		dum.loadDefFileToBundle(bundle,server);
*/
	}

	instrArgFromControl { arg control;
		// avoiding the super which delegates to the source
		// i control the enveloped players etc. so don't feed it the source's instrArg
		// feed it mine
		^this.rate.switch(
			\audio,{
				^In.ar(control,this.numChannels ?? {Error("numChannels is nil"+this).throw})
			},
			\control,{
				^In.kr(control,this.numChannels ?? {Error("numChannels is nil"+this).throw})
			},
			\stream,{
				^control
				//this.synthArg
			},
			{
				Error("AbstractPlayer:instrArgFromControl: rate unknown = "+this.rate).throw
			}
		)

/*		^if(source.notNil,{
			source.instrArgFromControl(control)
		},{
			^super.instrArgFromControl(control)
		})
*/	}
	
	spawnToBundle { arg bundle;
		if(source.notNil,{
			this.setSourceToBundle(source,bundle);
		});
		bundle.addMessage(this,\didSpawn);
	}
	synthArg { ^sharedBus.index }

	// prepared objects only
	setSource { arg s,atTime,releaseTime;
		var bundle;
		if(this.server.notNil,{ // else not even playing, or not loaded
			bundle = AbstractPlayer.bundleClass.new;
			this.setSourceToBundle(s,bundle,releaseTime);
			bundle.sendAtTime(this.server,atTime);
		},{
			source = s;
		});
	}
	// the main switching method
	// the source should be prepared
	setSourceToBundle { arg s,bundle,releaseTime;
		if(envdSource.isPlaying,{
			envdSource.releaseToBundle(releaseTime,bundle);
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
			if(sharedBus.isNil,{
				"PlayerSocket-setSourceToBundle: I am not prepared".error;
			},{
				("PlayerSocket-setSourceToBundle bus and source have different rates:"
					+ sharedBus.rate + source.rate).error(this,source);
			})
		});
		envdSource.spawnOnToBundle(socketGroup,sharedBus,bundle);
		socketStatus = \switching;
		bundle.addFunction({
			if(socketStatus != \switching, {
				socketStatus.debug("socket Status changed !!");
			});
			socketStatus = \isPlaying;
			//isSleeping = isWaking = false;
			lastPlayer = source;
			{ this.changed; nil; }.defer;
			nil
		});
	}
	didFree {
		super.didFree;
		socketStatus = \isSleeping;
		//isSleeping = true;
		if(sharedBus.notNil,{
			sharedBus.releaseBus(this);
			AbstractPlayer.removeAnnotation(sharedBus);
			sharedBus = nil;
		});
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
		this.annotate(inputBus,"inputBus");
		// assume not playing yet
	}

	setSourceToBundle { arg aplayer,bundle,releaseTime=0.2;
		aplayer.inputProxies.first.setInputBus(inputBus);
		//("setting inputbus",inputBus);
		super.setSourceToBundle(aplayer,bundle,releaseTime);
	}
}
