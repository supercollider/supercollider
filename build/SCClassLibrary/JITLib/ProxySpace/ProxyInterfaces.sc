

//lightweight objects that insulate different ways of playing/stopping.
//the bundle that is passed in is a MixedBundle

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
	
	readyForPlay { ^true }
	synthDef { ^nil }

	sendDefToBundle {}
	
	playToBundle { arg bundle; 
		bundle.addAction(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
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
		stream = source.wrapForNodeProxy(proxy, channelOffset);
		clock = proxy.clock;
		^stream.notNil;
	}
	readyForPlay { ^stream.notNil }
	clear {
		stream = nil;
	}
	
}

SynthControl : AbstractPlayControl {
	var <synth;
	
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
		synthMsg = [9, name, synth.nodeID, 0, group.nodeID]++extraArgs;
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
	hasGate { ^false }
}


SynthDefControl : SynthControl {
	var <synthDef;
		
	hasGate { ^synthDef.hasGate }
	readyForPlay { ^synthDef.notNil }
	
	build { arg proxy, index=0; 
		var ok;
		synthDef = source.wrapForNodeProxy(proxy, channelOffset, index);//channelOffest
		ok = proxy.initBus(synthDef.rate, synthDef.numChannels);
		if(ok && synthDef.notNil, { 
			//schedule to avoid hd sleep latency. this is only for server reboot
			AppClock.sched(rrand(0.2, 1), { this.writeDef; nil }); 
		}, { 
			synthDef = nil; 
		})
	}
	
	
	clear {
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
	build { arg proxy;
		source.asSynthDef;//build synthDef
		^proxy.initBus(source.rate, source.numChannels);
	}
	playToBundle { arg bundle, extraArgs, proxy;
		var bus, group;
		bus = proxy.bus;
		group = proxy.group;
		source.prepareForPlay(group, false, bus);
		source.spawnOnToBundle(group, bus, bundle);
		^source.synth;
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

