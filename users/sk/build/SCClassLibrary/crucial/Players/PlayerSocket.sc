
/*
	a player that can switch playing different players in its socket
	
	should all be same rate
	numChannels will adapt
*/

PlayerSocket : AbstractPlayerProxy {

	var <>round,<>rate,<>numChannels;
	var <>env,socketGroup,sched;
	var <lastPlayer,envdSource,onBat, dee,dum;
	
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
	}
	prepareAndSpawn { arg player,releaseTime;
		var bsize;
		if((bsize = this.preparePlayer(player)) > 0 ,{
			SystemClock.sched(bsize / 7.0,{
				this.spawnPlayer(player,releaseTime);
				nil
			})
		},{
			this.spawnPlayer(player,releaseTime);
		})		
	}
	prepareAndQSpawn { arg player,releaseTime;
		var bsize;
		if((bsize = this.preparePlayer(player)) > 0 ,{
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

	spawnPlayer { arg player,releaseTime,beatDelta=0.0;
		var bundle;
		//lastPlayer.identityHash.debug;
		if(status == \isPlaying,{
			if((player !== lastPlayer) or: (socketStatus == \isSleeping),{ 
				//socketStatus = \isWaking;
				bundle = CXBundle.new;
				this.setSourceToBundle(player,bundle,releaseTime);
				sched.xschedCXBundle(beatDelta,this.server,bundle);
			}/*,{
				[player === lastPlayer, socketStatus].debug(this);
			}*/)
		}/*,{
			"not playing".debug(status);
		}*/);
	}
	qspawnPlayer { arg player,releaseTime;
		// TODO switch to TempoClock so tempo can change after qspawn sent
		// for now it still needs xblock
		this.spawnPlayer(player,releaseTime,sched.deltaTillNext(round))
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
			if(envdSource.notNil,{
				bundle = CXBundle.new;
				envdSource.releaseToBundle(releaseTime,bundle);
				bundle.addFunction({ socketStatus = \isSleeping; });
				bundle.clumpedSendNow(this.server);
			});
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

		envdSource = dee;
		onBat = dum;
	
		sharedBus = SharedBus.newFrom(patchOut.bus,this);
		this.annotate(sharedBus,"Shared Bus");
		patchOut.bus = sharedBus;
	}
	freeResourcesToBundle { arg bundle;
		socketGroup.freeToBundle(bundle);
		socketGroup = nil;
	}

	prepareChildrenToBundle { arg bundle;
		dee.prepareToBundle(socketGroup,bundle, true, sharedBus, true);
		dum.prepareToBundle(socketGroup,bundle,true,sharedBus, true);
		if(source.notNil,{ source.prepareToBundle(socketGroup,bundle,true, sharedBus,true) });
	}
	// no synth of my own
	loadDefFileToBundle { arg bundle,server;
		this.children.do({ arg child;
			child.loadDefFileToBundle(bundle,server);
		});
		// if the children updated their rate/numChannels (if there are any children)
		numChannels = this.children.maxValue({ arg it; it.numChannels }) ? numChannels;
		rate = this.children.first.rate ? rate;

		dee = EnvelopedPlayer(Patch({NumChannels.ar(SinOsc.ar,numChannels)}),env,numChannels);
		dum = EnvelopedPlayer(Patch({NumChannels.ar(SinOsc.ar,numChannels)}),env,numChannels);
		dee.loadDefFileToBundle(bundle,server);
		dum.loadDefFileToBundle(bundle,server);
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
	
	// prepared objects only
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
	// the main switching method
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
		if(super.didFree) {
			socketStatus = \isSleeping;
			//isSleeping = true;
			if(sharedBus.notNil,{
				sharedBus.releaseBus(this);
				sharedBus = nil;
			});
		}
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
		//("setting inputbus",inputBus);
		super.setSourceToBundle(aplayer,bundle,releaseTime);
	}
}
