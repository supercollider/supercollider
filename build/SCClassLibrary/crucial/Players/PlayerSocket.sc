
/*
	a player that can switch playing different players in its socket

	numChannels will cause all players played within the socket to automatically adapt :
		stereo mixed to mono,
		mono expanded to stereo

*/

PlayerSocket : AbstractPlayerProxy {

	var <>round,<>rate,<>numChannels;
	var <>env,<socketGroup,sched;
	var <lastPlayer,<envdSource,onBat, dee,dum;
	var <fuseBlown = false;

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
		if(this.rate == \audio,{
			dee = EnvelopedPlayer(PlayerInputProxy(AudioSpec(numChannels)),env,numChannels);
			dum = EnvelopedPlayer(PlayerInputProxy(AudioSpec(numChannels)),env,numChannels);
			dee.name = this.name + "EnvelopedPlayer1";
			dum.name = this.name + "EnvelopedPlayer2";
			dee.onFuseBlown = { |time,value| this.fuseDidBlow(value) };
			dum.onFuseBlown = dee.onFuseBlown;
		},{
			dee = PlayerInputProxy(\bipolar);
			dum = PlayerInputProxy(\bipolar);
			dee.name = this.name + "dee";
			dum.name = this.name + "dum";
		});

		// dee and dum are swapped on alternate spawns
		// now playing:
		envdSource = dee;
		// "on bat" is a baseball term : the guy who is up to bat next
		onBat = dum;
	}
	prepareAndSpawn { arg player,releaseTime;
		if(player.readyForPlay.not,{
			this.preparePlayer(player,{
				this.spawnPlayer(player,releaseTime);
			})
		},{
			this.spawnPlayer(player,releaseTime);
		})
	}
	prepareAndQSpawn { arg player,releaseTime;
		if(player.readyForPlay.not,{
			this.preparePlayer(player,{
				this.qspawnPlayer(player,releaseTime);
			})
		},{
			this.qspawnPlayer(player,releaseTime);
		})
	}
	preparePlayer { arg player,onComplete;
		var b;
		b = AbstractPlayer.bundleClass.new;
		player.prepareToBundle(socketGroup,b,bus: this.bus);
		b.addFunction(onComplete);
		b.send(this.server)
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
		// needs xqschedBundle
		// for now it still needs xblock
		// and OSCSched needs to handle preparations
/*		var bundle;
		if(status == \isPlaying,{
			if((player !== lastPlayer) or: forceRespawn or: (socketStatus == \isSleeping),{
				bundle = AbstractPlayer.bundleClass.new;
				this.setSourceToBundle(player,bundle,releaseTime);
				sched.xqschedBundle(round,this.server,bundle);
			})
		});
*/
		this.spawnPlayer(player,releaseTime,sched.deltaTillNext(round))
	}
	// respawn the currently playing player
	qrespawnPlayer { arg releaseTime;
		this.spawnPlayer(source,releaseTime,sched.deltaTillNext(round),true)
	}

	releaseVoice { arg releaseTime;
		if(socketStatus == \isWaking,{
			sched.xblock;
			socketStatus = \isSleeping;
			^this.changed;
		});
		if(socketStatus != \isSleeping,{
			AbstractPlayer.bundle(this.server,nil,{ |bundle|
				var currentSubject;
				currentSubject = envdSource.subject;
				envdSource.releaseToBundle(releaseTime,bundle);
				bundle.addFunction({
					socketStatus = \isSleeping;
				});
			})
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
	}
	freeResourcesToBundle { arg bundle;
		socketGroup.freeToBundle(bundle);
		socketGroup = nil;
	}

	prepareChildrenToBundle { arg bundle;
		dee.prepareToBundle(socketGroup,bundle, true, this.bus);
		dum.prepareToBundle(socketGroup,bundle,true,this.bus);
		if(source.notNil,{
			source.prepareToBundle(socketGroup,bundle,true, this.bus);
		});
	}
	freeToBundle { |bundle|
		dee.freeToBundle(bundle);
		dum.freeToBundle(bundle);
		super.freeToBundle(bundle);
	}

	// no synth of my own
	loadDefFileToBundle { arg bundle,server;}

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
	}

	spawnToBundle { arg bundle;
		if(source.notNil,{
			this.setSourceToBundle(source,bundle);
		});
		bundle.addMessage(this,\didSpawn);
	}
	synthArg { ^this.bus.index }

	// the source should be already prepared
	setSource { arg s,atTime,releaseTime;
		var bundle;
		if(this.server.notNil,{ // else not even playing, or not loaded
			AbstractPlayer.bundle(this.server,atTime,{ |bundle|
				this.setSourceToBundle(s,bundle,releaseTime);
			})
		},{
			source = s;
		});
	}
	// the source should be already prepared
	setSourceToBundle { arg s,bundle,releaseTime;
		// the main switching method
		var currentSubject;

		if(envdSource.isPlaying,{
			envdSource.releaseToBundle(releaseTime,bundle);
			# envdSource, onBat = [onBat,envdSource];
		});
		if(this.rate == 'audio',{
			if(envdSource.subject.isKindOf(PlayerInputProxy),{
				envdSource.subject.freeToBundle(bundle)
			});
			source = s;
			envdSource.subject = source;
		},{
			if(envdSource.isKindOf(PlayerInputProxy),{
				envdSource.freeToBundle(bundle)
			});
			source = s;
			envdSource = source;
		});

		if(this.bus.rate != source.rate,{
			if(this.bus.isNil,{
				"PlayerSocket:setSourceToBundle: I am not prepared".error;
			},{
				("PlayerSocket:setSourceToBundle bus and source have different rates:"
					+ this.bus.rate + source.rate).error(this,source);
			})
		});
		socketStatus = \switching;
		envdSource.spawnOnToBundle(socketGroup,this.bus,bundle);
		bundle.addFunction({
			socketStatus = \isPlaying;
			fuseBlown = false;
			lastPlayer = source;
			{ this.changed; nil; }.defer;
			nil
		});
	}
	didFree {
		super.didFree;
		socketStatus = \isSleeping;
	}
	fuseDidBlow { |value|
		"% produced bad value: %".format(source,value).warn;
		{
			this.changed(\fuseDidBlow)
		}.defer
	}
	// emergency kill off contents
	socketFreeAll {
		socketGroup.freeAll
	}
	name {
		^name ?? {this.class.name.asString}
	}

	limit_ { |boo|
		if(this.rate == \audio,{
			dee.limit = boo;
			dum.limit = boo;
		})
	}
}

