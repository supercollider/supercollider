// a listener on a bus

BusPlug : AbstractFunction {
	
	var <server, <bus; 		
	var <monitor;
	var <busArg = \; // cache for "/s_new" bus arg
	classvar <>defaultNumAudio=2, <>defaultNumControl=1;
	
	
	*new { arg server;
		^super.newCopyArgs(server ? Server.default);
	}
	
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
		2.do { arg i; i = i + 1;
			SynthDef("system_link_audio_" ++ i, { arg out=0, in=16, vol=1;
				var env;
				env = EnvGate.new * vol;
				Out.ar(out, InFeedback.ar(in, i) * env) 
			}, [\kr, \ir]).writeDefFile;
		};
		2.do { arg i; i = i + 1;
			SynthDef("system_link_control_" ++ i, { arg out=0, in=16;
				var env;
				env = EnvGate.new;
				Out.kr(out, In.kr(in, i) * env) 
			}, [\kr, \ir]).writeDefFile;
		}
	}
	
	
	clear { 
		this.free;
		this.stop;
		this.freeBus;
	}
		
	
	////////  bus definitions  //////////////////////////////////////////
	
	
	
	bus_ { arg b; 
		this.freeBus; 
		bus = b; 
		this.makeBusArg;
	}
	
	// returns boolean
	initBus { arg rate, numChannels;
				if(rate.isNil or: { rate === 'scalar' }) { ^true }; // this is no problem
				if(this.isNeutral) {
					this.defineBus(rate, numChannels);
					^true
				} {
					numChannels = numChannels ? this.numChannels;
					^(bus.rate === rate) and: { numChannels <= bus.numChannels }
				};
	}
	
	defineBus { arg rate=\audio, numChannels;
		if(numChannels.isNil, {
			numChannels = if(rate === \audio) { 
								this.class.defaultNumAudio 
							} {
								this.class.defaultNumControl							}
		});
		this.freeBus;
		bus = Bus.alloc(rate, server, numChannels);
		this.makeBusArg;
	}
	
	freeBus {
		if(bus.notNil, { 
			bus.setAll(0); // clean up
			bus.free;
		});
		bus = nil;
		this.makeBusArg;
	}
		
	makeBusArg { 	
			var index, numChannels;
			busArg = if(bus.isNil or: { bus.rate === 'audio' }) // audio buses can't be 
					{ \ } {						// used for control mapping
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
	asBus { ^if(this.isNeutral) { nil } { bus } }
	
		
	
	////////////  playing and access  /////////////////////////////////////////////
	
	
	rate {  ^if(bus.isNil) { \scalar } { bus.rate } }
	numChannels {  ^if(bus.isNil) { nil } { bus.numChannels } }
	index { ^if(bus.isNil) { nil } { bus.index } }
	
	
	isNeutral {
		^bus.isNil or: { bus.index.isNil and: { bus.numChannels.isNil } }
	}
	
	prepareOutput { } // see subclass

	ar { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\audio, numChannels) 
		};
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? bus.numChannels, offset)
	}
	
	kr { arg numChannels, offset=0;
		if(this.isNeutral) { 
			this.defineBus(\control, numChannels) 
		};
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? bus.numChannels, offset)
	}
	
	//////////// embedding bus in event streams, myself if within a normal stream
	
	embedInStream { arg inval;
					if(inval.notNil) { 
						if(this.isPlaying.not) {
							if(this.isNeutral) { this.defineBus(\control, 1) }; 
							this.wakeUp 
						};
						busArg.yield;
					} { this.yield  } // if in event stream yield bus arg
					^inval
	}
	asStream  {
			^Routine.new { arg inval;
				loop { inval = this.embedInStream(inval) }
			}
	}
	
	
	///// lazy  math support  /////////
	
	value { arg something; 
		var n;
		n = something.numChannels;
		^if(something.rate == 'audio') { this.ar(n) } { this.kr(n) }  
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
		Error("use .ar or .kr to use within a synth.").throw;
	}
	
	
	///// monitoring //////////////
	
	
	play { arg out=0, numChannels, group, multi=false, vol;  
		var ok, localServer;
		
		localServer = this.localServer; // multi client support
		if(localServer.serverRunning.not, { "server not running".inform; ^nil });
		this.initBus(\audio, numChannels);
		if(this.rate !== 'audio') { Error("can't monitor a control rate proxy").throw };
		this.wakeUp;
		if(monitor.isNil) { monitor = Monitor.new };
		group = (group ? localServer).asGroup;
		monitor.play(bus.index, bus.numChannels, out, numChannels, group, multi, vol)
		^monitor.group
	}
	
	fadeTime  { ^0.02 }
	
	vol { ^if(monitor.isNil) { 1.0 } { monitor.vol } }
	vol_ { arg val; if(this.rate === 'audio') {
						if(monitor.isNil) { monitor = Monitor.new }; monitor.vol = val 
				}
	}
	monitorIndex { ^if(monitor.isNil) { nil } { monitor.out } }
	monitorGroup { ^if(monitor.isNil) { nil } { monitor.group } }
	
	stop { arg fadeTime=0.1;
		monitor.stop(fadeTime)
	}
	
	scope { arg bufsize = 4096, zoom; if(this.isNeutral.not) { ^bus.scope(bufsize, zoom) } }
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		if(server.serverRunning.not) { "server not running".inform; ^nil };
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


	var <group, <objects, <nodeMap;	
	var <loaded=false, <>awake=true, <>paused=false;
	var <>clock, <task; 	
	classvar <>buildProxyControl;
	
	
	*new { arg server, rate, numChannels, inputs;
		var res;
		res = super.new(server).init;
		res.initBus(rate, numChannels);
		inputs.do { arg o; res.add(o) };
		^res
	}
	
	init {
		nodeMap = ProxyNodeMap.new; 
		objects = Order.new;
		loaded = false;
	}
	
	clear {
		this.free(0, true); // free group and objects
		this.stop(0);		// stop any monitor
		this.task = nil;
		this.freeBus;	 // free the bus from the server allocator 
		this.init;	// reset the environment
	}
	
	end { arg fadeTime;
		var dt;
		dt = fadeTime ? this.fadeTime;
		Routine.new { this.free(dt, true); (dt + server.latency).wait; this.stop(0);  } .play;
	}
	
	pause {
		if(this.isPlaying) { objects.do { |item| item.pause(clock) } };
		paused = true;
	}
	
	resume {
		paused = false;
		if(this.isPlaying) { objects.do { |item| item.resume(clock) } };
	}
	
	fadeTime_ { arg t;
		this.set(\fadeTime, t);
	}
	fadeTime {
		^nodeMap.at(\fadeTime).value ? 0.02;
	}
	
	asGroup { ^group.asGroup }
	asTarget { ^group.asGroup }
	
		
		
	//////////// set the source to anything that returns a valid ugen input ////////////
	
	add { arg obj, channelOffset=0, extraArgs;
		this.put(objects.indices.last ? -1 + 1, obj, channelOffset, extraArgs)
	}
	
	source_ { arg obj;
		this.put(nil, obj, 0)
	}
	
	at { arg index;  ^objects.at(index) }
	
	
	put { arg index, obj, channelOffset = 0, extraArgs; 			var container, bundle, orderIndex;
			
			if(obj.isNil) { this.removeAt(index); ^this };
			
			orderIndex = index ? 0;
			if(obj.isSequenceableCollection)
				{ 
					obj.do { |item, i| this.put(i + orderIndex, item) }; 
					^this 
				};
			if(index.isSequenceableCollection)
				{ 
					obj = obj.asArray; 
					index.do { |index, i| this.put(index, obj.wrapAt(i)) } 
					^this	
				};
		
			container = obj.makeProxyControl(channelOffset, this);
			container.build(this, orderIndex); // bus allocation happens here
			
			if(this.shouldAddObject(container, index)) {
				bundle = MixedBundle.new;
				if(index.isNil) 
					{ this.removeAllToBundle(bundle) }
					{ this.removeToBundle(bundle, index) };
				objects = objects.put(orderIndex, container);
			} { 
				"failed to add object to node proxy.".inform;
				^this 
			};
			
			if(server.serverRunning) {
				container.loadToBundle(bundle);
				loaded = true;
				if(awake) { // revisit for tasks!
					this.prepareToBundle(nil, bundle);
					container.wakeUpParentsToBundle(bundle);
					this.sendObjectToBundle(bundle, container, extraArgs, index);
				};
				bundle.schedSend(server, clock);
			} {
				loaded = false;
			};

	}
	
	putSeries { arg first, second, last, value; this.put((first, second..last), value) }
	
	shouldAddObject { arg obj; ^obj.readyForPlay } // shared node proxy overrides this
	
	removeLast { this.removeAt(objects.indices.last) }
	removeAll { this.removeAt(nil) }
	removeAt { arg index; 
				var bundle;
				bundle = MixedBundle.new; 
				if(index.isNil) 
					{ this.removeAllToBundle(bundle) }
					{ this.removeToBundle(bundle, index) };
				bundle.schedSend(server); 
	}
	
	lag { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
	
	setRates { arg ... args;
		nodeMap.setRates(args);
		this.rebuild;
	}
			
	defineBus { arg rate=\audio, numChannels;
		super.defineBus(rate, numChannels);
		this.linkNodeMap;
	}
	
	linkNodeMap {
		var index;
		index = this.index;
		if(index.notNil) { nodeMap.set(\out, index, \i_out, index) };
		nodeMap.proxy = this;
	}
	
	bus_ { arg inBus;
		if(server != inBus.server) { Error("can't change the server").throw };
		super.bus_(inBus);
		this.linkNodeMap;
		this.rebuild;
	}
		
	group_ { arg agroup;
		var bundle;
		if(agroup.server !== server, { Error("cannot move to another server").throw });
		NodeWatcher.register(agroup.isPlaying_(true)); // assume it is playing
		if(this.isPlaying)
		{ 	bundle = MixedBundle.new;
			this.stopAllToBundle(bundle); 
			group = agroup;
			this.sendAllToBundle(bundle); 
			bundle.schedSend(server, clock);
		} { group = agroup };
	}
	
	task_ { arg newTask; 
		var bundle;
		bundle = MixedBundle.new;
		if(task.isPlaying) { bundle.addAction(task, \stop) };
		task = newTask;
		if(this.isPlaying) { this.playTaskToBundle(bundle) };
		bundle.schedSend(server, clock);
	}

	
	// applying the settings to nodes //
	
	sendNodeMap {
		var bundle;
		bundle = MixedBundle.new;
		nodeMap.addToBundle(bundle, group);
		bundle.schedSend(server, clock);
	}
	
	nodeMap_ { arg map, xfade=true;
		var bundle, old, oldargs, fadeTime;
		map.set(\fadeTime, this.fadeTime); // keep old fadeTime
		bundle = MixedBundle.new;
		old = nodeMap;
		nodeMap = map;
		this.linkNodeMap;
		if(this.isPlaying) {
			if(xfade) { this.sendEach(nil,true) }
			{
			oldargs = nodeMap.unsetArgs; 
			if(oldargs.notNil) { bundle.add(["/n_set", group.nodeID] ++ oldargs) }; // unmap old
			nodeMap.addToBundle(bundle, group); // map new
			bundle.schedSend(server, clock);
			}
		};
	}
	
	fadeToMap { arg map, interpolKeys;
		var fadeTime, values;
		fadeTime = this.fadeTime; // todo: old args
		if(interpolKeys.notNil) { 
			values = interpolKeys.asCollection.collect { arg item; map.settings[item].value };
			this.lineAt(interpolKeys, values) 
		};
		nodeMap = map.copy;
		nodeMap.set(\fadeTime, fadeTime);
		this.linkNodeMap;
		if(this.isPlaying) { this.sendEach(nil,true) };
	}
	
	rebuild {
		var bundle;
		if(this.isPlaying) {
			bundle = MixedBundle.new;
			this.stopAllToBundle(bundle);
			bundle.schedSend(server, clock);
			bundle = MixedBundle.new;
			this.loadToBundle(bundle);
			this.sendAllToBundle(bundle);
			bundle.schedSend(server, clock);
		} {
			loaded = false;
		};
	
	}
	
	// derive names and default args from synthDefs
	supplementNodeMap { arg keys, replaceOldKeys=false;
		objects.do { |obj|
				var controlNames, excluded;
				controlNames = obj.controlNames.asCollection;
				excluded = #[\out, \i_out, \gate];
				controlNames.do { arg cn,i;
					var name;
					name = cn.name; 
					if (
						excluded.includes(name).not 
						and: { replaceOldKeys or: { nodeMap.at(name).isNil } }
						and: { keys.isNil or: { keys.includes(name) } }
					) { nodeMap.set(name, cn.defaultValue) }
				};
		};
	}		
	generateUniqueName {
			^asString(this.identityHash.abs)
	}
	
	
	///////////////////// tasks ////////////////////////
	
	// sends all objects, frees the last ones first.
	
	spawner { arg beats=1, argFunc;
		var dt;
		if(beats.isNil) { ^this.freeSpawn };
		dt = beats.asStream;
		argFunc = if(argFunc.notNil) { argFunc.asArgStream } { #[] };
		this.task = Task.new { 
					inf.do { arg i;
						var t, args;
						if((t = dt.next).isNil, { nil.alwaysYield });
						args = argFunc.next(i, beats);
						this.send(args ++ ["i", i, "beats", thisThread.beats]); 
						t.wait; 
						}
					};
		awake = false;
	}
	
	// spawns synth at index, assumes fixed time envelope
	
	gspawner { arg beats=1, argFunc, index=0;
		var dt;
		if(beats.isNil) { ^this.freeSpawn };
		dt = beats.asStream;
		argFunc = if(argFunc.notNil) { argFunc.asArgStream } { #[] };
		index = index.loop.asStream;
		awake = true;
		this.task = Task.new { 
					inf.do { arg i;	
						var t;
						if((t = dt.next).isNil, { nil.alwaysYield });
						this.spawn(
							argFunc.value(i, beats) ++ ["i", i, "beats", thisThread.beats], 
							index.next
						); 
						t.wait; 
						}
					};
		awake = false;
	}
	
	freeSpawn { awake = true; this.task = nil; }
	
	/////////// filtering within one proxy /////////////////
	
	filter { arg i, func;
		var ok, ugen;
		if(this.isNeutral) { 
			ugen = func.value(Silent.ar);
			ok = this.initBus(ugen.rate, ugen.numChannels);
			if(ok.not) { ^nil }
		};
		
		this.put(i) { arg out;
			var e;
			e = EnvGate.new * Control.names(["wet"++i]).kr(1.0);
			if(this.rate === 'audio') {
				XOut.ar(out, e, SynthDef.wrap(func, nil, [In.ar(out, this.numChannels)]))
			} {
				XOut.kr(out, e, SynthDef.wrap(func, nil, [In.kr(out, this.numChannels)]))			};
		}
	}
	
	/////////// shortcuts for efficient bus input //////////////
	
	readFromBus { arg busses;
		var n, x;
		busses = busses.asCollection;
		n = this.numChannels;
		busses.do { arg item, i;
			x = min(item.numChannels ? n, n);
			this.put(i,
				SynthControl.new("system_link_" ++ this.rate ++ "_" ++ x), 
				0, 
				["in", item.index, "out", this.index]
			)
		};
	}
	
	read { arg proxies;
		proxies = proxies.asCollection;
		proxies.do { arg item; item.wakeUp };
		this.readFromBus(proxies)
	}
	
	
	/////// send and spawn //////////
	
	
	getBundle {
		var bundle;
		nodeMap.updateBundle;
		bundle = 	MixedBundle.new; 
		this.prepareToBundle(nil, bundle);
		^bundle
	}
		
	spawn { arg extraArgs, index=0;
			var bundle, obj, i;
			obj = objects.at(index);
			if(obj.notNil) {
				i = this.index;
				bundle = this.getBundle;
				obj.spawnToBundle(bundle, extraArgs, this);
				nodeMap.setToBundle(bundle, -1);
				nodeMap.mapToBundle(bundle, -1);
				bundle.schedSend(server);
			}
	}
	
	
	send { arg extraArgs, index, freeLast=true;
			var bundle, obj;
			if(index.isNil) { 
				bundle = this.getBundle;
				if(freeLast, { this.stopAllToBundle(bundle) });
				this.sendAllToBundle(bundle, extraArgs);
				bundle.schedSend(server);
			
			} {
				obj = objects.at(index);
				if(obj.notNil) {
					bundle = this.getBundle;
					if(freeLast, { this.stopToBundle(bundle, index) });
					
					this.sendObjectToBundle(bundle, obj, extraArgs, index);
					bundle.schedSend(server);
				}
			}
	}
	
	sendAll { arg extraArgs, freeLast=true; // better use send.
		this.send(extraArgs, nil, freeLast);
	}
	
	sendEach { arg extraArgs, freeLast=true;
			var bundle;
			bundle = this.getBundle;
			if(freeLast, { this.stopAllToBundle(bundle) });
			this.sendEachToBundle(bundle, extraArgs);
			bundle.schedSend(server);
	
	}
	
	wakeUp { 	if(this.isPlaying.not) { this.deepWakeUp } }
	
	deepWakeUp {
				var bundle;
				bundle = MixedBundle.new;
				this.wakeUpToBundle(bundle);
				bundle.schedSend(server, clock)
	}
		
		
	/////// append to bundle commands
	
	
	defaultGroupID { ^server.nextNodeID } //shared proxy support
	
	prepareToBundle { arg argGroup, bundle;
		if(this.isPlaying.not) {
				group = Group.basicNew(server, this.defaultGroupID);
				NodeWatcher.register(group);
				group.isPlaying = server.serverRunning;
				bundle.add(group.newMsg(argGroup ? server.asGroup, \addToHead));
		}
	}

	//apply the node map settings to the entire group
	sendAllToBundle { arg bundle, extraArgs;
				objects.do { arg item;
					item.playToBundle(bundle, extraArgs.value, this);
				};
				if(objects.notEmpty) { nodeMap.addToBundle(bundle, group) };
	}
	
	//apply the node map settings to each synth separately
	sendEachToBundle { arg bundle, extraArgs;
				objects.do { arg item;
					this.sendObjectToBundle(bundle, item, extraArgs.value)
				};
	}
	
	//send single object
	sendObjectToBundle { arg bundle, object, extraArgs, index;
				var synthID, target, nodes;
				synthID = object.playToBundle(bundle, extraArgs.value, this);
				if(synthID.notNil) {
					if(index.notNil) { // if nil, all are sent anyway
					// make list of nodeIDs following the index
					nodes = Array(8);
					objects.doFrom({ arg obj, i; 
						var id; id = obj.nodeID;
						if(id.notNil) { nodes = nodes ++ synthID ++ id };
					}, index + 1);
					if(nodes.size > 0) { bundle.add(["/n_before"] ++ nodes) };
				};
				nodeMap.addToBundle(bundle, synthID)
				};
	}
	
	stopToBundle { arg bundle, index;
		var obj;
		obj = objects.at(index);
		if(obj.notNil) { obj.stopToBundle(bundle, this.fadeTime) };
	}
	
	removeToBundle { arg bundle, index;
		var obj, dt, playing;
		playing = this.isPlaying;
		obj = objects.removeAt(index);
		if(obj.notNil) { 
				dt = this.fadeTime;
				if(playing) { obj.stopToBundle(bundle, dt) };
				obj.freeToBundle(bundle, dt);
		};
		
	}
	
	removeAllToBundle { arg bundle;
		var dt, playing;
		dt = this.fadeTime;
		playing = this.isPlaying;
		objects.do { arg obj; 
				if(playing) { obj.stopToBundle(bundle, dt) }; 
				obj.freeToBundle(bundle, dt);
		};
		objects.makeEmpty;
	}
	
	stopAllToBundle { arg bundle;
		var obj, dt;
		dt = this.fadeTime;
		if(this.isPlaying) { objects.do { arg obj; obj.stopToBundle(bundle, dt) } };
	}
	
	
	loadToBundle { arg bundle;
		if(bus.notNil) { bus.realloc }; // if server was rebooted 
		objects.do { arg item, i;
			item.build(this, i);
			item.loadToBundle(bundle);
		};
		loaded = true;
	}

	
	wakeUpToBundle { arg bundle, checkedAlready;
		if(checkedAlready.isNil) { checkedAlready = IdentitySet.new };
		if(checkedAlready.includes(this).not) {
			checkedAlready.add(this);
			this.wakeUpParentsToBundle(bundle, checkedAlready);
			if(loaded.not) { this.loadToBundle(bundle) };
			
			if(awake and: { this.isPlaying.not }) { 
				this.prepareToBundle(nil, bundle);
				this.sendAllToBundle(bundle)
			};
			
			this.playTaskToBundle(bundle);
		};
		
	}
	
	wakeUpParentsToBundle { arg bundle, checkedAlready;
			objects.do { arg item; item.wakeUpParentsToBundle(bundle, checkedAlready) };
			nodeMap.wakeUpParentsToBundle(bundle, checkedAlready);
	}
	
	playTaskToBundle { arg bundle;
		if(task.notNil and: { task.isPlaying.not }) 
			{ bundle.addAction(task, \play, [clock, false, 0.0]) }
	}
	
	// used in 'garbage collector'
	
	getFamily { arg set, alreadyAsked;
		var parents;
		parents = IdentitySet.new;
		alreadyAsked = alreadyAsked ?? { IdentitySet.new };
		alreadyAsked.add(this);
		objects.do { arg obj; parents.addAll(obj.parents) };
		parents.addAll(nodeMap.parents);
		parents.do { arg proxy; 
			if(alreadyAsked.includes(proxy).not) { proxy.getFamily(parents, alreadyAsked) }
		};
		set.add(this);
		set.addAll(parents);
		^set 	
	}
		
	
	////// private /////
	
	
	prepareOutput {
		var parentPlaying;
		parentPlaying = this.addToChild;
		if(parentPlaying) { this.deepWakeUp };
	}
	
	addToChild {
		var child;
		child = buildProxyControl;
		if(child.notNil and: { child !== this }) { child.addParent(this) };
		^child.isPlaying;
	}
	
	
	isPlaying { ^group.isPlaying }
	
	free { arg fadeTime, freeGroup = true;
		var bundle;
		if(this.isPlaying, {	
			bundle = MixedBundle.new;
			if(fadeTime.notNil) { bundle.add(["/n_set", group.nodeID, "fadeTime", fadeTime]) };
			this.stopAllToBundle(bundle);
			if(freeGroup) { 
				bundle.addSchedFunction({ group.free; task.stop }, 
					(fadeTime ? this.fadeTime) + server.latency); 
			};
			bundle.send(server);
		})
 	}
 	
 	
	////////////behave like my group////////////
	
	
	release { arg fadeTime; this.free(fadeTime, false) }
	
	
	set { arg ... args; // pairs of keys or indices and value
		nodeMap.set(*args);
		if(this.isPlaying) { 
			server.sendBundle(server.latency, [15, group.nodeID] ++ args); 
		};
	}
	
	setn { arg ... args;
		nodeMap.setn(*args);
		if(this.isPlaying) { group.setn(*args) };
	}
	
	// map to a control proxy
	map { arg ... args; // key, proxy ... args; 
		nodeMap.map(*args);
		if(this.isPlaying) { 
			nodeMap.updateBundle;
			server.sendBundle(server.latency, [14, group.nodeID] ++ nodeMap.mapArgs);
		}
	}
	
	// map to current environment
	mapEnvir { arg keys;
		nodeMap.mapEnvir(keys);
		if(this.isPlaying) { 
			nodeMap.sendToNode(group);
		}
	}
	
		
	unset { arg ... keys;
		var all;
		all = keys.isEmpty;
		if(all) { keys = nodeMap.settingKeys };
		nodeMap.unset(*keys);
		if(all) { this.linkNodeMap };
		if(this.isPlaying) { this.send(nil, nil, true) };
	}
	
	unmap { arg ... keys;
		if(keys.isEmpty) { keys = nodeMap.mappingKeys };
		nodeMap.unmap(*keys);
		if(this.isPlaying) {
			keys.do { arg key; group.map(key,-1) };
			nodeMap.sendToNode(group) 
		};
	}
	
	
	// xfades
	
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
		if(this.isPlaying) 
		{ this.sendEach(nil, true) } 
		{ "not playing".inform }
	}
	
	
	
	//////////////   behave like my bus //////////////////
	
	gate { arg level=1.0, dur=0;
		if(bus.notNil && this.isPlaying, { bus.gate(level, dur, group) }) 
		{ "not playing".inform };
	}
	
	line { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.line(level, dur, group) })
		{ "not playing".inform };
	}
	
	xline { arg level=1.0, dur=1.0;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.xline(level, dur, group) })
		{ "not playing".inform };
	}
	
	env { arg env;
		if(bus.notNil && this.isPlaying, { group.freeAll; bus.env(env) })
		{ "not playing".inform };
	}
		
	gateAt { arg key, level=1.0, dur=1.0;
		var oldLevel;
		oldLevel = nodeMap.at(key).value; // revisit: maybe simply set group?
		this.set(key, level); 
		SystemClock.sched(dur, {
			if(oldLevel.notNil) { this.set(key, oldLevel) } { this.unset(key) }; 
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
		var ctlBus, bundle, id, setArgs, setBundle, ctlIndex, missing;
		if(this.isPlaying) {
			durs = durs ? this.fadeTime;
			id = group.nodeID;
			keys = keys.asArray; levels = levels.asArray; durs = durs.asArray;

			ctlBus = Bus.alloc('control', server, keys.size);
			ctlIndex = ctlBus.index;
			bundle = ["/n_map", id];
			keys.do { arg key, i; bundle = bundle.addAll([key, ctlIndex + i]) };
			missing = keys.select { arg key; nodeMap.settings[key].isNil };
			if(missing.notEmpty) { 
				this.supplementNodeMap(missing); 
				nodeMap.addToBundle(bundle, group) 
			};
			server.sendBundle(server.latency, bundle);
				
			ctlBus.perform(action, levels, durs);
			ctlBus.free;
				
			setArgs = [keys, levels].flop.flat;
				// set the node map
			nodeMap.set(*setArgs);
				// finally set it to that vealue
			server.sendBundle(server.latency + durs.maxItem, 
					 ["/n_map", id] ++ [keys, -1].flop.flat,
					 ["/n_set", id] ++ setArgs
				);
		} { "not playing".inform };
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
		Library.at(this).do { arg item; item.do { arg item; item.clear } };
	}
	
	toLib { arg key;
		Library.put(this.class, server, key, this);
	}
	*at { arg server, key;
		^Library.at(this, server, key)
	}
}




// the server needs to be a BroadcastServer.
// this class takes care for a constant groupID.


SharedNodeProxy : NodeProxy { // should pass in a bus index/numChannels.
	var <constantGroupID;
	var <>allowMultipleObjects=true; // in larger groups this is better set to false
								// to limit node not found messages
	
	*new { arg server, groupID;
		^super.newCopyArgs(server).initGroupID(groupID).init	}
	
	shared { ^true }
	
	initGroupID { arg groupID;  
		constantGroupID = groupID ?? { server.nextSharedNodeID };
		awake = true;
	}
	
	// todo: rebuild! (bus identity needs to be fix)
	
	localServer { ^server.localServer }
	
	generateUniqueName {
		^asString(constantGroupID)
	}
	
	shouldAddObject { arg obj, index;
	
			^if(allowMultipleObjects.not and: {index.notNil} and: { index > 0 }) { 
				"only one object per proxy in shared node proxy possible".inform;
				^false
			} {
				if(obj.distributable.not) { 
					"this type of input is not distributable in a shared node proxy".inform;
					false
				} {
					obj.readyForPlay 
				}
			}
	}

	// map to a control proxy
	map { arg key, proxy ... args;
		args = [key,proxy]++args;
		// check if any not shared proxy is passed in
		(args.size div: 2).do { arg i; 
			if(args[2*i+1].shared.not, { Error("shouldn't map a local to a shared proxy").throw }) 
		};
		nodeMap.map(*args);
		if(this.isPlaying) { nodeMap.sendToNode(group) }
	}
	
	mapEnvir {}
	
	// use shared node proxy only with functions that can release the synth.
	// this is checked and throws an error in addObj
	stopAllToBundle { arg bundle;
			bundle.add(["/n_set", constantGroupID, "gate", 0])
	}
	stopToBundle { arg bundle, index;
				if(allowMultipleObjects) 
					{ super.stopToBundle(bundle, index) } 
					{ bundle.add(["/n_set", constantGroupID, "gate", 0]) }
	}
	group_ {}
	bus_ {}
	
	

	///////////////////
	
	prepareToBundle { arg argGroup, bundle; // ignore ingroup
		if(this.isPlaying.not) {
				group = Group.basicNew(
					this.localServer,   // NodeWatcher should know when local group stopped
					this.constantGroupID // but not care about any remote groups
				);
				group.isPlaying = true;
				NodeWatcher.register(group);
		};
		bundle.add(["/g_new", constantGroupID]); // duplicate sending is no problem
	}
	


}


