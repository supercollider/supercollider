

//lightweight objects that insulate different ways of playing/stopping.
//the bundle that is passed in is a MixedBundle
//these could be players in future, a superclass of AbstractPlayer would be required

AbstractPlayControl {
	var <source, <>channelOffset;
	
	*new { arg source, channelOffset=0;
		^super.newCopyArgs(source, channelOffset);
	}
	
	sendDef {}
	writeDef {}
	prepareForPlay {}
	build { ^true }
	pause { this.stop } 
	unpause { this.start }
	synth { ^nil }
	
	readyForPlay { ^true }
	synthDef { ^nil }

	sendDefToBundle {}
	
	playToBundle { arg bundle; 
		bundle.addAction(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}
	freeToBundle { arg bundle;
		bundle.addAction(this, \free);
	}
	
	stopToBundle { arg bundle;
		bundle.addAction(this, \stop);
	}
	
	stopClientToBundle { arg bundle;
		//used in shared node proxy
		this.stopToBundle(bundle);
	}
	
	play {
		this.subclassResponsibility(thisMethod);
	}
	
	stop {
		this.subclassResponsibility(thisMethod);
	}
	
	free {}
	
}

StreamControl : AbstractPlayControl {
	var <stream, clock;
		
	play {
		stream.stop;
		stream.play; //(clock);
	}
	
	stop {
		stream.stop;
	}
	mute {
		stream.mute;
	}
	unmute {
		stream.unmute;
	}
	
	build { arg proxy;
		stream = source.buildForProxy(proxy, channelOffset);
		clock = proxy.clock;
		^stream.notNil;
	}
	readyForPlay { ^stream.notNil }
	clear {
		stream = nil;
	}
	
}

SynthControl : AbstractPlayControl {
	var <synth, >hasGate=false;
	
	sendDef { } //assumes that SoundDef does send to the same server 
	writeDef { }
	build { ^true }
	sendDefToBundle {}
	name { ^source }
	clear { synth = nil }
	
	playToBundle { arg bundle, extraArgs, proxy;
		var synthMsg, name, group;
		group = proxy.asGroup;
		name = this.name;
		synth = Synth.basicNew(name, group.server);
		synth.isPlaying = true;
		synthMsg = [9, name, synth.nodeID, 1, group.nodeID]++extraArgs; 
		//check the target: better after the last object.
		//todo: Order does the playing.
		bundle.add(synthMsg);
		^synth
	}
	stopToBundle { arg bundle;
		if(synth.isPlaying,{
			if(this.hasGate, {
				bundle.add([15, synth.nodeID, '#gate', 0.0, \gate, 0.0]); //to be sure.
			}, {
				bundle.add([11, synth.nodeID]); //"/n_free"
			});
			synth.isPlaying = false;
		});
		

	}
	play { arg group, extraArgs;
		var bundle;
		group = group.asGroup;
		bundle = MixedBundle.new;
		this.playToBundle(bundle, extraArgs, group);
		bundle.send(group.server)
		^synth
	}
	stop { arg latency;
		var bundle;
		bundle = List.new;
		this.stopToBundle(bundle);
		synth.server.listSendBundle(latency, bundle)
	} 
			
	stopClientToBundle { }
	
	pause { synth.run(false) }
	unpause { synth.run(true) }
	hasGate { ^hasGate }
}


SynthDefControl : SynthControl {
	var <synthDef;
		
	hasGate { ^synthDef.hasGate }
	readyForPlay { ^synthDef.notNil }
	
	build { arg proxy, index=0; 
		var ok;
		synthDef = source.buildForProxy(proxy, channelOffset, index);//channelOffest
		ok = proxy.initBus(synthDef.rate, synthDef.numChannels);
		if(ok && synthDef.notNil, { 
			//schedule to avoid hd sleep latency. def writing is only for server reboot
			AppClock.sched(rrand(0.2, 1), { this.writeDef; nil }); 
		}, { 
			synthDef = nil; 
		})
	}
	
	
	free {
		synth = nil;
		synthDef = nil;
	}
	
	sendDefToBundle { arg bundle, server;
		bundle.addPrepare([5, synthDef.asBytes]); //"/d_recv"
	}
	
	sendDef { arg server;
		synthDef.send(server);
	}

	writeDef {
		synthDef.writeDefFile;
	}
	
	name { ^synthDef.name }
	
	
}

SoundDefControl : SynthDefControl {
	sendDef { } //assumes that SoundDef does send to the same server 
	writeDef { }
	build { synthDef = source.synthDef; ^true }
	sendDefToBundle {}
}



CXPlayerControl : AbstractPlayControl {
	//here the lazy bus init happens and allocation of client side ressources
	//there is no bundle passing in build
	build { arg proxy;
		var player, tempBus, ok;
		if(proxy.isNeutral.not, { proxy.bus.as(SharedBus) });
		source.prepareForPlay(bus: tempBus); //to get the rate
		proxy.initBus(source.rate ? 'audio', source.numChannels ? 2);
		//source.free; 
		tempBus.free;
		^ok
	}
	
	playToBundle { arg bundle, extraArgs, proxy;
		var bus, group;
		//we'll need channel offset maybe.
		group = Group.newToBundle(bundle, proxy.group, \addToTail);
//		if(source.readyForPlay, {
//			source.prepareToBundle(group, bundle);
//			source.bus = proxy.bus.as(SharedBus);
//		}, {
		
			source.prepareToBundle(group, bundle);
			source.makePatchOut(group, true, proxy.bus.as(SharedBus), bundle);
//		});
		source.spawnToBundle(bundle);
		^source.synth;
	}
	
	stopToBundle { arg bundle;
		source.stopToBundle(bundle);
	}
	
	stop {
		source.stop; //release? proxy time
	}
	play {
		source.play;
	}
	
	free { 
		source.free;
	}

}


//kr, ar, binop?
/*
NodeProxyControl : SynthControl {
	hasGate { ^true }
	playToBundle { arg bundle, extraArgs, proxy;
		^super.playToBundle(
			bundle, 
			extraArgs ++ [\in, source.index, \out, proxy.index], 
			proxy
		)
	}
	name {//for now only 2 channels
		^if(source.numChannels == 1, {'system-switch-1'}, {'system-switch-2'});	}
}
*/

