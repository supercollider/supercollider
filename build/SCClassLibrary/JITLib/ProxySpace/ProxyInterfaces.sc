

//lightweight objects that insulate different ways of playing/stopping.
//the bundle that is passed in is a MixedBundle
//these could be players in future, a superclass of AbstractPlayer would be required

AbstractPlayControl {
	var <source, <>channelOffset;
	var <paused=false;
	
	*new { arg source, channelOffset=0;
		^super.newCopyArgs(source, channelOffset);
	}
	
	
	writeDef {}
	prepareForPlay {}
	build { ^true }
	pause { this.stop } 
	resume { this.start }
	free {}
	synth { ^nil }
	
	readyForPlay { ^true }
	synthDef { ^nil }
	distributable {Ê^false } // shared proxy support

	sendDefToBundle {}
	spawnToBundle {}
	
	playToBundle { arg bundle, args; 
		bundle.addAction(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}
	
	freeToBundle { arg bundle;
		bundle.addAction(this, \free);
	}
	
	resumeToBundle { arg bundle, args;
		bundle.addAction(this, \resume);
	}
	
	stopToBundle { arg bundle;
		bundle.addAction(this, \stop);
	}
	
	stopClientToBundle { arg bundle;
		//used in shared node proxy
		this.stopToBundle(bundle);
	}
	
	play { this.subclassResponsibility(thisMethod) }
	stop { this.subclassResponsibility(thisMethod) }
	
	
	
}

StreamControl : AbstractPlayControl {
	var <stream, clock;
		
	play {
		stream.stop;
		stream.play(nil, false, 0); //(clock);
		CmdPeriod.add(this); // should maybe be in PauseStream
	}
	
	stop {
		stream.stop;
	}
	pause { stream.pause; paused=true }
	resume { arg clock, quant=1.0; stream.resume(clock, quant); paused=false }
	
	free { stream.stop; stream = nil }
	cmdPeriod { stream.stop }
	
	playToBundle { arg bundle; 
		if(paused.not and: { stream.isPlaying.not }, {
			bundle.addAction(this, \play); //no latency (latency is in stream already)
		})
		^nil //return a nil object instead of a synth
	}
	
	build { arg proxy;
		stream = source.buildForProxy(proxy, channelOffset);
		clock = proxy.clock;
		paused = proxy.paused;
		^stream.notNil;
	}
	readyForPlay { ^stream.notNil }
	clear {
		stream = nil;
	}
	
}

SynthControl : AbstractPlayControl {
	var <server, <nodeID;
	var >canReleaseSynth=true, >canFreeSynth=true;
	
	
	writeDef { }
	
	sendDefToBundle {} //assumes that SynthDef is loaded in the server 
	name { ^source }
	clear { nodeID = nil }
	distributable { ^true }
	build { arg proxy;
		^proxy.initBus(proxy.rate ? \audio, proxy.numChannels ? 2);
	}
	spawnToBundle { arg bundle, extraArgs, target, addAction=0; //assumes self freeing
		var synthMsg;
		synthMsg = [9, this.name, -1, addAction, target.asTarget.nodeID]++extraArgs;
		bundle.add(synthMsg);
	}
	
	playToBundle { arg bundle, extraArgs, target, addAction=1;
		target = target.asTarget;
		server = target.server;
		nodeID = server.nextNodeID;
		bundle.add([9, this.name, nodeID, addAction, target.nodeID]++extraArgs);
		if(paused) { bundle.add(["/n_run", nodeID, 0]) };
		^nodeID
	}
	
	stopToBundle { arg bundle;
		if(nodeID.notNil, {
			if(this.canReleaseSynth, {
					bundle.add([15, nodeID, \gate, 0.0]); //to be sure.
			}, {
					if(this.canFreeSynth.not, { bundle.add([11, nodeID]) }); //"/n_free"
			});
			nodeID = nil;
		});
	}
	// pauseToBundle ?

	play { arg group, extraArgs;
		var bundle;
		bundle = MixedBundle.new;
		this.playToBundle(bundle, extraArgs, group);
		bundle.send(group.server)
	}
	stop { arg latency;
		var bundle;
		bundle = List.new;
		this.stopToBundle(bundle);
		server.listSendBundle(latency, bundle)
	}
			
	stopClientToBundle { }  // used in shared node proxy
	
	pause { arg clock, quant=1;
		this.run(clock, quant, false); 
	}
	resume { arg clock, quant=1.0;
		this.run(clock, quant, true);
	}
	run { arg clock, quant, flag=true;
		if(nodeID.notNil) { 
			(clock ? SystemClock).play({
				server.sendMsg("/n_run", nodeID, flag.binaryValue);
				paused = flag.not;
				nil;
			}, quant)
		} { paused = flag.not; }
	}
	
	canReleaseSynth { ^canReleaseSynth }
}


SynthDefControl : SynthControl {
	classvar <>writeDefs=true;
	var <synthDef;
	
	readyForPlay { ^synthDef.notNil }
	canReleaseSynth { ^synthDef.canReleaseSynth }
	canFreeSynth { ^synthDef.canFreeSynth }
	
	build { arg proxy, index=0; 
		var ok, rate, numChannels;
		synthDef = source.buildForProxy(proxy, channelOffset, index);
		rate = synthDef.rate ?? { if(proxy.rate !== \control) { \audio } { \control } };
		numChannels = synthDef.numChannels ? proxy.numChannels ? 2;
		ok = proxy.initBus(rate, numChannels);
		// [rate, numChannels].debug;
		if(ok && synthDef.notNil, { 
			paused = proxy.paused;
			// schedule to avoid hd sleep latency. 
			// def writing is only for server boot.
			if(writeDefs, {
				AppClock.sched(rrand(0.01, 0.3), { this.writeDef; nil });
			}); 
		}, { 
			synthDef = nil; 
		})
	}
	
	free {ÊsynthDef = nil }
	
	sendDefToBundle { arg bundle, server;
		bundle.addPrepare([5, synthDef.asBytes]); //"/d_recv"
	}
	

	writeDef {
		synthDef.writeDefFile;
	}
	
	name { ^synthDef.name }
	
	
}

SoundDefControl : SynthDefControl {
	
	writeDef { }
	build { synthDef = source.synthDef; ^true }
	sendDefToBundle {} //assumes that SoundDef does send to the same server 
}



CXPlayerControl : AbstractPlayControl {
	
	//here the lazy bus init happens and allocation of client side ressources
	//there is no bundle passing in build
	
	build { arg proxy;
		var bus, ok;
		if(proxy.isNeutral) {
			source.prepareForPlay; // first initialization for lazy rate detection
			ok = proxy.initBus(source.rate ? 'audio', source.numChannels ? 2);
			source.free;
			
		};
		bus = proxy.asBus;
		source.prepareForPlay(nil, true, bus);
		ok = source.readyForPlay;
		paused = proxy.paused;
		^ok
	}
	
	playToBundle { arg bundle, extraArgs, proxy, addAction;
		var bus;
		if(paused.not) {
			bus = proxy.asBus;
			// we'll need channel offset maybe.
			source.prepareToBundle(nil, bundle); //proxy.group
			if(source.readyForPlay.not) {
				source.makePatchOut(nil, true, bus, bundle)
			};
			source.spawnOnToBundle(nil, bus, bundle); //proxy.group
			// this.moveToGroupToBundle(bundle, proxy.group);
		};
		^nil
		
		
	}
	 
	stopToBundle { arg bundle, fadeTime=0.02;
		source.releaseToBundle(fadeTime, bundle);
		//bundle.addSchedFunction({ source.free }, 
	}
	
	freeToBundle {} // maybe should free after fadeTime
	
	stop {
		source.stop;
	}
	play {
		source.play;
	}
	
	free { 
		// source.stop;
	}
	pause { source.stop }
	resume { source.play }
	// moveToGroupToBundle {}
	
	isPlaying { ^false } // ^source.isPlaying }
	
}

CXSynthPlayerControl : CXPlayerControl {
		
	/*
	moveToGroupToBundle { arg bundle, group; 
		if(group.isPlaying) {
			bundle.add(["/g_tail", group.nodeID, source.children[0].synth.nodeID]);
		}
	}
	*/

}
