

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
		bundle.addFunction({ 
			this.play; 
		}); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}
	
	stopToBundle { arg bundle;
		bundle.addFunction({ this.stop });
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
}

StreamControl : AbstractPlayControl {
	var <stream;
		
	play {
		stream.stop;
		stream.play; //what to do with clock?
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
		^stream.notNil;
	}
	readyForPlay { ^stream.notNil }
	clear {
		stream = nil;
	}
	
}


SynthDefControl : AbstractPlayControl {
	var <synthDef, <synth, <>index=0;
		
		
			
	hasGate { ^synthDef.hasGate }
	readyForPlay { ^synthDef.notNil }
	
	build { arg proxy; 
		var ok;
		synthDef = source.wrapForNodeProxy(proxy, channelOffset, index);//channelOffest
		ok = proxy.initBus(synthDef.rate, synthDef.numChannels);
		^if(ok && synthDef.notNil, { 
			//schedule to avoid hd sleep latency. this is only for server reboot
			AppClock.sched(rrand(0.2, 1), { this.writeDef; nil }); 
			true 
		}, { 
			synthDef = nil; false 
		})
	}
	
	
	clear {
		synth = nil;
		synthDef = nil;
	}
	
	sendDefToBundle { arg bundle, server;
		bundle.addPrepare(["/d_recv", synthDef.asBytes]);
		//this.registerDef(server);
	}
	
	sendDef { arg server;
		synthDef.send(server);
		//this.registerDef(server);
	}
	/*
	registerDef { arg server;
		Library.put(synthDef, server, synthDef.name, true);
	}
	
	onServer { arg server;
		^Library.at(synthDef, server, synthDef.name).notNil;
	}
	*/

	writeDef {
		synthDef.writeDefFile;
	}
	
	playToBundle { arg bundle, extraArgs, group;
		var synthMsg, msg;
		group = group.asGroup;
		synth = Synth.basicNew(synthDef.name, group.server);
		synth.isPlaying = true;
		synthMsg = [9, synthDef.name, synth.nodeID, 0, group.nodeID]++extraArgs;
		/*
		msg = if(this.onServer, {	
			synthMsg
		}, {
			msg = ["/d_recv", synthDef.asBytes, synthMsg] 
		});
		bundle.add(msg);
		*/
		bundle.add(synthMsg);
		^synth
	}
	
	play { arg group, extraArgs;
		var bundle;
		group = group.asGroup;
		bundle = MixedBundle.new;
		this.playToBundle(bundle, extraArgs, group);
		bundle.send(group.server)
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
	
	stopClientToBundle { }
	
	stop { arg latency;
		var bundle;
		bundle = List.new;
		this.stopToBundle(bundle);
		synth.server.listSendBundle(latency, bundle)
	} 
	
	pause { synth.run(false) }
	unpause { synth.run(true) }

}

SoundDefControl : SynthDefControl {
	sendDef { } //assumes that SoundDef does send to the same server 
	writeDef { }
}

//still to do
CXPlayerControl : AbstractPlayControl {
	playToBundle { arg bundle, extraArgs, group;
		source.prepareToBundle(group, bundle);
		source.spawnToBundle(bundle);
		^source.synth;
	}

}

