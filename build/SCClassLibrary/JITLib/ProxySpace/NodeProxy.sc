
NodeProxy : AbstractFunction {

	var <server, <group, <outbus;
	var <objects, <loaded=false, <parents;
	
	var <nodeMap, <>clock, <>freeSelf=true;
	
	
	*new { arg server;
		^super.newCopyArgs(server).ninit(server)	
	}
	*audio { arg server, numChannels=2;
		^this.new(server).allocBus(\audio, numChannels);
	}
	*control { arg server, numChannels=1;
		^this.new(server).allocBus(\control, numChannels);
	}
	
	ninit { arg  argServer, argNumChannels;
		server = argServer ? Server.local;
		this.clear;
	}
	
	*initClass {
		SynthDef("proxyOut-linkDefAr", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 1)) 
		}).writeDefFile;
		
	}
	
	clear {
		nodeMap = ProxyNodeMap.new;
		objects = Array.new;
		this.initParents;
		this.free;
		if(outbus.notNil, { outbus.free });
		outbus = nil;
		group = nil;
	}

	initParents {
		parents = IdentitySet.new;
		if(nodeMap.notNil, { //keep the mapped parents
			nodeMap.mappings.do({ arg proxy; parents.add(proxy) });
		});
	}
	
	allocBus { arg rate=\audio, numChannels=2;
		outbus = Bus.perform(rate, server, numChannels);
	}
	
	fadeTime_ { arg t;
		this.set(\synthFadeTime, t);
	}
	
	// playing and access
	
	ar { arg numChannels;
		if(outbus.isNil, { this.allocBus(\audio, numChannels ? 2) });
		^this.getOutput(numChannels)
	}
	
	kr { arg numChannels;
		if(outbus.isNil, { this.allocBus(\control, numChannels ? 1) });
		^this.getOutput(numChannels)
	}
	
	value { ^if(this.rate == 'audio', {this.ar}, {this.kr})  }
	
	
	
	nodeMap_ { arg map;
		nodeMap = map;
		map.sendToNode(group);
	}
	
		
	play { arg busIndex=0, nChan;
		var playGroup, bundle;
		if(server.serverRunning.not, { "server not running".inform; ^nil });
		if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
		
		bundle = MixedBundle(server);
		playGroup = Group.newToBundle(bundle, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		this.wakeUpParentsToBundle(bundle);
		
		nChan.do({ arg i;
			bundle.add([9, "proxyOut-linkDefAr", server.nextNodeID, 1,playGroup.nodeID,
				\i_busOut, busIndex+i, \i_busIn, outbus.index+i]);
		});
		
		this.schedSendOSC(bundle)
		^playGroup
	}
	
	send { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(nil, false, 0.0, extraArgs);
	}
	sendAll { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(nil, true, 0.0, extraArgs);
	}
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		rec = RecNodeProxy.newFrom(this, numChannels, {
					//rec.record(path, headerFormat, sampleFormat);
		});
		
		Routine({
			1.0.wait;//seemingly must wait.
			rec.record(path, headerFormat, sampleFormat);
		}).play;
		
		^rec
	}
	
	refresh {
		var bundle;
		bundle = MixedBundle(server);
		this.sendSynthMsg(bundle, true);
		this.schedSendOSC(bundle)
	}
	
	rate { ^outbus.tryPerform(\rate) }
	
	numChannels {
		^outbus.tryPerform(\numChannels);
	}
	
	isPlaying { 
		//^(group.notNil and: {group.isPlaying})
		^server.nodeIsPlaying(group.asNodeID)
	}
	
	printOn { arg stream;
		stream << "NodeProxy." << this.rate << "(" << server << ", " << this.numChannels <<")";
	}
	
	generateUniqueName {
		^asString(this.identityHash.abs) ++ objects.size
	}
	
	// setting the source to anything that returns a valid ugen input
	
	add { arg obj, send=true;
		this.setObj(obj, send, false)
	}
	
	source_ { arg obj; 
		this.setObj(obj, true) 
	}
	
	setObj { arg obj, send=false, freeAll=true, onCompletion, latency=0.3; 			var container, bundle;
				bundle = MixedBundle(server);
				if(freeAll, { 
					this.freeAllMsg(bundle);
					objects = Array.new;
					this.initParents 
				});
				
				container = obj.wrapForNodeProxy(this);
				if(container.notNil, {
					objects = objects.add(container);
					if(server.serverRunning, {
						container.sendDef(server);
						if(send, { 
							this.prepareForPlayMsg(bundle, freeAll);
							this.sendToServer(bundle, freeAll, latency, nil, onCompletion) 
						});
				 	});
				},  { "rate/numChannels must match".inform })
			
	}
	
	
	
	
	////////////behave like my group////////////
	
	free {
		if(this.isPlaying, { group.free });
	}
	
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying, { group.performList(\set, args) }, { "not playing".inform });
	}
	
	setn { arg ... args;
		nodeMap.performList(\setn, args);
		if(this.isPlaying, { group.performList(\setn, args) });
	}
	
	//map to a control proxy
	map { arg key, proxy; // ... args; //...args maybe doesn't work. test.
		var args;
		if(proxy.rate === 'control', { 
			args = [key, proxy]; //++args;
			nodeMap.performList(\map, args);
			parents.add(proxy);
			if(this.isPlaying, { nodeMap.sendToNode(group) })
		}, {
			"can only map to control bus".inform
		})
	}
	
	unset { arg ... args;
		nodeMap.performList(\unset, args);
	}
	
	unmap { arg ... args;
		nodeMap.performList(\unmap, args);
		if(this.isPlaying, { nodeMap.sendToNode(group) });
	}
	release { arg latency;
		var bundle;
		bundle = MixedBundle(server);
		if(this.isPlaying, { 
			this.freeAllMsg(bundle);
			bundle.send;
		});
	}
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	/////////////////////////////////////////////
	
	
	// server communications, updating
	
	sendToServer { arg bundle, freeAll=true, latency=0.3, extraArgs, onCompletion;
		var resp;
		
		if(server.serverRunning, {
				bundle = bundle ? MixedBundle(server);
				
				this.sendSynthMsg(bundle, freeAll, extraArgs);
				
				if(latency.notNil, {
					SystemClock.sched(latency, {
								this.schedSendOSC(bundle, onCompletion); 
					})
				}, {
					resp = OSCresponder(server.addr, '/done', { 
						this.schedSendOSC(bundle, onCompletion);
						resp.remove;
					}).add;
				
				});
		});
	}
			
	schedSendOSC { arg bundle, onCompletion;
					//bundle.asCompileString.postln;
					if(clock.notNil, {
						clock.sched(0, { 
							bundle.send; 
							onCompletion.value(this) 
						})
					}, {
						bundle.send;
						onCompletion.value(this)
					})
	}
	
	prepareForPlayMsg { arg bundle, freeAll=true;
				var watcher;
					if(this.isPlaying.not, {
						watcher = server.nodeWatcher;
						group = Group.newToBundle(bundle, server, \addToHead);
						group.prIsPlaying(true);
						//RootNode(server).prAddHead(group);//manually link the node
						watcher.nodes.add(group); //force isPlaying						if(watcher.isWatching.not, { watcher.start }); //to be sure.
						})
	}
	
	sendSynthMsg { arg bundle, freeAll=true, extraArgs;
				var synth;
				if(freeAll, {
					objects.do({ arg item;
						item.playToBundle(bundle, extraArgs, group);
					});
					nodeMap.updateMsg(bundle, group);
				}, {
						synth = objects.last.playToBundle(bundle, extraArgs, group);
						if(synth.notNil, {nodeMap.updateMsg(bundle, synth)});
				});
					
				
	}
	
	
	freeAllMsg { arg bundle;
					objects.do({ arg item;
						item.stopClientProcessToBundle(bundle);
					});
					if(this.isPlaying, {
						if(freeSelf, {
							group.msgToBundle(bundle, 15, [\synthGate, 0.0]); //n_set
						}, {
							//if the object has its own envelope, try to free it.
							group.msgToBundle(bundle, 15, [\gate, 0.0]); //n_set 
						});
					})

	}
	
	
		
	////// private /////
	
	
	initBus { arg rate, numChannels;
				if((rate === 'scalar') || rate.isNil, { ^true }); //just exit
				if(outbus.isNil, {
					this.allocBus(rate,numChannels);
					^true
				}, {
					^(outbus.rate === rate) && (numChannels <= outbus.numChannels)
				});
	}

	getOutput { arg numChannels;
		var out, n;
		
		this.addToBuildSynthDef;
		this.wakeUpParents;
		n = outbus.numChannels;
		out = if(this.rate === 'audio', 
				{ InFeedback.ar( outbus.index, n) },
				{ In.kr( outbus.index, n) }
			);
	
		//test that
		/*
		if(numChannels != n, {
			out = NumChannels.ar(out, numChannels, false)
		}); 
		*/
		^out
		
		
	}
	
	addToBuildSynthDef {
		var parentProxy;
		parentProxy = UGen.buildSynthDef.tryPerform(\proxy);
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
	}
	
	wakeUpToBundle { arg bundle; //no need to wait, def is on server
		if(this.isPlaying.not, { 
			this.prepareForPlayMsg(bundle, true);
			this.sendSynthMsg(bundle, true, 0);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle; //see for objects
		
		if(this.isPlaying.not, {
			if(loaded.not, { this.loadAll });
			parents.do({ arg item; item.wakeUpParentsToBundle(bundle) });
			this.wakeUpToBundle(bundle);
		});
	}
	
	wakeUpParents { 
		var bundle;
		bundle = MixedBundle(server);
		this.wakeUpParentsToBundle(bundle);
		this.schedSendOSC(bundle);
	}
	
	load {
		var bundle;
		if(server.serverRunning, { 
			bundle = MixedBundle(server);
			this.loadMsg(bundle);
			bundle.send; 
		});
	
	}
		
	loadAll {
		var bundle;
		bundle = MixedBundle(server);
		if(server.serverRunning, {
			parents.do({ arg proxy; 
				proxy.sendAllDefsMsg(bundle); 
			});
			this.sendAllDefsMsg(bundle);
			bundle.send; 
		}, { "server not running".inform });
	}
		
	
	sendAllDefsMsg { arg bundle;
		objects.do({ arg item;
			item.sendDefToBundle(bundle)
		});
		loaded = true; //remember to send;
	}	
	
	
}