PlayerEffectSocket : PlayerSocket {

	var inputBus;

	setInputBus { arg abus;
		BusPool.retain(abus,this,"inputBus");
	}

	setSourceToBundle { arg aplayer,bundle,releaseTime=0.2;
		aplayer.inputProxies.first.setInputBus(inputBus);
		super.setSourceToBundle(aplayer,bundle,releaseTime);
	}
}

/*
a pure control class

ControlPlayerSocket : PlayerSocket {


	*new { arg round=0.0;
		^this.prNew(\control,1,round)
	}
	psinit {
		sched = OSCSched.new;
		dee = PlayerInputProxy(\bipolar);
		dum = PlayerInputProxy(\bipolar);
		dee.name = this.name + "EnvelopedPlayer1";
		dum.name = this.name + "EnvelopedPlayer2";
		// dee and dum are swapped on alternate spawns
		// now playing:
		envdSource = dee;
		// "on bat" is a baseball term : the guy who is up to bat next
		onBat = dum;
	}

	// the main switching method
	// the source should be prepared
	setSourceToBundle { arg s,bundle,releaseTime;
		var currentSubject;
		if(envdSource.isPlaying,{
			envdSource.releaseToBundle(releaseTime,bundle);
			# envdSource, onBat = [onBat,envdSource];
		});
		if(envdSource.isKindOf(PlayerInputProxy),{
			envdSource.subject.freeToBundle(bundle)
		});

		source = s;
		envdSource.subject = source;
		if(this.bus.rate != source.rate,{
			if(this.bus.isNil,{
				"PlayerSocket:setSourceToBundle: I am not prepared".error;
			},{
				("PlayerSocket:setSourceToBundle bus and source have different rates:"
					+ this.bus.rate + source.rate).error(this,source);
			})
		});
		envdSource.spawnOnToBundle(socketGroup,this.bus,bundle);
		socketStatus = \switching;
		bundle.addFunction({
			if(socketStatus != \switching, {
				socketStatus.debug("socket Status changed !!");
			});
			socketStatus = \isPlaying;
			lastPlayer = source;
			{ this.changed; nil; }.defer;
			nil
		});
	}
}

*/
