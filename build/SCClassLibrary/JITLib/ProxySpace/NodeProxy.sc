
NodeProxy : AbstractFunction {

	var <server, <group, <outbus;
	var <objects, <loaded=false, <parents; //loading will be revisited
	
	var <nodeMap, <>clock, <>freeSelf=true;
	classvar <>buildProxy;
	
	*new { arg server;
		^super.newCopyArgs(server ? Server.local).clear	}
	*audio { arg server, numChannels=2;
		^this.new(server).allocBus(\audio, numChannels);
	}
	*control { arg server, numChannels=1;
		^this.new(server).allocBus(\control, numChannels);
	}
		
	*initClass {
		SynthDef("proxyOut-linkDefAr-1", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 1)) 
		}).writeDefFile;
		SynthDef("proxyOut-linkDefAr-2", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 2)) 
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
		if(server.nodeWatcher.isWatching.not, { server.nodeWatcher.start }); //to be sure.
	}

	initParents {
		parents = IdentitySet.new;
		if(nodeMap.notNil, { //keep the mapped parents
			nodeMap.mappings.do({ arg proxy; parents.add(proxy) });
		});
	}
	
	allocBus { arg rate=\audio, numChannels=2;
		//skip the first 32 channels. maybe use special server later. revisit.
		var alloc;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil,{ "no channels left".error ^nil });
		outbus = Bus.new(rate, alloc + 32, numChannels, server);
	}
	
	fadeTime_ { arg t;
		this.set('__synthFadeTime__', t);
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
	
	value { arg something; 
		var n;
		n = something.numChannels;
		^if(something.rate == 'audio', {this.ar(n)}, {this.kr(n)})  
	}
	
	
	nodeMap_ { arg map;
		nodeMap = map;
		if(this.isPlaying, {
			map.sendToNode(group);
		});
	}
	
		
	play { arg busIndex=0, nChan;
		var playGroup, bundle, divider;
		if(server.serverRunning.not, { "server not running".inform; ^nil });
		if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
		
		bundle = MixedBundle(server);
		playGroup = Group.newToBundle(bundle, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		this.wakeUpParentsToBundle(bundle);
		divider = if(nChan.even, 2, 1);
		(nChan div: divider).do({ arg i;
				bundle.add([9, "proxyOut-linkDefAr-"++divider, 
					server.nextNodeID, 1,playGroup.nodeID,
					\i_busOut, busIndex+(i*divider), \i_busIn, outbus.index+(i*divider)]);
		});
		
		bundle.schedSend(0,clock);
		^playGroup
	}
	
	send { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(MixedBundle(server), false, 0.0, extraArgs);
	}
	sendAll { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(MixedBundle(server), true, 0.0, extraArgs);
	}
	
	refresh {
		var bundle;
		bundle = MixedBundle(server);
		this.sendSynthToBundle(bundle, true);
		bundle.schedSend(0,clock)
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
	
	add { arg obj, channelOffset=0, send=true;
		this.put(obj, 0, send, false)
	}
	
	source_ { arg obj; 
		this.put(obj, 0, true) 
	}
	
	put { arg obj, channelOffset=0, send=false, freeAll=true, onCompletion, latency=0.3; 			var container, bundle;
				bundle = MixedBundle(server);
				if(freeAll, { 
					this.freeAllToBundle(bundle);
					objects = Array.new;
					this.initParents 
				});
				
				this.class.buildProxy = this;
				container = obj.wrapForNodeProxy(this,channelOffset);
				this.class.buildProxy = nil;
				
				if(container.notNil, {
					objects = objects.add(container);
					
					container.writeDef;
					if(server.serverRunning, {
						container.sendDefToBundle(bundle);
						if(send, { 
							this.sendToServer(bundle, freeAll, latency, nil, onCompletion);
						}, { 
							bundle.sendPrepare 
						});
						loaded = true;
				 	});
				},  { "rate/numChannels must match".inform })
			
	}
	
	load {
		var bundle;
		if(server.serverRunning, { 
			bundle = MixedBundle(server);
			this.loadToBundle(bundle);
			bundle.send;
			loaded = true; 
		});
	
	}
		
	loadAll {
		var bundle;
		bundle = MixedBundle(server);
		if(server.serverRunning, {
			parents.do({ arg proxy; 
				proxy.sendAllDefsToBundle(bundle); 
			});
			this.sendAllDefsToBundle(bundle);
			bundle.send;
			loaded = true; 
		}, { "server not running".inform });
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
	map { arg key, proxy;
		var args;
		if(proxy.outbus.isNil, { proxy.allocBus(\control,1) });
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
	release {
		var bundle;
		bundle = MixedBundle(server);
		if(this.isPlaying, { 
			this.freeAllToBundle(bundle);
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
				if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle, freeAll) });
				this.sendSynthToBundle(bundle, freeAll, extraArgs);
				bundle.schedSend(latency, clock, latency.isNil, onCompletion)
		});
	}
			
	//here happens the dangerous thing. if isPlaying is not assured properly, 
	//we might crash iteratively during wakeUpParents
	
	prepareForPlayToBundle { arg bundle, freeAll=true;
				//group = Group.newToBundle(bundle, server, \addToHead);
				group = Group.prNew(server);
				//add the message to the preparation part
				bundle.addPrepare(group.newMsg(server,\addToHead)); 
				server.nodeWatcher.nodes.add(group.nodeID); //force isPlaying.
	}
	
	sendSynthToBundle { arg bundle, freeAll=true, extraArgs;
				var synth;
				extraArgs = [\outIndex, outbus.index]++extraArgs;
				if(freeAll, {
					objects.do({ arg item;
						item.playToBundle(bundle, extraArgs, group);
					});
					nodeMap.addToBundle(bundle, group);
				}, {
						synth = objects.last.playToBundle(bundle, extraArgs, group);
						if(synth.notNil, {
							nodeMap.addToBundle(bundle, synth);
						});
				});
					
	}
	
	freeAllToBundle { arg bundle;
					objects.do({ arg item;
						item.stopToBundle(bundle);
					});
					if(this.isPlaying, {
							//if the object has its own envelope, try to free it, too.
							group.msgToBundle(bundle, 15, [\gate, 0.0, '__synthGate__', 0.0]);
					})
					
	}
	
	freeParentsToBundle {
		// todo maybe
	}
	
	////// private /////
	
	
	initBus { arg rate, numChannels;
				if((rate === 'scalar') || rate.isNil, { ^true }); //this is no problem
				if(outbus.isNil, {
					this.allocBus(rate,numChannels);
					^true
				}, {
					^(outbus.rate === rate) && (numChannels <= outbus.numChannels)
				});
	}


	getOutput { arg numChannels;
		var out, n;
		
		this.addToParentProxy;
		this.wakeUpParents(0.3);
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

	addToParentProxy {
		var parentProxy;
		parentProxy = this.class.buildProxy;
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
	}
	
	wakeUpToBundle { arg bundle; //no need to wait, def is on server
		if(this.isPlaying.not, { 
			this.prepareForPlayToBundle(bundle, true);
			this.sendSynthToBundle(bundle, true, 0);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle; //see for objects
		if(this.isPlaying.not, {
			//if(loaded.not, { this.loadAll; });
			this.wakeUpToBundle(bundle);
			parents.do({ arg item; item.wakeUpParentsToBundle(bundle) });
			
		});
	}
	
	wakeUpParents { arg latency=0.0; //see for load 
		var bundle;
		bundle = MixedBundle(server);
		this.wakeUpParentsToBundle(bundle);
		bundle.schedSend(latency,clock,latency.isNil)
	}
			
	
	sendAllDefsToBundle { arg bundle;
		objects.do({ arg item;
			item.sendDefToBundle(bundle)
		});
		loaded = true; //remember to send;
	}	
	
	
}