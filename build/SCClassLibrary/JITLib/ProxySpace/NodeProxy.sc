
NodeProxy : AbstractFunction {

	var <server, <group, <outbus; 		//server access
	var <objects, <parents; 			//playing templates
	
	var <nodeMap, <>lags, <>prepend, <>clock; //lags might go into function annotation later
	var <>freeSelf=true, <loaded=false;		
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
		SynthDef.writeOnce("proxyOut-linkDefAr-1", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 1)) 
		});
		SynthDef.writeOnce("proxyOut-linkDefAr-2", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 2)) 
		});
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
	}
	
	allocBus { arg rate=\audio, numChannels=2;
		//skip the first 32 channels. revisit.
		var alloc, busIndex;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil,{ "no channels left".error ^nil });
		outbus = Bus.new(rate, alloc + 32, numChannels, server);
		busIndex = outbus.index;
		//set both, because patterns may have both.
		nodeMap.set(\out, busIndex, \i_out, busIndex); 
	}
	
	fadeTime_ { arg t;
		this.set('#fadeTime', t);
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
	
		
		
	play { arg busIndex=0, nChan;
		var playGroup, bundle, divider, checkedAlready;
		if(server.serverRunning, {
			bundle = MixedBundle.new;
			playGroup = Group.newToBundle(bundle, server);
			if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
			nChan = nChan ? this.numChannels;
			nChan = nChan.min(this.numChannels);
		
			checkedAlready = Set.new;
			this.wakeUpToBundle(bundle, checkedAlready);
	
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do({ arg i;
			bundle.add([9, "proxyOut-linkDefAr-"++divider, 
					server.nextNodeID, 1,playGroup.nodeID,
					\i_busOut, busIndex+(i*divider), \i_busIn, outbus.index+(i*divider)]);
			});
		
			this.sendBundle(bundle);
		}, { "server not running".inform });
		
		^playGroup
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
	numChannels { ^outbus.tryPerform(\numChannels) }
	index { ^outbus.tryPerform(\index) }
	
	isPlaying { 
		^group.isPlaying
		//^server.nodeIsPlaying(group.asNodeID)
	}
	
	printOn { arg stream;
		stream << this.class.name << "." << this.rate << "(" << server << ", " << this.numChannels <<")";
	}
	
	generateUniqueName {
		^asString(this.identityHash.abs) ++ objects.size
	}
	
	
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0, send=true;
		this.put(obj, channelOffset, send, false)
	}
	
	source_ { arg obj; 
		this.put(obj, 0, true) 
	}
	
	removeLast { 
		this.removeAt(objects.size - 1); 
	}
	
	removeAt { arg index;
		if(index < objects.size, { 
			objects.removeAt(index).stop
		}, { "index out of range.".inform });
	}
	
	put { arg obj, channelOffset=0, send=false, freeAll=true, onCompletion; 			var container, bundle;
				bundle = MixedBundle.new;
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
					
					container.writeDef; //so it is there on server reboot
					if(server.serverRunning, {
						container.sendDefToBundle(bundle);
						if(send, { 
							if(freeAll, {
								this.sendToServer(bundle,  nil, onCompletion)
							}, {
								this.sendLastToServer(bundle, nil, onCompletion)
							})
						}, { 
							bundle.sendPrepare(server, server.latency)
						});
						loaded = true;
				 	}, {
				 		loaded = false;
				 	});
				},  { "rate/numChannels must match".inform })
			
	}
	
	

	//////////////behave like my bus/////////////
	
	gate { arg level=1.0, dur=0;
		if(outbus.notNil && this.isPlaying, { outbus.gate(level, dur, group) }, 
		{ "not playing".inform });
	}
	
	line { arg level=1.0, dur=1.0;
		if(outbus.notNil && this.isPlaying, { outbus.line(level, dur, group) },
		{ "not playing".inform });
	}
	
	xline { arg level=1.0, dur=1.0;
		if(outbus.notNil && this.isPlaying, { outbus.xline(level, dur, group) },
		{ "not playing".inform });
	}
	
	env { arg env;
		if(outbus.notNil && this.isPlaying, { outbus.env(env) },
		{ "not playing".inform });
	}
	
	setBus { arg ... values;
		if(outbus.notNil && this.isPlaying, { 
			outbus.performList(\set, values) 
		},
		{ "not playing".inform });
	}

	
	////////////behave like my group////////////
	
	free {
		if(this.isPlaying, { group.free });
	}
	
	freeAll {
		if(this.isPlaying, { group.freeAll });
	}
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying, { group.performList(\set, args) }, { "not playing".inform });
	}
	//to test
	setn { arg ... args;
		nodeMap.performList(\setn, args);
		if(this.isPlaying, { group.performList(\setn, args) });
	}
	
	//map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		nodeMap.mapToProxy(args);
		if(this.isPlaying, { nodeMap.sendToNode(group) })
	}
	
		
	unset { arg ... keys;
		nodeMap.performList(\unset, keys);
		if(this.isPlaying, { nodeMap.sendToNode(group) });
	}
	
	unmap { arg ... keys;
		nodeMap.unmapProxy(keys);
		if(this.isPlaying, {
			keys.do({ arg key; group.map(key,-1) }); 
			nodeMap.sendToNode(group) 
		});
	}
	
	release {
		var bundle;
		bundle = MixedBundle.new;
		if(this.isPlaying, { 
			this.freeAllToBundle(bundle);
			bundle.send(server);
		});
	}
	
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	pause { this.run(false) }
	
	//xfades
	
	xset { arg ... args;
		this.xFadePerform(\set, args);
	}
	xmap { arg ... args;
		this.xFadePerform(\map, args);
	}
	xsetn { arg ... args;
		this.xFadePerform(\setn, args);
	}
	xFadePerform { arg selector, args;
		var bundle;
		if(this.isPlaying, { 
			bundle = MixedBundle.new;
			bundle.preparationTime = 0;
			this.freeAllToBundle(bundle);
			if(selector === 'map', {
				nodeMap.mapToProxy(args);
			}, {
				nodeMap.performList(selector, args);
			});
			this.sendEachToBundle(bundle);
			this.sendBundle(bundle);
		}, {
			nodeMap.performList(selector, args);
		})
	}
	
	/////////////////////////////////////////////
	
	send { arg extraArgs;
			//latency is 0, def is on server
			var bundle;
			bundle = MixedBundle.new.preparationTime_(0);
			this.sendLastToServer(bundle, extraArgs);
	}
	
	sendAll { arg extraArgs;
			//latency is 0, def is on server
			var bundle;
			bundle = MixedBundle.new.preparationTime_(0);
			this.sendToServer(bundle, extraArgs);
	}
	
	
	refresh {
		var bundle;
		bundle = MixedBundle.new;
		bundle.preparationTime = 0;
		this.freeAllToBundle(bundle);
		this.sendAllToBundle(bundle);
		this.sendBundle(bundle);
	}
	

	
	// server communications, updating
	
	sendToServer { arg bundle, extraArgs, onCompletion;
				if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle) });
				this.sendAllToBundle(bundle, extraArgs);
				this.sendBundle(bundle, onCompletion);
	}
	
	sendLastToServer { arg bundle, extraArgs, onCompletion;
				if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle) });
				this.sendLastToBundle(bundle, extraArgs);
				this.sendBundle(bundle, onCompletion);
	}
	
	sendBundle { arg bundle, onCompletion;
			bundle.schedSend(server, clock, onCompletion)
	}
			
	prepareForPlayToBundle { arg bundle;
				
				group = Group.prNew(server);
				NodeWatcher.register(group);
				group.isPlaying = true; //force isPlaying.
				//add the message to the preparation part of the bundle
				bundle.addPrepare(group.newMsg(server,\addToHead));
	}
	
	
	sendAllToBundle { arg bundle, extraArgs;
				objects.do({ arg item;
						item.playToBundle(bundle, extraArgs, group);
				});
				nodeMap.addToBundle(bundle, group);
	}
	
	sendLastToBundle { arg bundle, extraArgs;
				var synth;
				synth = objects.last.playToBundle(bundle, extraArgs, group);
				if(synth.notNil, { nodeMap.addToBundle(bundle, synth) });
	}

	sendEachToBundle { arg bundle, extraArgs;
				var synth;
				objects.do({ arg item;
					synth = item.playToBundle(bundle, extraArgs, group).postln;
					if(synth.notNil, {
							nodeMap.addToBundle(bundle, synth.nodeID);
					});
				});
	}
	
	freeAllToBundle { arg bundle;
				objects.do({ arg item;
					item.stopToBundle(bundle);
				});
	}
	
	freeParentsToBundle {
		// todo maybe
	}
	
	loadToBundle { arg bundle;
		if(loaded == false, {
			this.sendAllDefsToBundle(bundle);
			loaded = true; 
		});
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
		this.wakeUp(0.3);
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
	
	wakeUpToBundle { arg bundle, checkedAlready; //no need to wait, def is on server
		if(this.isPlaying.not && checkedAlready.includes(this).not, {
			checkedAlready.add(this); 
			//this.loadToBundle(bundle);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			this.prepareForPlayToBundle(bundle);
			this.sendAllToBundle(bundle);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			//if(loaded.not, { this.loadAll; });
			parents.do({ arg item; item.wakeUpToBundle(bundle, checkedAlready) });
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
	
	wakeUp { arg latency=0.0; //see for load 
		var bundle, checkedAlready;
		bundle = MixedBundle.new;
		bundle.preparationTime = 0; //synthdefs are on server
		checkedAlready = Set.new;
		this.wakeUpToBundle(bundle, checkedAlready);
		this.sendBundle(bundle);
	}
			
	
	sendAllDefsToBundle { arg bundle;
		objects.do({ arg item;
			item.sendDefToBundle(bundle)
		});
		loaded = true; //remember to send;
	}	
	
	
}



Ndef : NodeProxy {
	var key;
	*new { arg key, object, server;
		var res;
		server = server ? Server.local;
		res = this.at(server, key);
		if(res.isNil, {
			res = super.new(server).toLib(key);
		});
		if(object.notNil, { res.source = object });
		^res;
	}
	toLib { arg key;
		Library.put(this.class, server, key, this);
	}
	*at { arg server, key;
		^Library.at(this, server, key)
	}
}

//the server needs to be a Broadcast.
//this class takes care for a constant groupID.


SharedNodeProxy : NodeProxy {
	var <constantGroupID;
	
	*new { arg server;
		^super.newCopyArgs(server).initGroupID.clear	
	}
		
	initGroupID {   
		constantGroupID = server.nextSharedNodeID; 
	} 											
		
	prepareForPlayToBundle { arg bundle, freeAll=true;
				postln("started new shared group" + constantGroupID);
				group = Group.basicNew(server, constantGroupID);
				group.isPlaying = true; //force isPlaying.
				NodeWatcher.register(group);
				bundle.add(["/g_new",constantGroupID,0,0]);
	}
	generateUniqueName {
		^asString(constantGroupID) ++ objects.size
	}
	
	play { arg busIndex=0, nChan;
		var playGroup, bundle, divider, checkedAlready, localServer;
		
		localServer = server.localServer;
		if(localServer.serverRunning, {
			bundle = MixedBundle.new;
			playGroup = Group.newToBundle(bundle, localServer);
			if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
			nChan = nChan ? this.numChannels;
			nChan = nChan.min(this.numChannels);
			checkedAlready = Set.new;
			this.wakeUpToBundle(bundle, checkedAlready);
		
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do({ arg i;
			bundle.add([9, "proxyOut-linkDefAr-"++divider, 
						localServer.nextNodeID, 1,playGroup.nodeID,
						\i_busOut, busIndex+(i*divider), \i_busIn, outbus.index+(i*divider)]);
			});
			
			
				bundle.sendPrepare(server, server.latency);
				bundle.send(localServer, localServer.latency);
		}, { "local server not running".inform });
		
		^playGroup
	}

}





