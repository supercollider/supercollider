
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
	var <>env,socketGroup;
	
	*new { arg round=0.0,rate=\audio,numChannels=2;
		^super.new.round_(round)
			.rate_(rate).numChannels_(numChannels)
	}
	preparePlayer { arg player;
		player.prepareForPlay(socketGroup,bus: this.bus)
	}
	prepareAndSpawn { arg player;
		// use players prepare / spawn sequence
		player.play(socketGroup,nil,this.bus);
		isSleeping = false;
		this.changed;
		if(player != source,{
			source.stop;
		});
		source = player;
	}
	spawnPlayer { arg player,newEnv,onTrigger,atTime;
		var bundle;
		bundle = CXBundle.new;
		this.setSourceToBundle(player,bundle);
		bundle.addFunction({
			isSleeping = false;
			this.changed;
			onTrigger.value;
			nil
		});
		// should use a shared BeatSched
		bundle.send(this.server, atTime );
	}
	qspawnPlayer { arg player,newEnv,onTrigger;
		this.spawnPlayer(player,newEnv,onTrigger,BeatSched.tdeltaTillNext(round))
	}
	

	releaseVoice {
		isSleeping = true;
		if(source.notNil,{
			source.stop;
		});
		this.changed;
	}
	
	
	//
	prepareToBundle { arg group,bundle;
		group = group.asGroup;
		socketGroup = Group.basicNew(server: group.server);
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
		bundle.addMessage(this,\didSpawn);
	}
	instrArgFromControl { arg control;
		^if(this.rate == \audio,{
			In.ar(control,this.numChannels)
		},{
			In.kr(control,this.numChannels)
		})
	}
	
	// prepared sources only
	setSource { arg s,atTime;
		var bundle;
		bundle = CXBundle.new;
		this.setSourceToBundle(s,bundle);
		bundle.send(this.server,atTime);
	}
	setSourceToBundle { arg s,bundle;
		if(source != s,{
			//bundle.add( source.stopMsg );
			source.stop;
		});
		source = s;
		source.spawnOnToBundle(socketGroup,this.bus,bundle);
	}


	free {
		isPlaying = false;
		isSleeping = true;
		socketGroup.free;
		super.free;
	}
}


//
//PPPregVoice : PlayerSocket {
//
//	var <register;
//	
//	init {
//		register = Array.newClear(3);
//	}
//	setRegister { arg i;
//		register.put(i,)
//
//}




PlayerEffectSocket : PlayerSocket {

	var bus;
	
	setInputBus { arg abus;
		bus = abus.asBus;
		// assume not playing yet
	}	
	
	setSource { arg aplayer;
		aplayer.inputProxies.first.setInputBus(bus);
		super.setSource(aplayer);
	}

}


