//a listener on a bus

BusPlug : AbstractFunction {
	
	var <server, <bus; 		
	var <monitorGroup;
	var <busArg = \; // cache for "/s_new" bus arg
	
	classvar <>defaultNumAudio=2, <>defaultNumControl=1;
	
	
	*new { arg server;
		^super.newCopyArgs(server ? Server.default).init;
	}
	
	*for { arg bus;
		bus = bus.asBus.as(SharedBus);
		^this.new(bus.server).bus_(bus)
	}
	
	*audio { arg server, numChannels;
		^this.new(server).defineBus(\audio, numChannels);
	}
	
	*control { arg server, numChannels;
		^this.new(server).defineBus(\control, numChannels);
	}
	
	*initClass {
		2.do({ arg i; i = i + 1;
			SynthDef("system_link_audio_" ++ i, { arg out=0, in=16, vol=1;
				var env;
				env = ProxySynthDef.makeFadeEnv * vol;
				Out.ar(out, InFeedback.ar(in, i) * env) 
			}, [\ir, \ir]).writeDefFile;
		});
		2.do({ arg i; i = i + 1;
			SynthDef.writeOnce("system_link_control_" ++ i, { arg out=0, in=16;
				var env;
				env = ProxySynthDef.makeFadeEnv;
				Out.kr(out, In.kr(in, i) * env) 
			}, [\ir, \ir]);
		})
	}
	
	init {
		this.free;
		this.stop;
		this.freeBus;
	}
	clear { this.init }
	
	
	////////  bus definitions  //////////////////////////////////////////
	
	
	
	bus_ { arg b; 
		this.freeBus; 
		bus = b; 
		this.makeBusArg;
	}
	
	//returns boolean
	initBus { arg rate, numChannels;
				if((rate === 'scalar') || rate.isNil, { ^true }); //this is no problem
				if(this.isNeutral, {
					this.defineBus(rate, numChannels);
					^true
				}, {
					numChannels = numChannels ? this.numChannels;
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
		this.freeBus;
		bus = SharedBus.alloc(rate, server, numChannels);
		this.makeBusArg;
	}
	
	freeBus {
		if(bus.notNil, { 
			bus.setAll(0); // clean up
			bus.releaseBus; // free shared bus
			//bus.free;
		});
		bus = nil;
		this.makeBusArg;
	}
		
	makeBusArg { 	
			var index, numChannels;
			busArg = if(bus.isNil or: {Êbus.rate === 'audio' }) // audio buses can't be 
					{Ê\ } {						// used for control mapping
						index = this.index;
						numChannels = this.numChannels;
						if(numChannels == 1) 
							{ asSymbol("\c" ++ index) } 
							{
							Array.fill(numChannels, { arg i; asSymbol("\c" ++ (index + i)) })
							}
					}          
	}
	wakeUpToBundle {}
	wakeUp {}
	asBus { ^if(this.isNeutral, { nil }, { bus }) }
	
		
	
	////////////  playing and access  /////////////////////////////////////////////
	
	
	rate {  ^if(bus.isNil) { \scalar } { bus.rate } }
	numChannels {  ^if(bus.isNil) { nil } { bus.numChannels } }
	index { ^if(bus.isNil) { nil } { bus.index } }
	
	
	isNeutral {
		^if(bus.isNil, { true }, {
			bus.index.isNil && bus.numChannels.isNil
		})
	}
	
	prepareOutput { } //see subclass

	ar { arg numChannels, offset=0;
		if(this.isNeutral, { 
			this.defineBus(\audio, numChannels) 
		});
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? this.class.defaultNumAudio, offset)
	}
	
	kr { arg numChannels, offset=0;
		if(this.isNeutral, { 
			this.defineBus(\control, numChannels) 
		});
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? this.class.defaultNumControl, offset)
	}
	
	//////////// embedding bus in event streams, myself if within a normal stream
	
	embedInStream { arg inval;
			if(this.isPlaying.not) { this.wakeUp }; 
			if(inval.isNil) {Êthis.yield } { busArg.yield } // if in event stream yield bus arg
	}
	asStream  {
			^Routine.new({ arg inval;
				loop({
					if(this.isPlaying.not) { this.wakeUp }; 
					if(inval.isNil) {Êthis.yield } { busArg.yield }
				})
			})
	}
	
	
	///// lazy  math support  /////////
	
	value { arg something; 
		var n;
		n = something.numChannels;
		^if(something.rate == 'audio', {this.ar(n)}, {this.kr(n)})  
	}
	composeUnaryOp { arg aSelector;
		^UnaryOpPlug.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, this, something)
	}
	reverseComposeBinaryOp { arg aSelector, something;
		^BinaryOpPlug.new(aSelector, something, this)
	}
	composeNAryOp { arg aSelector, anArgList;
		^thisMethod.notYetImplemented
	}
	
	// error catch
	
	writeInputSpec {
		"use .ar or .kr to use within a synth.".error; this.halt;
	}
	
	
	///// monitoring //////////////
	
	//play { arg group=0, atTime, bus, multi=false;
	play { arg busIndex=0, nChan, group, multi=false;
		var bundle, divider, n, localServer, ok;
		
		localServer = this.localServer; //multi client support
		if(localServer.serverRunning.not, { "server not running".inform; ^nil });
		group = (group ? localServer).asGroup;
		if(multi.not and: { monitorGroup.isPlaying }, { //maybe should warn if not same server
			if(monitorGroup.group !== group) { monitorGroup.moveToTail(group) };
			^monitorGroup 
		});
			
			ok = this.initBus(\audio, nChan);
			if(ok.not, { ("can't monitor a" + bus.rate + "proxy").error });
			this.wakeUp;
			
			n = bus.numChannels;
			nChan = nChan ? n;
			nChan = nChan.min(n);
		
			bundle = MixedBundle.new;
			if(monitorGroup.isPlaying.not, { 
				monitorGroup = Group.newToBundle(bundle, group, \addToTail);
				NodeWatcher.register(monitorGroup);
			});
			
			divider = if(nChan.even, 2, 1);
			(nChan div: divider).do({ arg i;
			bundle.add([9, "system_link_audio_"++divider, 
					localServer.nextNodeID, 1, monitorGroup.nodeID,
					\out, busIndex + (i*divider), \in, bus.index + (i*divider)
					]);
			});
			bundle.send(localServer, localServer.latency);
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
	
	// shared node proxy support
	
	shared { ^false } 
	localServer { ^server }
	
	printOn { arg stream;
		stream 	<< this.class.name << "." << bus.rate << "(" 
				<< server << ", " << bus.numChannels <<")";
	}
	
	
}




