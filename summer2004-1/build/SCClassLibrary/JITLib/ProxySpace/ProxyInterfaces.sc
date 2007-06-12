

//lightweight objects that insulate different ways of playing/stopping.
//the bundle that is passed in is a MixedBundle
//these could be players in future, a superclass of AbstractPlayer would be required

AbstractPlayControl {
	var <source, <>channelOffset;
	var <paused=false;
	
	*new { arg source, channelOffset=0;
		^super.newCopyArgs(source, channelOffset);
	}
	
	build { ^true }
	pause { this.stop } 
	resume { this.start }
	nodeID { ^nil }
	
	readyForPlay { ^true }
	distributable {Ê^false } // shared proxy support

	loadToBundle {}
	spawnToBundle {} // only active in synthcontrols

	
	playToBundle { arg bundle, args; 
		bundle.addAction(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}
	
	stopToBundle { arg bundle;
		bundle.addAction(this, \stop);
	}
	
	freeToBundle {}
	
	controlNames { ^nil }	
	play { this.subclassResponsibility(thisMethod) }
	stop { this.subclassResponsibility(thisMethod) }
	
	
	wakeUpParentsToBundle {}
	addParent { "wrong object in NodeProxy.buildControl".error } // for now.
	parents { ^nil }
	
}

StreamControl : AbstractPlayControl {
	var stream, clock; // stream is a pause stream
	
	playToBundle { arg bundle;
		if(paused.not and: { stream.isPlaying.not }, {
			bundle.addAction(this, \play); //no latency (latency is in stream already)
		})
		^nil //return a nil object instead of a synth
	}
	
	build { arg proxy;
		clock = proxy.clock;
		paused = proxy.paused;
		stream = source.buildForProxy;
		^true;
	}
	
	pause { stream.pause; paused=true }
	resume { arg clock, quant=1.0; stream.resume(clock, quant); paused=false }
	
	readyForPlay { ^stream.notNil }

	play { stream.play(clock, false, 0.0) }
	stop { stream.stop }
	
}

PatternControl : StreamControl {
	var fadeTime, <array;
		
	playStream { arg str;
		var dt;
		dt = fadeTime.value;
		array = array.add(str);
		if(dt <= 0.02) 
			{ str.play(clock, false, 0.0) } 
			{ str.xplay(dt / clock.beatDur, clock, false, 0.0) };
	}
		
	stopStreams { arg streams;
		var dt;
		dt = fadeTime.value;
		if(dt <= 0.02) { 
			streams.do { arg item; item.stop  }; 
		} 
		{
			dt = dt / clock.beatDur;
			streams.do { arg item; item.xstop(dt) };
			// make sure it is stopped, in case next is never called
			SystemClock.sched(dt, { streams.do { arg item; item.stop; } });
		};
	}
		
	build { arg proxy;
		fadeTime = { proxy.fadeTime }; // needed for pattern xfade
		stream = source.buildForProxy(proxy, channelOffset); // this is needed for validity check 
		clock = proxy.clock ? TempoClock.default;
		paused = proxy.paused;
		^stream.notNil;
	}
	
	stopToBundle { arg bundle;
		var streams;
		streams = array.copy;
		array = nil;
		bundle.addFunction({ this.stopStreams(streams) });
	}
	
	pause { array.do { arg item; item.pause }; paused=true }
	resume { arg clock, quant=1.0;  
		array.do { arg item; item.resume(clock, quant) }; 
		paused=false 
	}
	
	playToBundle { arg bundle, args, proxy, addAction=1; // maybe use args to add them in Event?
		var event;
		if(paused.not and: { stream.isPlaying.not })
		{
			//no latency (latency is in stream already)
			bundle.addFunction({
				var str;
				str = source.buildForProxy(proxy, channelOffset);
				if(args.notNil) { 
					event = str.event;
					args.pairsDo { arg key, val; event[key] = val } 
				};
				this.playStream(str) 
			}); 		
		}
		^nil //return a nil object instead of a synth
	}
	
	stop { this.stopStreams(array.copy); array = nil; }
	
}

SynthControl : AbstractPlayControl {
	var <server, <>nodeID;
	var canReleaseSynth=true, canFreeSynth=true;
	
	
	loadToBundle {} //assumes that SynthDef is loaded in the server 
	name { ^source }
	distributable { ^canReleaseSynth } // n_free not implemented in shared node proxy
	
	build { arg proxy; // assumes audio, if proxy is not initialized
		var rate, desc;
		desc = this.synthDesc; // might be false assumption:
		if(desc.notNil) { canFreeSynth = canReleaseSynth = desc.hasGate }; 		if(proxy.isNeutral) { rate = \audio }
		^proxy.initBus(rate, proxy.numChannels ? 2);
	}
	
	spawnToBundle { arg bundle, extraArgs, target, addAction=0; //assumes self freeing
		var synthMsg;
		synthMsg = [9, this.name, -1, addAction, target.asTarget.nodeID]++extraArgs;
		bundle.add(synthMsg);
	}
	
	playToBundle { arg bundle, extraArgs, target, addAction=1;
		var group;
		server = target.server;
		group = target.asTarget;
		nodeID = server.nextNodeID;
		bundle.add([9, this.name, nodeID, addAction, group.nodeID]++extraArgs);
		if(paused) { bundle.add(["/n_run", nodeID, 0]) };
		^nodeID
	}
	
	stopToBundle { arg bundle;
		if(nodeID.notNil) {
			if(canReleaseSynth) {
					bundle.add([15, nodeID, \gate, 0.0]); //to be sure.
			} {
					if(canFreeSynth.not) { bundle.add([11, nodeID]) }; //"/n_free"
					// otherwise it is self freeing by some inner mechanism.
			};
			nodeID = nil;
		};
	}
	
	
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
	
	synthDesc { var dict;
		dict = SynthDescLib.global.synthDescs;
		^if(dict.notNil) { dict.at(source) } {Ênil }; // source is symbol: synth def name
	}
	controlNames { var desc; 
					desc = this.synthDesc; 
					^if(desc.notNil) { desc.controls } { nil } 
	}
	
}


SynthDefControl : SynthControl {
	classvar <>writeDefs=true;
	var <synthDef, <parents;
	
	readyForPlay { ^synthDef.notNil }
	

	build { arg proxy, index=0; 
		var ok, rate, numChannels;
		
		NodeProxy.buildProxyControl = this;
		synthDef = source.buildForProxy(proxy, channelOffset, index);
		NodeProxy.buildProxyControl = nil;
		
		rate = synthDef.rate ?? { if(proxy.rate !== \control) { \audio } { \control } };
		numChannels = synthDef.numChannels ? proxy.numChannels ? 2;
		ok = proxy.initBus(rate, numChannels);

		if(ok && synthDef.notNil, { 
			paused = proxy.paused;
			canReleaseSynth = synthDef.canReleaseSynth;
			canFreeSynth = synthDef.canFreeSynth;
			// schedule to avoid hd sleep latency. 
			// def writing is only for server boot.
			if(writeDefs, {
				AppClock.sched(rrand(0.01, 0.3), { synthDef.writeDefFile; nil });
			}); 
		}, { 
			synthDef = nil; 
		})
	}
	
	loadToBundle { arg bundle;
		bundle.addPrepare([5, synthDef.asBytes]); //"/d_recv"
	}
	
	name { ^synthDef.name }
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
		parents.do { arg proxy; proxy.wakeUpToBundle(bundle, checkedAlready) }
	}
	addParent { arg proxy;
		if(parents.isNil) { parents = IdentitySet.new };
		parents.add(proxy);
	}
	controlNames { ^synthDef.allControlNames }
}

