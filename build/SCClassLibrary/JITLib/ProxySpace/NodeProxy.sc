
NodeProxy : AbstractFunction {

	var <server, <group, <outbus; 		//server access
	var <objects, <parents; 			//playing templates
	
	var <nodeMap, <>lags=#[], <>prepend, <>clock;
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
		//skip the first 32 channels. maybe use special server later. revisit.
		var alloc;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil,{ "no channels left".error ^nil });
		outbus = Bus.new(rate, alloc + 32, numChannels, server);
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
		bundle = MixedBundle.new;
		playGroup = Group.newToBundle(bundle, server);
		if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		if(parents.notEmpty, {
			checkedAlready = Set.new;
			this.wakeUpParentsToBundle(bundle, checkedAlready);
		});
		divider = if(nChan.even, 2, 1);
		(nChan div: divider).do({ arg i;
		bundle.add([9, "proxyOut-linkDefAr-"++divider, 
					server.nextNodeID, 1,playGroup.nodeID,
					\i_busOut, busIndex+(i*divider), \i_busIn, outbus.index+(i*divider)]);
		});
		
		server.waitForBoot({
			this.sendBundle(bundle,0,clock);
		});
		^playGroup
	}
	
	send { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(MixedBundle.new, false, 0.0, extraArgs);
	}
	
	sendAll { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(MixedBundle.new, true, 0.0, extraArgs);
	}
	
	
	refresh {
		var bundle;
		bundle = MixedBundle.new;
		this.freeAllToBundle(bundle);
		this.sendSynthToBundle(bundle, true);
		this.sendBundle(bundle,0,clock);
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
	
	asInstr {
		^{ this.value }.asInstr
	}
	//asDefName { ^this.generateUniqueName }
	
	// setting the source to anything that returns a valid ugen input
	
	add { arg obj, channelOffset=0, send=true;
		this.put(obj, channelOffset, send, false)
	}
	
	source_ { arg obj; 
		this.put(obj, 0, true) 
	}
	
	put { arg obj, channelOffset=0, send=false, freeAll=true, onCompletion, latency=0.3; 			var container, bundle;
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
			bundle = MixedBundle.new;
			this.sendAllDefsToBundle(bundle);
			bundle.send(server);
			loaded = true; 
		});
	
	}
		
	loadAll {
		var bundle;
		bundle = MixedBundle.new;
		if(server.serverRunning, {
			parents.do({ arg proxy; 
				proxy.sendAllDefsToBundle(bundle); 
			});
			this.sendAllDefsToBundle(bundle);
			bundle.send;
			loaded = true; 
		}, { "server not running".inform });
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
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		nodeMap.mapToProxy(args);
		if(this.isPlaying, { nodeMap.sendToNode(group) })
	}
	
		
	unset { arg ... keys;
		nodeMap.performList(\unset, keys);
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
	
	/////////////////////////////////////////////
	
	
	// server communications, updating
	
	sendToServer { arg bundle, freeAll=true, latency=0.3, extraArgs, onCompletion;
		
				if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle, freeAll) });
				this.sendSynthToBundle(bundle, freeAll, extraArgs);
				this.sendBundle(bundle, latency=0.3, clock, onCompletion);
		
	}
	
	sendBundle { arg bundle, latency=0.3, clock, onCompletion;
				if(latency.isNil, {
					bundle.schedSendRespond(server, 0, clock, onCompletion)
				},{
					bundle.schedSend(server, latency, clock, onCompletion)
				})
	}
			
	prepareForPlayToBundle { arg bundle, freeAll=true;
				//group = Group.basicNew(server.nextStaticNodeID, server);
				group = Group.prNew(server);
				NodeWatcher.register(group);
				group.isPlaying = true; //force isPlaying.
				//add the message to the preparation part
				bundle.addPrepare(group.newMsg(server,\addToHead)); 
	}
	
	sendSynthToBundle { arg bundle, sendAll=true, extraArgs;
				var synth;
				extraArgs = [\out, outbus.index]++extraArgs;
				if(sendAll, {
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
	
	wakeUpToBundle { arg bundle, checkedAlready; //no need to wait, def is on server
		if(this.isPlaying.not && checkedAlready.includes(this).not, { 
			this.prepareForPlayToBundle(bundle, true);
			this.sendSynthToBundle(bundle, true, 0);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
		if(this.isPlaying.not && checkedAlready.includes(this).not, {
			//if(loaded.not, { this.loadAll; });
			this.wakeUpToBundle(bundle, checkedAlready);
			parents.do({ arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) });
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
		});
	}
	
	wakeUpParents { arg latency=0.0; //see for load 
		var bundle, checkedAlready;
		bundle = MixedBundle.new;
		checkedAlready = Set.new;
		this.wakeUpParentsToBundle(bundle, checkedAlready);
		this.sendBundle(bundle,0,clock);
	}
			
	
	sendAllDefsToBundle { arg bundle;
		objects.do({ arg item;
			item.sendDefToBundle(bundle)
		});
		loaded = true; //remember to send;
	}	
	
	
}



LibNodeProxy : NodeProxy {
	var key;
	*new { arg server, key;
		var res;
		res = this.at(server, key);
		if(res.isNil, {
			res = super.new(server).toLib(key);
		});
		^res;
	}
	toLib { arg key;
		Library.put(this.class, server, key, this);
	}
	*at { arg server, key;
		^Library.at(this, server, key)
	}
}

//the server needs to be a Router.
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
				group = Group.basicNew(constantGroupID, server);
				NodeWatcher.register(group);
				bundle.add(["/g_new",constantGroupID,0,0]);
				//server.nodeIsPlaying_(constantGroupID); //force isPlaying.
	}
	generateUniqueName {
		^asString(constantGroupID) ++ objects.size
	}

}





