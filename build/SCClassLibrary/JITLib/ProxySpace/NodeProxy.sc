//a listener on a bus

BusPlug : AbstractFunction {
	
	var <server, <bus; 		
	var <monitorGroup, <>clock;
	
	classvar <>defaultNumAudio=2, <>defaultNumControl=1;
	
	
	*new { arg server;
		^super.newCopyArgs(server ? Server.local).clear	}
	
	*for { arg bus;
		bus = bus.asBus;
		^this.new(bus.server).bus_(bus)
	}
	
	*audio { arg server, numChannels;
		^this.new(server).defineBus(\audio, numChannels);
	}
	
	*control { arg server, numChannels;
		^this.new(server).defineBus(\control, numChannels);
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
		this.free;
		this.stop;
		this.freeBus;
	}
	
	////////  bus definitions  //////////////////////////////////////////
	
	freeBus {
		if(bus.notNil, { bus.free });
		bus = nil;
	}
	
	
	//returns boolean
	initBus { arg rate, numChannels;
				if((rate === 'scalar') || rate.isNil, { ^true }); //this is no problem
				if(this.isNeutral, {
					this.defineBus(rate, numChannels);
					^true
				}, {
					^(bus.rate === rate) && (numChannels <= bus.numChannels)
				});
	}
	
	defineBus { arg rate=\audio, numChannels;
		if(numChannels.isNil, {
			numChannels = if(rate === \audio, { 
								this.class.defaultNumAudio 
								}, {
								this.class.defaultNumControl							})
		});
		bus = Bus.alloc(rate, server, numChannels); 
	}
		
	
	wakeUpToBundle {}
	wakeUp {}
	asBus { ^bus }
	
	
	
	////////////  playing and access  /////////////////////////////////////////////
	
	
	rate {  ^bus.tryPerform(\rate) ? \scalar }
	numChannels {  ^bus.tryPerform(\numChannels) }
	index { ^bus.tryPerform(\index) }
	
	
	isNeutral {
		^if(bus.isNil, { true }, {
			bus.index.isNil && bus.numChannels.isNil
		})
	}
	
	prepareOutput { } //see subclass

	ar { arg numChannels;
		if(this.isNeutral, { 
			this.defineBus(\audio, numChannels) 
		});
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? this.class.defaultNumAudio)
	}
	
	kr { arg numChannels=1;
		if(this.isNeutral, { 
			this.defineBus(\control, numChannels) 
		});
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? this.class.defaultNumControl)
	}
	
	writeInputSpec {
		"use .ar or .kr to use within a synth.".error; this.halt;
	}
	
	///// lazy  math support  /////////
	
	value { arg something; 
		var n;
		n = something.numChannels;
		^if(something.rate == 'audio', {this.ar(n)}, {this.kr(n)})  
	}
	
	composeBinaryOp { arg aSelector, something;
		^{ this.value(something).perform(aSelector, something.value) }
	}
	reverseComposeBinaryOp { arg aSelector, something;
		^{ something.value.perform(aSelector, this.value(something)) }
	}
	composeNAryOp { arg aSelector, anArgList;
		^{ this.value(anArgList).performList(aSelector, anArgList) }
	}
	
	
	
	///// monitoring //////////////
	
	play { arg busIndex=0, nChan, group, multi=false;
		var bundle, divider, n;
		
		if(server.serverRunning.not, { "server not running".inform; ^nil });
		if(multi.not and: { monitorGroup.isPlaying }, { ^monitorGroup });
		
			bundle = MixedBundle.new;
			
			if(this.isNeutral, { 
				this.defineBus(\audio, nChan)
			}, {
				if(bus.rate != 'audio', { ("can't monitor a" + bus.rate + "proxy").error; ^nil });
			}); 
		
			
			n = bus.numChannels;
			nChan = nChan ? n;
			nChan = nChan.min(n);
		
			this.wakeUpToBundle(bundle);
	
			if(monitorGroup.isPlaying.not, { 
				monitorGroup = Group.newToBundle(bundle, group ? server);
				NodeWatcher.register(monitorGroup);
			});
			
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
	
	
	toggle {
		if(monitorGroup.isPlaying, { this.stop }, { this.play });
	}

	stop {
		if(monitorGroup.isPlaying, {
			monitorGroup.free;
			monitorGroup = nil;
		})
	}
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		if(server.serverRunning.not, { "server not running".inform; ^nil });
		rec = RecNodeProxy.newFrom(this, numChannels);
		rec.open(path, headerFormat, sampleFormat);
		rec.record;
		^rec
	}
	
	sendBundle { arg bundle;
			bundle.schedSend(server, clock)
	}
	
	printOn { arg stream;
		stream << this.class.name << "." << bus.rate << "(" << server << ", " << bus.numChannels <<")";
	}
	
	
	
	
}




