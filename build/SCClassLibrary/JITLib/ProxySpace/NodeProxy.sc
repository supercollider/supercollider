
NodeProxy : AbstractFunction {


	var <server, <group, <bus; 		//server access, the group that has the inner synths
	var <objects, <parents; 			//playing templates
	var <nodeMap, <>lags, <>prepend, <>clock; //lags might go into function annotation later

	var <loaded=false;					//synthDefs on server
	var <monitorGroup;					//the group that has the user output synth
	
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
		this.stop;
		if(bus.notNil, { bus.free });
		bus = nil;
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
		bus = Bus.new(rate, alloc, numChannels, server);
		busIndex = bus.index;
		//set both, because patterns may have both.
		nodeMap.set(\out, busIndex, \i_out, busIndex); 
	}
	
	fadeTime_ { arg t;
		this.set('#fadeTime', t);
	}
	
	
	// playing and access
	
	ar { arg numChannels;
		if(bus.isNil, { this.allocBus(\audio, numChannels ? 2) });
		^this.getOutput(numChannels)
	}
	
	kr { arg numChannels;
		if(bus.isNil, { this.allocBus(\control, numChannels ? 1) });
		^this.getOutput(numChannels)
	}
	
	value { arg something; 
		var n;
		n = something.numChannels;
		^if(something.rate == 'audio', {this.ar(n)}, {this.kr(n)})  
	}
	
	stop {
		monitorGroup.free;
		monitorGroup = nil;
	}	
		
	play { arg busIndex=0, nChan;
		var bundle, divider;
		if(server.serverRunning.not, { "server not running".inform; ^nil });
			bundle = MixedBundle.new;
			if(monitorGroup.isPlaying.not, { 
				monitorGroup = Group.newToBundle(bundle, server);
				NodeWatcher.register(monitorGroup);
			});
			if(bus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
			nChan = nChan ? this.numChannels;
			nChan = nChan.min(this.numChannels);
		
			this.wakeUpToBundle(bundle);
	
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do({ arg i;
			bundle.add([9, "proxyOut-linkDefAr-"++divider, 
					server.nextNodeID, 1, monitorGroup.nodeID,
					\i_busOut, busIndex + (i*divider), \i_busIn, bus.index + (i*divider)
					]);
			});
		
			this.sendBundle(bundle);
		^monitorGroup
	}
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		if(server.serverRunning.not, { "server not running".inform; ^nil });
		rec = RecNodeProxy.newFrom(this, numChannels);
		rec.open(path, headerFormat, sampleFormat);
		rec.record;
		^rec
	}
	
	
	///// access
		
	rate { ^bus.tryPerform(\rate) }
	numChannels { ^bus.tryPerform(\numChannels) }
	index { ^bus.tryPerform(\index) }
	
	isPlaying { 
		^group.isPlaying
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
					//container.writeDef;
					AppClock.sched(0, { container.writeDef; nil }); //so it is there on server reboot
					if(server.serverRunning, {
						if(this.isPlaying, {
							container.sendDefToBundle(bundle); 

						if(send, { 
							if(freeAll, {
								this.sendToServer(bundle,  nil, onCompletion)
							}, {
								this.sendLastToServer(bundle, nil, onCompletion)
							});
							
						}, { 
							bundle.sendPrepare(server, server.latency)
						});
							loaded = true;
						});
							loaded = false;
						
				 	}, {
				 		loaded = false;
				 	});
				},  { "rate/numChannels must match".inform })
			
	}
	
	

	//////////////behave like my bus/////////////
	
	gate { arg level=1.0, dur=0;
		if(bus.notNil && this.isPlaying, { bus.gate(level, dur, group) }, 
		{ "not playing".inform });
	}
	
	line { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { bus.line(level, dur, group) },
		{ "not playing".inform });
	}
	
	xline { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { bus.xline(level, dur, group) },
		{ "not playing".inform });
	}
	
	env { arg env;
		if(bus.notNil && this.isPlaying, { bus.env(env) },
		{ "not playing".inform });
	}
	
	setBus { arg ... values;
		if(bus.notNil && this.isPlaying, { 
			bus.performList(\set, values) 
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
		if(this.isPlaying, { group.performList(\set, args) });
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
		if(this.isPlaying, { 
			nodeMap.sendToNode(group);
		})
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
		if(selector === 'map', {
				nodeMap.mapToProxy(args);
			}, {
				nodeMap.performList(selector, args);
		});
		
		if(this.isPlaying, { 
			bundle = MixedBundle.new;
			bundle.preparationTime = 0;
			this.freeAllToBundle(bundle);
			
			this.sendEachToBundle(bundle);
			this.sendBundle(bundle);
		})
	}
	
	/////////////////////////////////////////////
	
	send { arg extraArgs;
			var bundle;
			bundle = MixedBundle.new.preparationTime_(0); //latency is 0, def is on server
			this.sendLastToServer(bundle, extraArgs);
	}
	
	sendAll { arg extraArgs;
			var bundle;
			bundle = MixedBundle.new.preparationTime_(0); //latency is 0, def is on server
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
				
				group = Group.basicNew(server);
				NodeWatcher.register(group);
				group.isPlaying = true; //force isPlaying.
				//add the message to the preparation part of the bundle
				bundle.add(group.newMsg(server, \addToHead));
	}
	
	
	sendAllToBundle { arg bundle, extraArgs;
				objects.do({ arg item;
						item.playToBundle(bundle, extraArgs, group);
				});
				if(objects.notEmpty, { nodeMap.addToBundle(bundle, group) });
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
	
	wakeUp { 
		var bundle, checkedAlready;
		bundle = MixedBundle.new;
		bundle.preparationTime = if(loaded, { 0 }, { 0.2 }); //synthdefs are on server?
		checkedAlready = Set.new;
		this.wakeUpToBundle(bundle, checkedAlready);
		this.sendBundle(bundle);
	}
	
		
	
	////// private /////
	
	
	initBus { arg rate, numChannels;
				if((rate === 'scalar') || rate.isNil, { ^true }); //this is no problem
				if(bus.isNil, {
					this.allocBus(rate, numChannels);
					^true
				}, {
					^(bus.rate === rate) && (numChannels <= bus.numChannels)
				});
	}


	getOutput { arg numChannels;
		var out, n, parentPlaying;
		
		parentPlaying = this.addToParentProxy;
		if(parentPlaying, { this.wakeUp });
		n = bus.numChannels;
		out = if(this.rate === 'audio', 
				{ InFeedback.ar( bus.index, n) },
				{ In.kr( bus.index, n ) }
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
		^(parentProxy.isPlaying || parentProxy.isNil);
	}
	
	wakeUpToBundle { arg bundle, checkedAlready;
		if(checkedAlready.isNil, { checkedAlready = Set.new });
		if(this.isPlaying.not && checkedAlready.includes(this).not, {
			checkedAlready.add(this); 
			this.loadToBundle(bundle);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			this.prepareForPlayToBundle(bundle);
			this.sendAllToBundle(bundle);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			parents.do({ arg item; item.wakeUpToBundle(bundle, checkedAlready) });
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
			
	
	loadToBundle { arg bundle;
		objects.do({ arg item;
			item.sendDefToBundle(bundle)
		});
		loaded = true; //remember to send
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

//do not add patterns for now.

SharedNodeProxy : NodeProxy {
	var <constantGroupID;
	
	*new { arg server, groupID;
		^super.newCopyArgs(server).initGroupID(groupID).clear	
	}
		
	initGroupID { arg groupID;  
		constantGroupID = groupID ?? {server.nextSharedNodeID}; 
	} 											
		
	prepareForPlayToBundle { arg bundle, freeAll=true;
				postln("started new shared group" + constantGroupID);
				group = Group.basicNew(server, constantGroupID);
				group.isPlaying = true; //force isPlaying.
				NodeWatcher.register(group);
				bundle.add(["/g_new", constantGroupID, 0, 0]);
	}
	
	generateUniqueName {
		^asString(constantGroupID) ++ objects.size
	}
	
	play { arg busIndex=0, nChan;
		var bundle, localBundle, localServer, divider;
		
		localServer = server.localServer;
		if(localServer.serverRunning.not, { "local server not running".inform; ^nil });
			bundle = MixedBundle.new;
			localBundle = MixedBundle.new;
			
			monitorGroup = Group.newToBundle(localBundle, localServer);
			if(bus.isNil, { this.allocBus(\audio, nChan ? 2) }); //assumes audio
			nChan = nChan ? this.numChannels;
			nChan = nChan.min(this.numChannels);
		
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do({ arg i;
			localBundle.add([9, "proxyOut-linkDefAr-"++divider, 
						localServer.nextNodeID, 1, monitorGroup.nodeID,
						\i_busOut, 	busIndex+(i*divider), 
						\i_busIn,	 	bus.index+(i*divider)
					]);
			});
			this.wakeUpToBundle(bundle);
			this.sendBundle(bundle);
			localBundle.send(localServer, localServer.latency);
		
		
		^monitorGroup
	}
	freeAllToBundle { arg bundle;
				objects.do({ arg item;
					item.stopClientToBundle(bundle);
				});
				
				if(group.notNil, {
					bundle.add(["/g_freeAll", group.nodeID]);
				})
	}

}





