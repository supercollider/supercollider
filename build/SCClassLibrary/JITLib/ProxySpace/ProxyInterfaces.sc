

//lightweight objects that insulate different ways of playing/stopping.
//the bundle that is passed in is a MixedBundle
//these could be players in future, a superclass of AbstractPlayer would be required

AbstractPlayControl {
	var <source, <>channelOffset;
	var <paused=false, <>parents;
	
	*new { arg source, channelOffset=0;
		^super.newCopyArgs(source, channelOffset);
	}
	
	
	writeDef {}
	prepareForPlay {}
	build { ^true }
	pause { this.stop } 
	resume { this.start }
	free {}
	nodeID { ^nil }
	
	readyForPlay { ^true }
	synthDef { ^nil }
	distributable {Ê^false } // shared proxy support

	loadToBundle {}
	spawnToBundle {}
	freeToBundle {}
	
	playToBundle { arg bundle, args; 
		bundle.addAction(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
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
	controlNames { ^[] }
	controlValues { ^[] }
	
	play { this.subclassResponsibility(thisMethod) }
	stop { this.subclassResponsibility(thisMethod) }
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
		parents.do { arg proxy; proxy.wakeUpToBundle(bundle, checkedAlready) }
	}
	addParent { arg proxy;
		if(parents.isNil) { parents = IdentitySet.new };
		parents.add(proxy);
	}
	
	
}

StreamControl : AbstractPlayControl {
	var <stream, clock, fadeTime;
		
	play { 
		var dt;
		stream.stop;
		dt = fadeTime.value;
		if(dt < 0.3) { stream.play(clock, false, 0) } { stream.xplay(dt, clock, false, 0) };
		CmdPeriod.add(this); // should maybe be in PauseStream
	}
	
	stop { 
		var dt;
		dt = fadeTime.value;
		if(dt < 0.3) { stream.stop } 
		{ 
			stream.xstop(dt);  
			// make sure it is stopped, in case next is never called
			SystemClock.sched(dt, { stream.stop }) 
		}; 
	}
	pause { stream.pause; paused=true }
	resume { arg clock, quant=1.0; stream.resume(clock, quant); paused=false }
	
	isPlaying { ^stream.isPlaying }
	//free { stream.stop; stream = nil }
	cmdPeriod { stream.stop }
	
	playToBundle { arg bundle; 
		if(paused.not and: { stream.isPlaying.not }, {
			bundle.addAction(this, \play); //no latency (latency is in stream already)
		})
		^nil //return a nil object instead of a synth
	}
	
	build { arg proxy;
		fadeTime = { proxy.fadeTime }; // needed for fadetime
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
	
	loadToBundle {} //assumes that SynthDef is loaded in the server 
	name { ^source }
	clear { nodeID = nil }
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
		if(nodeID.notNil, {
			if(this.canReleaseSynth, {
					bundle.add([15, nodeID, \gate, 0.0]); //to be sure.
			}, {
					if(this.canFreeSynth.not, { bundle.add([11, nodeID]) }); //"/n_free"
			});
			nodeID = nil;
		});
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
	isPlaying { ^nodeID.notNil }
	canReleaseSynth { ^canReleaseSynth } // maybe get this from synthdesclib
	synthDesc { var dict;
		dict = SynthDescLib.global.synthDescs;
		^if(dict.notNil) { dict.at(source) } {Ênil }; // source is symbol: synth def name
	}
	controlNames { var desc; desc = this.synthDesc; 
					^if(desc.notNil) { desc.controls.collect { |u| u.name } } { [] } 
	}
	controlValues {  var desc; desc = this.synthDesc; 
					^if(desc.notNil) { desc.controls.collect { |u| u.defaultValue } } { [] } 
	}
	
}


SynthDefControl : SynthControl {
	classvar <>writeDefs=true;
	var <synthDef;
	
	readyForPlay { ^synthDef.notNil }
	canReleaseSynth { ^synthDef.canReleaseSynth }
	canFreeSynth { ^synthDef.canFreeSynth }

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
	
	loadToBundle { arg bundle;
		bundle.addPrepare([5, synthDef.asBytes]); //"/d_recv"
	}
	

	writeDef {
		synthDef.writeDefFile;
	}
	
	name { ^synthDef.name }
	controlNames { ^source.def.argNames }
	controlValues { ^source.def.prototypeFrame }
	
	
}