//////////////////////////////////////////////////////////////
// a bus plug that holds synths or client side players ///////


NodeProxy : BusPlug {


	var <group, <objects;
	var <parents, <nodeMap;	
	var <loaded=false, <>awake=true, <>paused=false, <task, <>clock; 	
	classvar <>buildProxy;
	
	
	*make { arg server, rate, numChannels, inputs;
		var res;
		res = this.new(server);
		res.initBus(rate, numChannels);
		inputs.do({ arg o; res.add(o) });
		^res
	}
	
	init {
		nodeMap = ProxyNodeMap.new.proxy_(this); 
		this.clearObjects;
		loaded = false;
	}
	
	clear {
		this.init;
		super.init;
	}
	
	clearObjects {
		if(objects.notNil, { objects.do({ arg item; item.free }) });
		objects = Order.new;
		this.initParents;
	}

	initParents {
		parents = IdentitySet.new;
	}
	
	end {
		var dt;
		dt = this.fadeTime;
		Routine({ this.free; (dt + server.latency).wait; this.stop;  }).play;
	}
	
	fadeTime_ { arg t;
		this.set(\fadeTime, t);
	}
	fadeTime {
		^nodeMap.at(\fadeTime).value ? 0.02;
	}
	
	generateUniqueName {
			^asString(this.identityHash.abs)
	}
	
	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	
	pause {
		objects.do { |item| item.pause(clock) };
		task.stop;
		paused = true;
	}
	
	resume {
		paused = false;
		objects.do { |item| item.resume(clock) };
	}
	
		
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0, extraArgs;
		this.put(nil, obj, channelOffset, extraArgs)
	}
	
	source_ { arg obj;
		this.put(-1, obj, 0) 
	}
	
	removeLast { 
		this.removeAt(objects.size - 1); 
	}
	
	removeAt { arg index;
		objects.removeAt(index).stop //should free also. parents are the problem still
	}
	
	removeAll { objects.size.do { |i| this.removeAt(0) } }
	
	
	at { arg index;  ^objects.at(index) }
	
	
	put { arg index=0, obj, channelOffset = 0, extraArgs; 			var container, bundle, grandParents, freeAll;
			
			freeAll = index.notNil and: { index < 0 };
			if(obj.isNil) { 
				^if(freeAll) { this.removeAll  } { this.removeAt(index) }
			};
			bundle = MixedBundle.new;
			if(parents.isEmpty, { grandParents = parents }, {
				grandParents = parents.copy;
				if(freeAll) { this.initParents };
			});			
			
			container = this.wrapObject(obj, channelOffset, index ? objects.size);
			if(container.readyForPlay, {
				this.addObject(bundle, container, index, freeAll);
			}, { 
				parents = grandParents;
				"failed to add to node proxy: numChannels and rate must match".inform;
				^this 
			});
			
			if(server.serverRunning, {
				container.loadToBundle(bundle);
				loaded = true;
				if(awake, {
					this.prepareToBundle(nil, bundle);
					this.sendObjectToBundle(bundle, container, extraArgs);
				});
				bundle.schedSend(server, clock);
			}, {
				loaded = false;
			});

	}
	
	
		
	
	wrapObject { arg obj, channelOffset=0, index;
		var container, ok;
		container = obj.makeProxyControl(channelOffset,this);
		this.class.buildProxy = this;
		container.build(this, index); // bus allocation happens here
		this.class.buildProxy = nil;
		^container
	}
	
	addObject { arg bundle, obj, index, freeAll;
		var rep;
		
		if(freeAll, { 
			if(this.isPlaying, { this.stopAllToBundle(bundle); }); 
			objects.do({ arg item; item.freeToBundle(bundle);  });
			objects = Order.with(obj);
		}, {
			if(index.isNil, {
				objects.add(obj);
			}, {
				this.stopToBundleAt(bundle, index);
				objects.at(index).freeToBundle(bundle); //, this.fadeTime
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
		index = this.index;
		if(index.isNil, { "couldn't allocate bus".error; this.halt });
		nodeMap.set(\out, index, \i_out, index);
	}
	
		
	freeTask { this.task = nil;  }
	
	task_ { arg argTask;
		var bundle, t;
		bundle = MixedBundle.new;
		if(task.notNil) { bundle.addAction(task, \stop) };
		task = argTask;
		if(paused.not and: {this.isPlaying}, {  this.playTaskToBundle(bundle) });//clock
		bundle.schedSend(server, clock);

	}
	
	lag { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
	unlag { arg ... args;
		nodeMap.removeRates(args);
		this.rebuild;
	}
	setRates { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
		
	bus_ { arg inBus;
		if(server != inBus.server, { "can't change the server".inform;^this });
		super.bus_(inBus);
		this.rebuild;
	}
	
	index_ { arg i;
		this.bus = Bus.new(this.rate, i, this.numChannels, server);
	}
	
	group_ { arg agroup;
		if(agroup.server !== server, { "cannot move to another server".error; ^this });
		if(this.isPlaying, { this.free; group = agroup; this.wakeUp }, { group = agroup });
	}
		
	sendNodeMap { 
		var bundle;
		bundle = MixedBundle.new;
		nodeMap.addToBundle(bundle, group);
		bundle.schedSend(server, clock);
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
		nodeMap.set(\fadeTime, fadeTime);
		this.linkNodeMap;
		if(this.isPlaying, { this.sendEach(nil,true) });
	}
	
	loadToBundle { arg bundle;
		this.initParents;
		if(bus.notNil) { bus.realloc }; // if server was rebooted 
		this.class.buildProxy = this;
		objects.do { arg item, i;
			item.build(this, i);
			item.loadToBundle(bundle);
		};
		this.class.buildProxy = nil;
		loaded = true;
	}

	rebuild {
		var bundle;
		if(this.isPlaying, {
			bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			bundle.schedSend(server, clock);
			bundle = MixedBundle.new;
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock);
		}, {
			loaded = false;
		});
	
	}
	
	supplementNodeMap {
		objects.do { |obj|
				var names, values;
				names = obj.controlNames;
				values = obj.controlValues;
				names.do({ arg name,i; 
					if (nodeMap.at(name).isNil)
						{ nodeMap.set(name, values.at(i)) }
				});
		};
	}
	
	
	/*
	undo {
		var bundle, dt;
		bundle = MixedBundle.new;
		dt = this.fadeTime;
		this.fadeTime = 0.1;
		this.stopAllToBundle(bundle);
		objects = undo;
		this.loadToBundle(bundle);
		this.sendAllToBundle(bundle);
		if(this.isPlaying, {
			bundle.schedSend(server, clock);
		});
		this.fadeTime = dt;
	}
	*/
	
		

	
	
	///////////////////// tasks ////////////////////////
	
	//sends all objects, frees the last ones first.
	
	spawner { arg beats=1, argFunc;
		var dt;
		dt = beats.asStream;
		argFunc = if(argFunc.notNil, { argFunc.asArgStream }, { #[] });
		awake = true;
		this.task = Routine.new({ 
					inf.do({ arg i;
						var t, args;
						if((t = dt.next).isNil, { nil.alwaysYield });
						args = argFunc.next(i, beats);
						this.sendAll(
							args ++ [\i, i, \beats, thisThread.beats],
							true
						); 
						t.wait; 
						})
					});
		awake = false;
	}
	
	// spawns synth at index, assumes fixed time envelope
	
	gspawner { arg beats=1, argFunc, index=0;
		var dt;
		dt = beats.asStream;
		argFunc = if(argFunc.notNil, { argFunc.asArgStream }, { #[] });
		index = index.loop.asStream;
		awake = true;
		this.task = Routine.new({ 
					inf.do({ arg i;	
						var t;
						if((t = dt.next).isNil, { nil.alwaysYield });
						this.spawn(
							argFunc.value(i, beats) ++ [\i, i, \beats, thisThread.beats], 
							index.next
						); 
						t.wait; 
						})
					});
		awake = false;
	}
	
	freeSpawn { awake = true; this.task = nil } // find better solution.
	
	taskFunc_ { arg func; 
		this.task = Routine.new({ func.value(this) })
	}
	
	readFromBus { arg busses;
		var n, x;
		busses = busses.asCollection;
		n = this.numChannels;
		busses.do({ arg item, i;
			x = min(item.numChannels ? n, n);
			this.put(i,
				SynthControl.new("system_link_" ++ this.rate ++ "_" ++ x).hasGate_(true), 
				0, 
				[\in, item.index, \out, this.index]
			)
		});
	}
	
	read { arg proxies;
		proxies = proxies.asCollection;
		proxies.do({ arg item; item.wakeUp });
		this.readFromBus(proxies)
	}
	
	
	/////// send and spawn //////////
	
	
	getBundle { arg prepTime=0.0;//default: def is on server, time required is 0
		var bundle;
		nodeMap.updateBundle;
		bundle = 	MixedBundle.new.preparationTime_(prepTime); 
		this.prepareToBundle(nil, bundle);
		^bundle
	}
		
	spawn { arg extraArgs, index=0;
			var bundle, obj, i;
			obj = objects.at(index);
			if(obj.notNil, {
				i = this.index;
				bundle = this.getBundle;
				obj.spawnToBundle(bundle, [\out, i, \i_out, i] ++ extraArgs, this);
				nodeMap.setToBundle(bundle, -1);
				nodeMap.mapToBundle(bundle, -1);
				bundle.schedSend(server);
			})
	}
	
	
	send { arg extraArgs, index=0, freeLast=true;
			var bundle, obj;
			obj = objects.at(index);
			if(obj.notNil, {
				bundle = this.getBundle;
				if(freeLast, { obj.stopToBundle(bundle) });
				this.sendObjectToBundle(bundle, obj, extraArgs);
				bundle.schedSend(server);
			})
	}
	
	sendAll { arg extraArgs, freeLast=true;
			var bundle;
			bundle = this.getBundle;
			if(freeLast, { this.stopAllToBundle(bundle) });
			this.sendAllToBundle(bundle, extraArgs);
			bundle.schedSend(server);
	}
	
	sendEach { arg extraArgs, freeLast=false;
			var bundle;
			bundle = this.getBundle;
			if(freeLast, { this.stopAllToBundle(bundle) });
			this.sendEachToBundle(bundle, extraArgs);
			bundle.schedSend(server);
	
	}
	
	

	
		
	/////// append to bundle commands
	
	
	defaultGroupID { ^nil } //shared proxy support
	
	prepareToBundle { arg argGroup, bundle;
		if(this.isPlaying.not) {
				group = Group.basicNew(server, this.defaultGroupID);
				NodeWatcher.register(group);
				group.isPlaying = server.serverRunning;
				bundle.add(group.newMsg(argGroup ? server, \addToHead));
				if(task.notNil, { this.playTaskToBundle(bundle) });
		}
	}
	
	playTaskToBundle { arg bundle; //revisit
				bundle.addSchedFunction({ 
						if(task.isPlaying.not) { task.reset; task.play(clock); };
						nil; 
					}, server.latency)
	}

	//apply the node map settings to the entire group
	sendAllToBundle { arg bundle, extraArgs;
				objects.do({ arg item;
						item.playToBundle(bundle, extraArgs.value, this);
				});
				if(objects.notEmpty, { nodeMap.addToBundle(bundle, group) });
	}
	
	//apply the node map settings to each synth separately
	sendEachToBundle { arg bundle, extraArgs;
				objects.do({ arg item;
					this.sendObjectToBundle(bundle, item, extraArgs.value)
				});
	}
	
	//send single object
	sendObjectToBundle { arg bundle, object, extraArgs;
				var synth;
				synth = object.playToBundle(bundle, extraArgs.value, this);
				if(synth.notNil, { nodeMap.addToBundle(bundle, synth) });
	}

	
	
	stopAllToBundle { arg bundle;
				objects.do({ arg item;
					item.stopToBundle(bundle);
				});
	}
	
	stopToBundleAt { arg bundle, index;
				var obj;
				obj = objects.at(index);
				if(obj.notNil, { obj.stopToBundle(bundle) });
	}

	
	wakeUp { 		
			var bundle;
				bundle = MixedBundle.new;
				this.wakeUpToBundle(bundle);
				bundle.schedSend(server, clock)
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
				this.prepareToBundle(nil, bundle);
				if(awake, { this.sendAllToBundle(bundle) });
			});
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			parents.do({ arg item; item.wakeUpToBundle(bundle, checkedAlready) });
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
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
	
	isPlaying { ^group.isPlaying }
	free { arg all=true;
		var bundle;
		if(this.isPlaying, {	
			bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			if(all) { bundle.add(["/n_free", group.nodeID]) };
			bundle.send(server);
		})
 	}
	
	release { this.free(false) }
	
	
//	set { arg ... args;
//		nodeMap.performList(\set, args);
//		if(this.isPlaying, { group.performList(\set, args) });
//	} // use group.set otherwise..
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying, { 
			server.sendBundle(server.latency, [15, group.nodeID] ++ args); 
		});
	}
	
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
		if(keys.isEmpty, { keys = nodeMap.settingKeys });
		nodeMap.performList(\unset, keys);
		if(this.isPlaying, { this.sendAll(nil, true) });
	}
	
	unmap { arg ... keys;
		if(keys.isEmpty, { keys = nodeMap.mappingKeys });
		nodeMap.performList(\unmap, keys);
		if(this.isPlaying, {
			keys.do({ arg key; group.map(key,-1) });
			nodeMap.sendToNode(group) 
		});
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
			this.sendEach(nil, true)
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
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.env(env) },
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
		SystemClock.sched(dur, {
			if(oldLevel.notNil, { this.set(key, oldLevel) }, { this.unset(key) }); 
			nil;
		});
	}
	
	lineAt { arg key, level=1.0, dur;
		this.performAtControl(\line, key, level, dur);
	}
	
	xlineAt { arg key, level=1.0, dur;
		this.performAtControl(\xline, key, level, dur);
	}
	
	// private
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
	classvar <>defaultServer;
	var key;
	*new { arg key, object, server;
		var res;
		server = server ? defaultServer ? Server.default;
		res = this.at(server, key);
		if(res.isNil, {
			res = super.new(server).toLib(key);
		});
		if(object.notNil, { res.source = object });
		^res;
	}
	*clear {
		Library.at(this).do({ arg item; item.do({ arg item; item.clear }) });
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

//to test with patterns

SharedNodeProxy : NodeProxy {
	var <constantGroupID;
	
	*new { arg server, groupID;
		^super.newCopyArgs(server).initGroupID(groupID).clear	}
	
	shared { ^true }
	
	initGroupID { arg groupID;  
		constantGroupID = groupID ?? {server.nextSharedNodeID}; 
		awake = true;
	}
	defaultGroupID { 
				postln("started new shared group" + constantGroupID);
				^constantGroupID;
	}
	
	localServer { ^server.localServer }
	
	generateUniqueName {
		^asString(constantGroupID)
	}
	// problem: parents are lost
	addObject { arg bundle, obj, index, freeAll;
		if(obj.distributable) {
			super.addObject(bundle, obj, index, freeAll);
		} { "this type of input is not distributable in a shared node proxy".error; ^this.halt; };
	}

	//map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		//check if any not shared proxy is passed in
		(args.size div: 2).do({ arg i; 
			if(args[2*i+1].shared.not, { "shouldn't map a local to a shared proxy".error; ^this }) 
		});
		nodeMap.performList(\map, args);
		if(this.isPlaying, { 
			nodeMap.sendToNode(group);
		})
	}
	
	mapEnvir {}
	
	// use shared node proxy only with functions that can release the synth.
	stopAllToBundle { arg bundle;
				bundle.add(["/n_set", constantGroupID, \gate, 0]);
	}
	stopToBundleAt { arg bundle, index;
				bundle.add(["/n_set", constantGroupID, \gate, 0]);
	}
	
	
	

	///////////////////
	
	prepareToBundle { arg argGroup, bundle; // ignore ingroup
		if(this.isPlaying.not) {
				group = Group.basicNew(server, this.defaultGroupID);
				group.isPlaying = true;
				NodeWatcher.newFrom(this.localServer).register(group);
				// this happens in sendAll and sendEach, should implement in sendObj
				// bundle.add(group.newMsg(argGroup ? server, \addToHead));
		};
		bundle.add(["/g_new", this.defaultGroupID]); // duplicate sending is no problem
	}
	


}