//////////////////////////////////////////////////////////////
// a bus plug that holds synths or client side players ///////


NodeProxy : BusPlug {


	var <group, <objects;
	var <parents, <nodeMap;	//playing templates
	var <lags, <prepend, <task, <loaded=false, <>awake=true; 	
	classvar <>buildProxy;
	
	clear {
		nodeMap = ProxyNodeMap.new.proxy_(this); 
		this.clearObjects;
		loaded = false;
		super.clear;
	}
	
	clearObjects {
		//objects = Array.new;
		objects = Order.new;
		this.initParents;
	}

	initParents {
		parents = IdentitySet.new;
	}
	
	end { this.stop; this.free; } //stop playback and inner group
	
	fadeTime_ { arg t;
		this.set('#fadeTime', t);
	}
	fadeTime {
		^nodeMap.at('#fadeTime').value;
	}
	
	generateUniqueName { 
			//^asString(this.identityHash.abs) ++ objects.size
			^asString(this.identityHash.abs)
	}
	
	asGroup { ^group.asGroup }
	
		
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0;
		this.put(obj, channelOffset, nil)
	}
	
	source_ { arg obj; 
		this.put(obj, 0, -1) 
	}
	
	removeLast { 
		this.removeAt(objects.size - 1); 
	}
	
	removeAt { arg index;
		objects.removeAt(index).stop
	}
	
	put { arg obj, channelOffset = 0, index = -1; 					var container, bundle, grandParents, freeAll;
			freeAll = index.notNil and: {index < 0};
			bundle = MixedBundle.new;
			if(parents.isEmpty, { grandParents = parents }, {
				grandParents = parents.copy;
				if(freeAll, { this.initParents });
			});
			
			
			container = this.wrapObject(obj, channelOffset, index ? objects.size);			if(container.readyForPlay, {
				this.addObject(bundle, container, index, freeAll);
			}, { 
				parents = grandParents;
				"failed to add to node proxy: numChannels and rate must match".inform;
				^this 
			});
			
			if(server.serverRunning, {
				container.sendDefToBundle(bundle, server);
				if(awake, {
					if(this.isPlaying.not, { 
						this.prepareForPlayToBundle(bundle);
					});
					if(freeAll, {
							this.sendAllToBundle(bundle)
					}, {
							this.sendObjectToBundle(bundle, container)
					});
					
				});
				this.sendBundle(bundle);
				loaded = true;
			}, {
				loaded = false;
			});

	}
		
	
	wrapObject { arg obj, channelOffset=0, index;
		var container, ok;
		container = obj.makeProxyControl(channelOffset);
		this.class.buildProxy = this;
		container.build(this, index); //bus allocation happens here
		this.class.buildProxy = nil;
		^container
	}
	
	addObject { arg bundle, obj, index, freeAll;
		var rep;
		if(freeAll, { 
			if(this.isPlaying, { this.freeAllToBundle(bundle) }); 
			objects.do({ arg item; item.free });
			//objects = Array.with(obj);
			objects = Order.with(obj);
		}, {
			if(index.isNil, {
			objects = objects.add(obj);
			}, {
				rep = objects.at(index);
				if(rep.notNil, { 
					rep.stopToBundle(bundle);
					rep.free;
				});
				objects = objects.put(index, obj);
			})
		})
	}
	
	
	defineBus { arg rate=\audio, numChannels;
		super.defineBus(rate, numChannels);
		this.linkNodeMap;
	}
	
	linkNodeMap {
		var index;
		index = bus.index;
		if(index.isNil, { "couldn't allocate bus".error; this.halt });
		nodeMap.set(\out, index, \i_out, index);
	}
	
	build { 
		this.initParents;
		if(bus.notNil, { bus.realloc });
		this.class.buildProxy = this;
			objects.do({ arg container, i;
				container.build(this, i);
			});
		this.class.buildProxy = nil;
		objects.selectInPlace({ arg item; item.readyForPlay }); //clean up
	}
	task_ { arg argTask;
		if(task.notNil, { task.stop });
		task = argTask;
		if(this.isPlaying, { task.play });//clock
	
	}
	lags_ { arg list;
		lags = list;
		this.rebuild;
	}
	
	prepend_ { arg list;
		prepend = list;
		this.rebuild;
	}
	
	clearPrepend {
		this.release;
		this.clearObjects;
		prepend = #[];
	}
	
	bus_ { arg inBus;
		server = inBus.server;
		bus = inBus;
		this.linkNodeMap;
		this.rebuild;
	}
	
	index_ { arg i;
		this.bus = Bus.new(this.rate, i, this.numChannels, server);
	}
	
	server_ { arg srv;
		server = srv;
		this.rebuild;
	}
	
	sendNodeMap { 
		var bundle;
		bundle = MixedBundle.new;
		nodeMap.addToBundle(bundle, group);
		this.sendBundle(bundle);
	}
	
	nodeMap_ { arg map; //keep fadeTime?
		
		nodeMap = map;
		this.linkNodeMap;
		if(this.isPlaying, { this.sendNodeMap });
	}
	
	fadeToMap { arg map, interpolKeys;
		var fadeTime, values;
		fadeTime = this.fadeTime;
		if(interpolKeys.notNil, { 
			values = interpolKeys.asCollection.collect({ arg item; map.settings.at(item).value });
			this.lineAt(interpolKeys, values) 
		});
		nodeMap = map.copy;
		nodeMap.set('#fadeTime', fadeTime);
		this.linkNodeMap;
		if(this.isPlaying, { this.refresh(true) });
	}
	
	refresh { arg each=false, args;
		var bundle;
		bundle = MixedBundle.new;
		bundle.preparationTime = 0;
		this.freeAllToBundle(bundle);
		if(each, { this.sendEachToBundle(bundle, args) }, { this.sendAllToBundle(bundle, args) });
		this.sendBundle(bundle);
	}
	
	refreshAt { arg index=0, args;
		var bundle, obj;
		obj = objects.at(index);
		if(obj.notNil, {
			bundle = MixedBundle.new;
			bundle.preparationTime = 0;
			obj.stopToBundle(bundle);
			this.sendObjectToBundle(bundle, obj, args);
		});
		this.sendBundle(bundle);
	}
	
	rebuild {
		var bundle;
		if(this.isPlaying, {
			bundle = MixedBundle.new;
			this.freeAllToBundle(bundle);
			this.sendBundle(bundle);
			bundle = MixedBundle.new;
			this.build;
			objects.do({ arg item; item.sendDefToBundle(bundle, server) });
			this.sendAllToBundle(bundle);
			this.sendBundle(bundle);
		}, {
			loaded = false;
		});
	
	}
	
	
	/*
	undo {
		var bundle, dt;
		bundle = MixedBundle.new;
		dt = this.fadeTime;
		this.fadeTime = 0.1;
		this.freeAllToBundle(bundle);
		objects = undo;
		this.loadToBundle(bundle);
		this.sendAllToBundle(bundle);
		if(this.isPlaying, {
			this.sendBundle(bundle);
		});
		this.fadeTime = dt;
	}
	*/
	
	

	
	
	/////////////////////////////////////////////
	
	send { arg extraArgs, index=0;
			var bundle, obj;
			obj = objects.at(index);
			if(obj.notNil, {
				bundle = MixedBundle.new.preparationTime_(0); //latency is 0, def is on server
				if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle) });
				this.sendObjectToBundle(bundle, obj, extraArgs);
				this.sendBundle(bundle);
			})
	}
	
	sendAll { arg extraArgs;
			var bundle;
			bundle = MixedBundle.new.preparationTime_(0); //latency is 0, def is on server
			if(this.isPlaying.not, { this.prepareForPlayToBundle(bundle) });
			this.sendAllToBundle(bundle, extraArgs);
			this.sendBundle(bundle);
	}
	
	
	xtexture { arg beats=1, argFunc, index;
		var dt;
		dt = beats.asStream; //.loop;
		if(index.isNil, {
			this.task = Task({ loop({ this.refresh(false, argFunc.value); dt.value.wait; }) })
		}, {
			this.task = Task({ loop({ this.refreshAt(index, argFunc.value); dt.value.wait; }) })
		
		});
	}
	
	
	
	/////// append to bundle commands
	
			
	prepareForPlayToBundle { arg bundle;
				group = Group.basicNew(server);
				NodeWatcher.register(group);
				group.isPlaying = true; //force isPlaying.
				bundle.add(group.newMsg(server, \addToHead));
				if(task.notNil, { bundle.addFunction({ task.stop; task.play; })  });
	}
	

	sendAllToBundle { arg bundle, extraArgs;
				objects.do({ arg item;
						item.playToBundle(bundle, extraArgs, this);
				});
				//apply the node map settings to the entire group
				if(objects.notEmpty, { nodeMap.addToBundle(bundle, group) });
	}
	
	sendObjectToBundle { arg bundle, object, extraArgs;
				var synth;
				synth = object.playToBundle(bundle, extraArgs, this);
				if(synth.notNil, { nodeMap.addToBundle(bundle, synth) });
	}

	sendEachToBundle { arg bundle, extraArgs;
				//apply the node map settings to each synth separately
				objects.do({ arg item;
					this.sendObjectToBundle(bundle, item, extraArgs)
				});
	}
	
	freeAllToBundle { arg bundle;
				objects.do({ arg item;
					item.stopToBundle(bundle);
				});
	}
	
	wakeUp { 
		var bundle;
		bundle = MixedBundle.new;
		this.wakeUpToBundle(bundle);
		this.sendBundle(bundle);
	}
	
	wakeUpToBundle { arg bundle, checkedAlready;
		if(checkedAlready.isNil, { checkedAlready = IdentitySet.new });
		if(checkedAlready.includes(this).not, {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not, { 
				this.loadToBundle(bundle);
				bundle.preparationTime = 0.2;
			}, { 
				bundle.preparationTime = 0;
			});
			if(this.isPlaying.not, { 
				this.prepareForPlayToBundle(bundle);
				this.sendAllToBundle(bundle);
			});
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			parents.do({ arg item; item.wakeUpToBundle(bundle, checkedAlready) });
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
			
	
	loadToBundle { arg bundle;
		this.build;
		objects.do({ arg item;
			item.sendDefToBundle(bundle, server)
		});
		loaded = true;
	}	
		
	
	////// private /////
	
	
	prepareOutput {
		var parentPlaying;
		parentPlaying = this.addToParentProxy;
		if(parentPlaying, { this.wakeUp });
	}
	
	addToParentProxy {
		var parentProxy;
		parentProxy = this.class.buildProxy;
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
		^parentProxy.isPlaying;
	}
	
	
	////////////behave like my group////////////
	
	isPlaying {
		^group.isPlaying
	}
	
	free {
		if(this.isPlaying, { group.free; group = nil });
	}
	
	freeAll {
		if(this.isPlaying, { group.freeAll });
	}
	
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	pause { this.run(false) }
	

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
		nodeMap.performList(\map, args);
		if(this.isPlaying, { 
			nodeMap.sendToNode(group);
		})
	}
	
	//map to current environment
	mapEnvir { arg keys;
		nodeMap.mapEnvir(keys);
		if(this.isPlaying, { 
			nodeMap.sendToNode(group);
		})
	}
	
		
	unset { arg ... keys;
		nodeMap.performList(\unset, keys);
		if(this.isPlaying, { nodeMap.sendToNode(group) });
	}
	
	unmap { arg ... keys;
		nodeMap.performList(\unmap, keys);
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
	
	releaseAndStop {
		var dt;
		dt = this.fadeTime ? 0.01;
		Routine({ this.release; (dt + server.latency).wait; this.end }).play;
	}
	
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
		nodeMap.performList(selector, args);
		if(this.isPlaying, { 
			this.refresh(true)
		}, { "not playing".inform })
	}
	
	
	
	//////////////   behave like my bus //////////////////
	
	gate { arg level=1.0, dur=0;
		if(bus.notNil && this.isPlaying, { bus.gate(level, dur, group) }, 
		{ "not playing".inform });
	}
	
	line { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.line(level, dur, group) },
		{ "not playing".inform });
	}
	
	xline { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.xline(level, dur, group) },
		{ "not playing".inform });
	}
	
	env { arg env;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.env(env) }, //??
		{ "not playing".inform });
	}
	
	setBus { arg ... values; //does work only when no node is playing
		if(bus.notNil && this.isPlaying, {
			bus.performList(\set, values) 
		},
		{ "not playing".inform });
	}
	
	gateAt { arg key, level=1.0, dur=1.0;
		var oldLevel;
		oldLevel = nodeMap.at(key).value;
		this.set(key, level);
		if(oldLevel.notNil, { 
			SystemClock.sched(dur, {
				this.set(key, oldLevel); nil;
			});
		 })
	}
	
	lineAt { arg key, level=1.0, dur;
		this.performAtControl(\line, key, level, dur);
	}
	
	xlineAt { arg key, level=1.0, dur;
		this.performAtControl(\xline, key, level, dur);
	}
	
	performAtControl { arg action, keys, levels=1.0, durs;
		var ctlBus, bundle, id, setArgs, setBundle;
		if(bus.notNil && this.isPlaying, {
			durs = durs ? this.fadeTime;
			id = group.nodeID;
			keys = keys.asArray; levels = levels.asArray; durs = durs.asArray;
			
			keys = keys.select({ arg key, i;
				var ok;
				ok = nodeMap.settings.at(key).notNil;
				if(ok.not, { //set all undefined keys directly to value. this is probably what is expected.
					this.set(key, levels.wrapAt(i));
					if(i < levels.size, { levels.removeAt(i) });
					if(i < durs.size, { durs.removeAt(i) });
					
				});
				ok
			});
			
			ctlBus = Bus.alloc('control', server, keys.size);
			
			bundle = ["/n_map", id];
			keys.do({ arg key, i; bundle = bundle.addAll([key, ctlBus.index + i]) });
			server.sendBundle(server.latency, bundle);
			
			ctlBus.perform(action, levels, durs);
			ctlBus.free;
			
			setArgs = [keys, levels].multiChannelExpand.flat;
			nodeMap.performList(\set, setArgs);
			
			server.sendBundle(server.latency + durs.maxItem, 
				 ["/n_map", id] ++ [keys, -1].multiChannelExpand.flat,
				 ["/n_set", id] ++ setArgs
			);
		}, { "not playing".inform });
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




//the server needs to be a BroadcastServer.
//this class takes care for a constant groupID.

//do not add patterns for now.

SharedNodeProxy : NodeProxy {
	var <constantGroupID;
	
	*new { arg server, groupID;
		^super.newCopyArgs(server).initGroupID(groupID).clear	
	}
		
	initGroupID { arg groupID;  
		constantGroupID = groupID ?? {server.nextSharedNodeID}; 
		awake = true;
	} 											
	
	prepareForPlayToBundle { arg bundle, freeAll=true;
				postln("started new shared group" + constantGroupID);
				group = Group.basicNew(server, constantGroupID);
				group.isPlaying = true; //force isPlaying.
				NodeWatcher.register(group);
				bundle.add(["/g_new", constantGroupID, 0, 0]);
	}
	
	generateUniqueName {
		^asString(constantGroupID)
	}
	
	//play local, wakeUp global
	
	play { arg busIndex=0, nChan, n, multi=false;
		var bundle, localBundle, localServer, divider;
		
		localServer = server.localServer;
		if(localServer.serverRunning.not, { "local server not running".inform; ^nil });
		if(multi.not and: { monitorGroup.isPlaying }, { ^monitorGroup });
			bundle = MixedBundle.new;
			localBundle = MixedBundle.new;
			if(this.isNeutral, { 
				this.defineBus(\audio, nChan)
			}, {
				if(bus.rate != 'audio', { ("can't play a" + bus.rate + "nodeproxy").error; ^nil });
			}); 
			if(monitorGroup.isPlaying.not, { 
				monitorGroup = Group.newToBundle(localBundle, localServer);
				NodeWatcher.register(monitorGroup);
			});
			n = bus.numChannels;
			nChan = nChan ? n;
			nChan = nChan.min(n);
		
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


