
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
	var <>env,<isPlaying = false, <isSleeping = true;
	
	*new { arg round=0.0,rate=\audio,numChannels=2;
		^super.new.round_(round)
			.rate_(rate).numChannels_(numChannels)
	}
	
	prepareToBundle { arg group,bundle;
		if(source.notNil,{
			super.prepareToBundle(group,bundle)
		})
	}
//	makePatchOut { arg group,public;
//		if(source.notNil,{
//			super.makePatchOut(group,public)
//		})
//	}
	childrenMakePatchOut { arg group,private = true;
		if(source.notNil,{
			super.childrenMakePatchOut(group,private)
		})
	}
	spawnAtTime { arg atTime;
		if(source.notNil,{
			super.spawnAtTime(atTime)
		});
		if(atTime.isNumber,{
			AppClock.sched(atTime,{ isPlaying = true; nil });
		},{
			isPlaying = true;
		})
	}
	loadDefFileToBundle { arg bundle,server;
		if(source.notNil,{
			super.loadDefFileToBundle(bundle,server)
		})
	}
	spawnToBundle { arg bundle;
		if(source.notNil,{
			super.spawnToBundle(bundle)
		})
	}
	
	// prepared sources only
	setSource { arg s,atTime;
		this.server.listSendBundle(atTime,
			this.setSourceMsg(s)
		)
	}
	setSourceMsg { arg s;
		var bundle;
		bundle = List.new;
		// do replace, same bus
		// set patchout of s ?
		if(source.notNil,{
			//bundle.add( source.stopMsg );
			source.stop;
			
			// deallocate busses !
			// but keep samples etc.
			source.freePatchOut;
		});
		source = s;
		source.spawnOnToBundle(this.group,this.bus,bundle);
		^bundle
	}
	trigger { arg player,newEnv;
		isSleeping = false;
		// ideally replace the same node, bus
		this.setSource(player);
		this.changed;
	}
	qtime { ^BeatSched.tdeltaTillNext(round) }
	qtrigger { arg player,newEnv,onTrigger;
		var t;
		// should use a shared BeatSched
		this.setSource(player,t = BeatSched.tdeltaTillNext(round));
		AppClock.sched(t,{
			isSleeping = false;
			this.changed;
			onTrigger.value;
			nil
		});
	}
	
	preparePlayer { arg player;
		// should have prepared the socket first
		player.prepareForPlay(this.group,bus: this.bus)
	}
	prepareAndTrigger { arg player;
		// play on my bus
		player.play(this.group,nil,this.bus);

		if(source.notNil,{
			source.free; // or stop
		});
		source = player;
	}

	release {
		isSleeping = true;
		if(source.notNil,{
			source.stop;
		});
		this.changed;
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


