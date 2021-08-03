ServerOptions {
	classvar <defaultValues;

	// order of variables is important here. Only add new instance variables to the end.
	var <numAudioBusChannels;
	var <>numControlBusChannels;
	var <numInputBusChannels;
	var <numOutputBusChannels;
	var <>numBuffers;

	var <>maxNodes;
	var <>maxSynthDefs;
	var <>protocol;
	var <>blockSize;
	var <>hardwareBufferSize;

	var <>memSize;
	var <>numRGens;
	var <>numWireBufs;

	var <>sampleRate;
	var <>loadDefs;

	var <>inputStreamsEnabled;
	var <>outputStreamsEnabled;

	var <>inDevice;
	var <>outDevice;

	var <>verbosity;
	var <>zeroConf; // Whether server publishes port to Bonjour, etc.

	var <>restrictedPath;
	var <>ugenPluginsPath;

	var <>initialNodeID;
	var <>remoteControlVolume;

	var <>memoryLocking;
	var <>threads; // for supernova
	var <>useSystemClock;  // for supernova

	var <numPrivateAudioBusChannels;

	var <>reservedNumAudioBusChannels;
	var <>reservedNumControlBusChannels;
	var <>reservedNumBuffers;
	var <>pingsBeforeConsideredDead;

	var <>maxLogins;

	var <>recHeaderFormat;
	var <>recSampleFormat;
	var <>recChannels;
	var <>recBufSize;

	var <>bindAddress;

	var <>safetyClipThreshold;

	*initClass {
		defaultValues = IdentityDictionary.newFrom(
			(
				numAudioBusChannels: 1024, // see corresponding setter method below
				numControlBusChannels: 16384,
				numInputBusChannels: 2, // see corresponding setter method below
				numOutputBusChannels: 2, // see corresponding setter method below
				numBuffers: 1024,
				maxNodes: 1024,
				maxSynthDefs: 1024,
				protocol: \udp,
				blockSize: 64,
				hardwareBufferSize: nil,
				memSize: 8192,
				numRGens: 64,
				numWireBufs: 64,
				sampleRate: nil,
				loadDefs: true,
				inputStreamsEnabled: nil,
				outputStreamsEnabled: nil,
				inDevice: nil,
				outDevice: nil,
				verbosity: 0,
				zeroConf: false,
				restrictedPath: nil,
				ugenPluginsPath: nil,
				initialNodeID: 1000,
				remoteControlVolume: false,
				memoryLocking: false,
				threads: nil,
				useSystemClock: true,
				numPrivateAudioBusChannels: 1020, // see corresponding setter method below
				reservedNumAudioBusChannels: 0,
				reservedNumControlBusChannels: 0,
				reservedNumBuffers: 0,
				pingsBeforeConsideredDead: 5,
				maxLogins: 1,
				recHeaderFormat: "aiff",
				recSampleFormat: "float",
				recChannels: 2,
				recBufSize: nil,
				bindAddress: "127.0.0.1",
				safetyClipThreshold: 1.26 // ca. 2 dB
			)
		)
	}

	*new {
		^super.new.init
	}

	init {
		defaultValues.keysValuesDo { |key, val| this.instVarPut(key, val) }
	}

	device {
		^if(inDevice == outDevice) {
			inDevice
		} {
			[inDevice, outDevice]
		}
	}

	device_ { |dev|
		inDevice = outDevice = dev;
	}

	asOptionsString { | port = 57110 |
		var o;
		o = if(protocol == \tcp, " -t ", " -u ");
		o = o ++ port;

		o = o ++ " -a " ++ (numPrivateAudioBusChannels + numInputBusChannels + numOutputBusChannels) ;
		o = o ++ " -i " ++ numInputBusChannels;
		o = o ++ " -o " ++ numOutputBusChannels;

		if (bindAddress != defaultValues[\bindAddress], {
			o = o ++ " -B " ++ bindAddress;
		});
		if (numControlBusChannels !== defaultValues[\numControlBusChannels], {
			numControlBusChannels = numControlBusChannels.asInteger;
			o = o ++ " -c " ++ numControlBusChannels;
		});
		if (numBuffers !== defaultValues[\numBuffers], {
			numBuffers = numBuffers.asInteger;
			o = o ++ " -b " ++ numBuffers;
		});
		if (maxNodes !== defaultValues[\maxNodes], {
			maxNodes = maxNodes.asInteger;
			o = o ++ " -n " ++ maxNodes;
		});
		if (maxSynthDefs !== defaultValues[\maxSynthDefs], {
			maxSynthDefs = maxSynthDefs.asInteger;
			o = o ++ " -d " ++ maxSynthDefs;
		});
		if (blockSize !== defaultValues[\blockSize], {
			blockSize = blockSize.asInteger;
			o = o ++ " -z " ++ blockSize;
		});
		if (hardwareBufferSize.notNil, {
			o = o ++ " -Z " ++ hardwareBufferSize;
		});
		if (memSize !== defaultValues[\memSize], {
			memSize = memSize.asInteger;
			o = o ++ " -m " ++ memSize;
		});
		if (numRGens !== defaultValues[\numRGens], {
			numRGens = numRGens.asInteger;
			o = o ++ " -r " ++ numRGens;
		});
		if (numWireBufs !== defaultValues[\numWireBufs], {
			numWireBufs = numWireBufs.asInteger;
			o = o ++ " -w " ++ numWireBufs;
		});
		if (sampleRate.notNil, {
			o = o ++ " -S " ++ sampleRate;
		});
		if (loadDefs.not, {
			o = o ++ " -D 0";
		});
		if (inputStreamsEnabled.notNil, {
			o = o ++ " -I " ++ inputStreamsEnabled ;
		});
		if (outputStreamsEnabled.notNil, {
			o = o ++ " -O " ++ outputStreamsEnabled ;
		});
		if (inDevice == outDevice)
		{
			if (inDevice.notNil,
				{
					o = o ++ " -H %".format(inDevice.quote);
			});
		}
		{
			o = o ++ " -H % %".format((inDevice ? "").asString.quote, (outDevice ? "").asString.quote);
		};
		if (verbosity != defaultValues[\verbosity], {
			o = o ++ " -V " ++ verbosity;
		});
		if (zeroConf.not, {
			o = o ++ " -R 0";
		});
		if (restrictedPath.notNil, {
			o = o ++ " -P " ++ restrictedPath;
		});
		if (ugenPluginsPath.notNil, {
			if(ugenPluginsPath.isString, {
				ugenPluginsPath = ugenPluginsPath.bubble;
			});
			o = o ++ " -U " ++ ugenPluginsPath.collect{|p|
				thisProcess.platform.formatPathForCmdLine(p)
			}.join(Platform.pathDelimiter);
		});
		if (memoryLocking, {
			o = o ++ " -L";
		});
		if (threads.notNil, {
			if (Server.program.asString.endsWith("supernova")) {
				o = o ++ " -T " ++ threads;
			}
		});
		if (useSystemClock, {
			o = o ++ " -C 1"
		}, {
			o = o ++ " -C 0"
		});
		if (maxLogins.notNil, {
			o = o ++ " -l " ++ maxLogins;
		});
		if (thisProcess.platform.name === \osx && safetyClipThreshold.notNil, {
			o = o ++ " -s " ++ safetyClipThreshold;
		});
		^o
	}

	firstPrivateBus { // after the outs and ins
		^numOutputBusChannels + numInputBusChannels
	}

	bootInProcess {
		_BootInProcessServer
		^this.primitiveFailed
	}

	numPrivateAudioBusChannels_ { |numChannels = 1020| // arg default value should match defaultValues above
		numPrivateAudioBusChannels = numChannels;
		this.recalcChannels;
	}

	numAudioBusChannels_ { |numChannels = 1024| // arg default value should match defaultValues above
		numAudioBusChannels = numChannels;
		numPrivateAudioBusChannels = numAudioBusChannels - numInputBusChannels - numOutputBusChannels;
	}

	numInputBusChannels_ { |numChannels = 2| // arg default value should match defaultValues above
		numInputBusChannels = numChannels;
		this.recalcChannels;
	}

	numOutputBusChannels_ { |numChannels = 2| // arg default value should match defaultValues above
		numOutputBusChannels = numChannels;
		this.recalcChannels;
	}

	recalcChannels {
		numAudioBusChannels = numPrivateAudioBusChannels + numInputBusChannels + numOutputBusChannels;
	}

	*prListDevices {
		arg in, out;
		_ListAudioDevices
		^this.primitiveFailed
	}

	*devices {
		^this.prListDevices(1, 1);
	}

	*inDevices {
		^this.prListDevices(1, 0);
	}

	*outDevices {
		^this.prListDevices(0, 1);
	}
}


ServerShmInterface {
	// order matters!
	var ptr, finalizer;

	*new {|port|
		^super.new.connect(port)
	}

	copy {
		// never ever copy! will cause duplicate calls to the finalizer!
		^this
	}

	connect {
		_ServerShmInterface_connectSharedMem
		^this.primitiveFailed
	}

	disconnect {
		_ServerShmInterface_disconnectSharedMem
		^this.primitiveFailed
	}

	getControlBusValue {
		_ServerShmInterface_getControlBusValue
		^this.primitiveFailed
	}

	getControlBusValues {
		_ServerShmInterface_getControlBusValues
		^this.primitiveFailed
	}

	setControlBusValue {
		_ServerShmInterface_setControlBusValue
		^this.primitiveFailed
	}

	setControlBusValues {
		_ServerShmInterface_setControlBusValues
		^this.primitiveFailed
	}
}


Server {

	classvar <>local, <>internal, <default;
	classvar <>named, <>all, <>program, <>sync_s = true;
	classvar <>nodeAllocClass, <>bufferAllocClass, <>busAllocClass;
	classvar <>defaultOptionsClass;

	var <name, <addr, <clientID;
	var <isLocal, <inProcess, <>sendQuit, <>remoteControlled;
	var maxNumClients; // maxLogins as sent from booted scsynth

	var <>options, <>latency = 0.2, <dumpMode = 0;

	var <nodeAllocator, <controlBusAllocator, <audioBusAllocator, <bufferAllocator, <scopeBufferAllocator;

	var <>tree;
	var <defaultGroup, <defaultGroups;

	var <syncThread, <syncTasks;
	var <window, <>scopeWindow, <emacsbuf;
	var <volume, <recorder, <statusWatcher;
	var <pid, serverInterface;
	var pidReleaseCondition;

	*initClass {
		Class.initClassTree(ServerOptions);
		Class.initClassTree(NotificationCenter);
		named = IdentityDictionary.new;
		all = Set.new;

		nodeAllocClass = NodeIDAllocator;
		// nodeAllocClass = ReadableNodeIDAllocator;
		bufferAllocClass = ContiguousBlockAllocator;
		busAllocClass = ContiguousBlockAllocator;

		defaultOptionsClass = if(Platform.hasBelaSupport, BelaServerOptions, ServerOptions);

		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		internal = Server.new(\internal, NetAddr.new);
	}

	*fromName { |name|
		^Server.named[name] ?? {
			Server(name, NetAddr.new("127.0.0.1", 57110))
		}
	}

	*default_ { |server|
		default = server;
		if(sync_s) { thisProcess.interpreter.s = server };  // sync with s?
		this.all.do { |each| each.changed(\default, server) };
	}

	*new { |name, addr, options, clientID|
		^super.new.init(name, addr, options, clientID)
	}

	*remote { |name, addr, options, clientID|
		var result;
		result = this.new(name, addr, options, clientID);
		if(options.protocol == \tcp) {
			addr.tryConnectTCP({ result.startAliveThread }, nil, 20)
		} {
			result.startAliveThread;
		};
		^result;
	}

	init { |argName, argAddr, argOptions, argClientID|
		this.addr = argAddr;
		options = argOptions ?? { defaultOptionsClass.new };

		// set name to get readable posts from clientID set
		name = argName.asSymbol;

		// make statusWatcher before clientID, so .serverRunning works
		statusWatcher = ServerStatusWatcher(server: this);

		// go thru setter to test validity
		this.clientID = argClientID ? 0;

		volume = Volume(server: this, persist: true);
		recorder = Recorder(server: this);
		recorder.notifyServer = true;

		this.name = argName;
		all.add(this);

		pidReleaseCondition = Condition({ this.pid == nil });

		Server.changed(\serverAdded, this);

	}

	maxNumClients { ^maxNumClients ?? { options.maxLogins } }

	remove {
		all.remove(this);
		named.removeAt(this.name);
	}

	addr_ { |netAddr|
		addr = netAddr ?? { NetAddr("127.0.0.1", 57110) };
		inProcess = addr.addr == 0;
		isLocal = inProcess || { addr.isLocal };
		remoteControlled = isLocal.not;
	}

	name_ { |argName|
		name = argName.asSymbol;
		if(named.at(argName).notNil) {
			"Server name already exists: '%'. Please use a unique name".format(name, argName).warn;
		} {
			named.put(name, this);
		}
	}

	initTree {
		fork({
			this.sendDefaultGroups;
			tree.value(this);
			this.sync;
			ServerTree.run(this);
			this.sync;
		}, AppClock);
	}

	/* clientID */

	// clientID is settable while server is off, and locked while server is running
	// called from prHandleClientLoginInfoFromServer once after booting.
	clientID_ { |val|
		var failstr = "Server % couldn't set clientID to % - %. clientID is still %.";
		if (this.serverRunning) {
			failstr.format(name, val.cs, "server is running", clientID).warn;
			^this
		};

		if(val.isInteger.not) {
			failstr.format(name, val.cs, "not an Integer", clientID).warn;
			^this
		};
		if (val < 0 or: { val >= this.maxNumClients }) {
			failstr.format(name,
				val.cs,
				"outside of allowed server.maxNumClients range of 0 - %".format(this.maxNumClients - 1),
				clientID
			).warn;
			^this
		};

		if (clientID != val) {
			"% : setting clientID to %.\n".postf(this, val);
		};
		clientID = val;
		this.newAllocators;
	}

	/* clientID-based id allocators */

	newAllocators {
		this.newNodeAllocators;
		this.newBusAllocators;
		this.newBufferAllocators;
		this.newScopeBufferAllocators;
		NotificationCenter.notify(this, \newAllocators);
	}

	newNodeAllocators {
		nodeAllocator = nodeAllocClass.new(
			clientID,
			options.initialNodeID,
			this.maxNumClients
		);
		// defaultGroup and defaultGroups depend on allocator,
		// so always make them here:
		this.makeDefaultGroups;
	}

	newBusAllocators {
		var numControlPerClient, numAudioPerClient;
		var controlReservedOffset, controlBusClientOffset;
		var audioReservedOffset, audioBusClientOffset;

		var audioBusIOOffset = options.firstPrivateBus;

		numControlPerClient = options.numControlBusChannels div: this.maxNumClients;
		numAudioPerClient = options.numAudioBusChannels - audioBusIOOffset div: this.maxNumClients;

		controlReservedOffset = options.reservedNumControlBusChannels;
		controlBusClientOffset = numControlPerClient * clientID;

		audioReservedOffset = options.reservedNumAudioBusChannels;
		audioBusClientOffset = numAudioPerClient * clientID;

		controlBusAllocator = busAllocClass.new(
			numControlPerClient,
			controlReservedOffset,
			controlBusClientOffset
		);
		audioBusAllocator = busAllocClass.new(
			numAudioPerClient,
			audioReservedOffset,
			audioBusClientOffset + audioBusIOOffset
		);
	}

	newBufferAllocators {
		var numBuffersPerClient = options.numBuffers div: this.maxNumClients;
		var numReservedBuffers = options.reservedNumBuffers;
		var bufferClientOffset = numBuffersPerClient * clientID;

		bufferAllocator = bufferAllocClass.new(
			numBuffersPerClient,
			numReservedBuffers,
			bufferClientOffset
		);
	}

	newScopeBufferAllocators {
		if(isLocal) {
			scopeBufferAllocator = StackNumberAllocator.new(0, 127)
		}
	}

	nextBufferNumber { |n|
		var bufnum = bufferAllocator.alloc(n);
		if(bufnum.isNil) {
			if(n > 1) {
				Error("No block of % consecutive buffer numbers is available.".format(n)).throw
			} {
				Error("No more buffer numbers -- free some buffers before allocating more.").throw
			}
		};
		^bufnum
	}

	freeAllBuffers {
		var bundle;
		bufferAllocator.blocks.do { arg block;
			(block.address .. block.address + block.size - 1).do { |i|
				bundle = bundle.add( ["/b_free", i] );
			};
			bufferAllocator.free(block.address);
		};
		this.sendBundle(nil, *bundle);
	}

	nextNodeID {
		^nodeAllocator.alloc
	}

	nextPermNodeID {
		^nodeAllocator.allocPerm
	}

	freePermNodeID { |id|
		^nodeAllocator.freePerm(id)
	}

	prHandleClientLoginInfoFromServer { |newClientID, newMaxLogins|

		// only set maxLogins if not internal server
		if (inProcess.not) {
			if (newMaxLogins.notNil) {
				if (newMaxLogins != options.maxLogins) {
					"%: server process has maxLogins % - adjusting my options accordingly.\n"
					.postf(this, newMaxLogins);
				} {
					"%: server process's maxLogins (%) matches with my options.\n"
					.postf(this, newMaxLogins);
				};
				options.maxLogins = maxNumClients = newMaxLogins;
			} {
				"%: no maxLogins info from server process.\n"
				.postf(this, newMaxLogins);
			};
		};
		if (newClientID.notNil) {
			if (newClientID == clientID) {
				"%: keeping clientID (%) as confirmed by server process.\n"
				.postf(this, newClientID);
			} {
				"%: setting clientID to %, as obtained from server process.\n"
				.postf(this, newClientID);
			};
			this.clientID = newClientID;
		};
	}

	prHandleNotifyFailString {|failString, msg|

		// post info on some known error cases
		case
		{ failString.asString.contains("already registered") } {
			// when already registered, msg[3] is the clientID by which
			// the requesting client was registered previously
			"% - already registered with clientID %.\n".postf(this, msg[3]);
			statusWatcher.prHandleLoginWhenAlreadyRegistered(msg[3]);

		} { failString.asString.contains("not registered") } {
			// unregister when already not registered:
			"% - not registered.\n".postf(this);
			statusWatcher.notified = false;
		} { failString.asString.contains("too many users") } {
			"% - could not register, too many users.\n".postf(this);
			statusWatcher.notified = false;
		} {
			// throw error if unknown failure
			Error(
				"Failed to register with server '%' for notifications: %\n"
				"To recover, please reboot the server.".format(this, msg)).throw;
		};
	}

	/* network messages */

	sendMsg { |... msg|
		addr.sendMsg(*msg)
	}

	sendBundle { |time ... msgs|
		addr.sendBundle(time, *msgs)
	}

	sendRaw { |rawArray|
		addr.sendRaw(rawArray)
	}

	sendMsgSync { |condition ... args|
		var cmdName, resp;
		if(condition.isNil) { condition = Condition.new };
		cmdName = args[0].asString;
		if(cmdName[0] != $/) { cmdName = cmdName.insert(0, $/) };
		resp = OSCFunc({|msg|
			if(msg[1].asString == cmdName) {
				resp.free;
				condition.test = true;
				condition.signal;
			};
		}, '/done', addr);
		condition.test = false;
		addr.sendBundle(nil, args);
		condition.wait;
	}

	sync { |condition, bundles, latency| // array of bundles that cause async action
		addr.sync(condition, bundles, latency)
	}

	schedSync { |func|
		syncTasks = syncTasks.add(func);
		if(syncThread.isNil) {
			syncThread = Routine.run {
				var c = Condition.new;
				while { syncTasks.notEmpty } { syncTasks.removeAt(0).value(c) };
				syncThread = nil;
			}
		}
	}

	listSendMsg { |msg|
		addr.sendMsg(*msg)
	}

	listSendBundle { |time, msgs|
		addr.sendBundle(time, *(msgs.asArray))
	}

	reorder { |nodeList, target, addAction=\addToHead|
		target = target.asTarget;
		this.sendMsg(62, Node.actionNumberFor(addAction), target.nodeID, *(nodeList.collect(_.nodeID))) //"/n_order"
	}

	// load from disk locally, send remote
	sendSynthDef { |name, dir|
		var file, buffer;
		dir = dir ? SynthDef.synthDefDir;
		file = File(dir ++ name ++ ".scsyndef","r");
		if(file.isOpen.not) { ^nil };
		protect {
			buffer = Int8Array.newClear(file.length);
			file.read(buffer);
		} {
			file.close;
		};
		this.sendMsg("/d_recv", buffer);
	}

	// tell server to load from disk
	loadSynthDef { |name, completionMsg, dir|
		dir = dir ? SynthDef.synthDefDir;
		this.listSendMsg(
			["/d_load", dir ++ name ++ ".scsyndef", completionMsg ]
		)
	}

	//loadDir
	loadDirectory { |dir, completionMsg|
		this.listSendMsg(["/d_loadDir", dir, completionMsg])
	}


	/* network message bundling */

	openBundle { |bundle|	// pass in a bundle that you want to
		// continue adding to, or nil for a new bundle.
		if(addr.hasBundle) {
			bundle = addr.bundle.addAll(bundle);
			addr.bundle = []; // debatable
		};
		addr = BundleNetAddr.copyFrom(addr, bundle);
	}

	closeBundle { |time| // set time to false if you don't want to send.
		var bundle;
		if(addr.hasBundle) {
			bundle = addr.closeBundle(time);
			addr = addr.saveAddr;
		} {
			"there is no open bundle.".warn
		};
		^bundle;
	}

	makeBundle { |time, func, bundle|
		this.openBundle(bundle);
		try {
			func.value(this);
			bundle = this.closeBundle(time);
		} {|error|
			addr = addr.saveAddr; // on error restore the normal NetAddr
			error.throw
		}
		^bundle
	}

	bind { |func|
		^this.makeBundle(this.latency, func)
	}


	/* scheduling */

	wait { |responseName|
		var condition = Condition.new;
		OSCFunc({
			condition.test = true;
			condition.signal
		}, responseName, addr).oneShot;

		condition.wait
	}

	waitForBoot { |onComplete, limit = 100, onFailure|
		// onFailure.true: why is this necessary?
		// this.boot also calls doWhenBooted.
		// doWhenBooted prints the normal boot failure message.
		// if the server fails to boot, the failure error gets posted TWICE.
		// So, we suppress one of them.
		if(this.serverRunning.not) { this.boot(onFailure: true) };
		this.doWhenBooted(onComplete, limit, onFailure);
	}

	doWhenBooted { |onComplete, limit=100, onFailure|
		statusWatcher.doWhenBooted(onComplete, limit, onFailure)
	}

	ifRunning { |func, failFunc|
		^if(statusWatcher.unresponsive) {
			"server '%' not responsive".format(this.name).postln;
			failFunc.value(this)
		} {
			if(statusWatcher.serverRunning) {
				func.value(this)
			} {
				"server '%' not running".format(this.name).postln;
				failFunc.value(this)
			}
		}

	}

	ifNotRunning { |func|
		^ifRunning(this, nil, func)
	}


	bootSync { |condition|
		condition ?? { condition = Condition.new };
		condition.test = false;
		this.waitForBoot {
			// Setting func to true indicates that our condition has become true and we can go when signaled.
			condition.test = true;
			condition.signal
		};
		condition.wait;
	}


	ping { |n = 1, wait = 0.1, func|
		var result = 0, pingFunc;
		if(statusWatcher.serverRunning.not) { "server not running".postln; ^this };
		pingFunc = {
			Routine.run {
				var t, dt;
				t = Main.elapsedTime;
				this.sync;
				dt = Main.elapsedTime - t;
				("measured latency:" + dt + "s").postln;
				result = max(result, dt);
				n = n - 1;
				if(n > 0) {
					SystemClock.sched(wait, { pingFunc.value; nil })
				} {
					("maximum determined latency of" + name + ":" + result + "s").postln;
					func.value(result)
				}
			};
		};
		pingFunc.value;
	}

	cachedBuffersDo { |func|
		Buffer.cachedBuffersDo(this, func)
	}

	cachedBufferAt { |bufnum|
		^Buffer.cachedBufferAt(this, bufnum)
	}

	// keep defaultGroups for all clients on this server:
	makeDefaultGroups {
		defaultGroups = this.maxNumClients.collect { |clientID|
			Group.basicNew(this, nodeAllocator.numIDs * clientID + 1);
		};
		defaultGroup = defaultGroups[clientID];
	}

	defaultGroupID { ^defaultGroup.nodeID }

	sendDefaultGroups {
		defaultGroups.do { |group|
			this.sendMsg("/g_new", group.nodeID, 0, 0);
		};
	}

	sendDefaultGroupsForClientIDs { |clientIDs|
		defaultGroups[clientIDs].do { |group|
			this.sendMsg("/g_new", group.nodeID, 0, 0);
		}
	}

	inputBus {
		^Bus(\audio, this.options.numOutputBusChannels, this.options.numInputBusChannels, this)
	}

	outputBus {
		^Bus(\audio, 0, this.options.numOutputBusChannels, this)
	}

	/* recording formats */

	recHeaderFormat { ^options.recHeaderFormat }
	recHeaderFormat_ { |string| options.recHeaderFormat_(string) }
	recSampleFormat { ^options.recSampleFormat }
	recSampleFormat_ { |string| options.recSampleFormat_(string) }
	recChannels { ^options.recChannels }
	recChannels_ { |n| options.recChannels_(n) }
	recBufSize { ^options.recBufSize }
	recBufSize_ { |n| options.recBufSize_(n) }

	/* server status */

	numUGens { ^statusWatcher.numUGens }
	numSynths { ^statusWatcher.numSynths }
	numGroups { ^statusWatcher.numGroups }
	numSynthDefs { ^statusWatcher.numSynthDefs }
	avgCPU { ^statusWatcher.avgCPU }
	peakCPU { ^statusWatcher.peakCPU }
	sampleRate { ^statusWatcher.sampleRate }
	actualSampleRate { ^statusWatcher.actualSampleRate }
	hasBooted { ^statusWatcher.hasBooted }
	serverRunning { ^statusWatcher.serverRunning }
	serverBooting { ^statusWatcher.serverBooting }
	unresponsive { ^statusWatcher.unresponsive }
	startAliveThread { | delay=0.0 | statusWatcher.startAliveThread(delay) }
	stopAliveThread { statusWatcher.stopAliveThread }
	aliveThreadIsRunning { ^statusWatcher.aliveThread.isPlaying }
	aliveThreadPeriod_ { |val| statusWatcher.aliveThreadPeriod_(val) }
	aliveThreadPeriod { |val| ^statusWatcher.aliveThreadPeriod }

	disconnectSharedMemory {
		if(this.hasShmInterface) {
			"server '%' disconnected shared memory interface\n".postf(name);
			serverInterface.disconnect;
			serverInterface = nil;
		}
	}

	connectSharedMemory {
		var id;
		if(this.isLocal) {
			this.disconnectSharedMemory;
			id = if(this.inProcess) { thisProcess.pid } { addr.port };
			serverInterface = ServerShmInterface(id);
		}
	}

	hasShmInterface { ^serverInterface.notNil }

	*resumeThreads {
		all.do { |server| server.statusWatcher.resumeThread }
	}

	boot { | startAliveThread = true, recover = false, onFailure |

		if(statusWatcher.unresponsive) {
			"server '%' unresponsive, rebooting ...".format(this.name).postln;
			this.quit(watchShutDown: false)
		};
		if(statusWatcher.serverRunning) { "server '%' already running".format(this.name).postln; ^this };
		if(statusWatcher.serverBooting) { "server '%' already booting".format(this.name).postln; ^this };


		statusWatcher.serverBooting = true;

		statusWatcher.doWhenBooted({
			statusWatcher.serverBooting = false;
			this.bootInit(recover);
		}, onFailure: onFailure ? false);

		if(remoteControlled) {
			"You will have to manually boot remote server.".postln;
		} {
			this.prPingApp({
				this.quit;
				this.boot;
			}, {
				this.prWaitForPidRelease {
					this.bootServerApp({
						if(startAliveThread) { statusWatcher.startAliveThread }
					})
				};
			}, 0.25);
		}
	}

	prWaitForPidRelease { |onComplete, onFailure, timeout = 1|
		var waiting = true;
		if (this.inProcess or: { this.isLocal.not or: { this.pid.isNil } }) {
			onComplete.value;
			^this
		};

		// FIXME: quick and dirty fix for supernova reboot hang on macOS:
		// if we have just quit before running server.boot,
		// we wait until server process really ends and sets its pid to nil
		SystemClock.sched(timeout, {
			if (waiting) {
				pidReleaseCondition.unhang
			}
		});

		forkIfNeeded {
			pidReleaseCondition.hang;
			if (pidReleaseCondition.test.value) {
				waiting = false;
				onComplete.value;
			} {
				onFailure.value
			}
		}
	}

	// FIXME: recover should happen later, after we have a valid clientID!
	// would then need check whether maxLogins and clientID have changed or not,
	// and recover would only be possible if no changes.
	bootInit { | recover = false |
		// if(recover) { this.newNodeAllocators } {
		// 	"% calls newAllocators\n".postf(thisMethod);
		// this.newAllocators };
		if(dumpMode != 0) { this.sendMsg(\dumpOSC, dumpMode) };
		if(sendQuit.isNil) {
			sendQuit = this.inProcess or: { this.isLocal };
		};
		this.connectSharedMemory;
	}

	prOnServerProcessExit { |exitCode|
		pid = nil;
		pidReleaseCondition.signal;

		"Server '%' exited with exit code %."
			.format(this.name, exitCode)
			.postln;
		statusWatcher.quit(watchShutDown: false);
	}

	bootServerApp { |onComplete|
		if(inProcess) {
			"Booting internal server.".postln;
			this.bootInProcess;
			pid = thisProcess.pid;
			onComplete.value;
		} {
			this.disconnectSharedMemory;
			pid = unixCmd(program ++ options.asOptionsString(addr.port), { |exitCode|
				this.prOnServerProcessExit(exitCode);
			});
			("Booting server '%' on address %:%.").format(this.name, addr.hostname, addr.port.asString).postln;
			// in case the server takes more time to boot
			// we increase the number of attempts for tcp connection
			// in order to minimize the chance of timing out
			if(options.protocol == \tcp, { addr.tryConnectTCP(onComplete, nil, 20) }, onComplete);
		}
	}

	reboot { |func, onFailure| // func is evaluated when server is off
		if(isLocal.not) { "can't reboot a remote server".postln; ^this };
		if(statusWatcher.serverRunning and: { this.unresponsive.not }) {
			this.quit({
				func.value;
				defer { this.boot }
			}, onFailure);
		} {
			func.value;
			this.boot(onFailure: onFailure);
		}
	}

	applicationRunning {
		^pid.tryPerform(\pidRunning) == true
	}

	status {
		addr.sendStatusMsg // backward compatibility
	}

	sendStatusMsg {
		addr.sendStatusMsg
	}

	notify {
		^statusWatcher.notify
	}

	notify_ { |flag|
		statusWatcher.notify_(flag)
	}

	notified {
		^statusWatcher.notified
	}

	dumpOSC { |code = 1|
		/*
		0 - turn dumping OFF.
		1 - print the parsed contents of the message.
		2 - print the contents in hexadecimal.
		3 - print both the parsed and hexadecimal representations of the contents.
		*/
		dumpMode = code;
		this.sendMsg(\dumpOSC, code);
		this.changed(\dumpOSC, code);
	}

	quit { |onComplete, onFailure, watchShutDown = true|
		var func;

		addr.sendMsg("/quit");

		if(this.unresponsive) {
			if(pid.notNil) {
				"Server '%' is currently unresponsive. Forcing process to stop via system command.".format(name).postln;
				thisProcess.platform.killProcessByID(pid);
			} {
				if(watchShutDown) {
					"Server '%' was unresponsive. Quitting anyway.".format(name).postln;
				};
			};
			watchShutDown = false;
		};

		if(options.protocol == \tcp) {
			statusWatcher.quit({ addr.tryDisconnectTCP(onComplete, onFailure) }, nil, watchShutDown);
		} {
			statusWatcher.quit(onComplete, onFailure, watchShutDown);
		};

		if(inProcess) {
			this.quitInProcess;
			"Internal server has quit.".postln;
		} {
			"'/quit' message sent to server '%'.".format(name).postln;
		};

		sendQuit = nil;
		maxNumClients = nil;

		if(scopeWindow.notNil) { scopeWindow.quit };
		volume.freeSynth;
		RootNode(this).freeAll;
		this.newAllocators;
	}

	*quitAll { |watchShutDown = true|
		all.do { |server|
			if(server.sendQuit === true) {
				server.quit(watchShutDown: watchShutDown)
			}
		}
	}

	*killAll {
		// if you see Exception in World_OpenUDP: unable to bind udp socket
		// its because you have multiple servers running, left
		// over from crashes, unexpected quits etc.
		// you can't cause them to quit via OSC (the boot button)

		// this brutally kills them all off
		thisProcess.platform.name.switch(
			\windows, {
				thisProcess.platform.killAll("scsynth.exe");
				thisProcess.platform.killAll("supernova.exe");
			}, {
				thisProcess.platform.killAll("scsynth");
				thisProcess.platform.killAll("supernova");
			}
		);
		this.quitAll(watchShutDown: false);
	}

	freeAll {
		this.sendMsg("/g_freeAll", 0);
		this.sendMsg("/clearSched");
		this.initTree;
	}

	freeMyDefaultGroup {
		this.sendMsg("/g_freeAll", defaultGroup.nodeID);
	}

	freeDefaultGroups {
		defaultGroups.do { |group|
			this.sendMsg("/g_freeAll", group.nodeID);
		};
	}

	*freeAll { |evenRemote = false|
		if(evenRemote) {
			all.do { |server|
				if( server.serverRunning ) { server.freeAll }
			}
		} {
			all.do { |server|
				if(server.isLocal and:{ server.serverRunning }) { server.freeAll }
			}
		}
	}

	*hardFreeAll { |evenRemote = false|
		if(evenRemote) {
			all.do { |server|
				server.freeAll
			}
		} {
			all.do { |server|
				if(server.isLocal) { server.freeAll }
			}
		}
	}

	*allBootedServers {
		^this.all.select(_.hasBooted)
	}

	*allRunningServers {
		^this.all.select(_.serverRunning)
	}

	/* volume control */

	volume_ { | newVolume |
		volume.volume_(newVolume)
	}

	mute {
		volume.mute
	}

	unmute {
		volume.unmute
	}

	/* recording output */

	record { |path, bus, numChannels, node, duration| recorder.record(path, bus, numChannels, node, duration) }
	isRecording { ^recorder.isRecording }
	pauseRecording { recorder.pauseRecording }
	stopRecording { recorder.stopRecording }
	prepareForRecord { |path, numChannels| recorder.prepareForRecord(path, numChannels) }

	/* internal server commands */

	bootInProcess {
		^options.bootInProcess;
	}
	quitInProcess {
		_QuitInProcessServer
		^this.primitiveFailed
	}
	allocSharedControls { |numControls=1024|
		_AllocSharedControls
		^this.primitiveFailed
	}
	setSharedControl { |num, value|
		_SetSharedControl
		^this.primitiveFailed
	}
	getSharedControl { |num|
		_GetSharedControl
		^this.primitiveFailed
	}

	prPingApp { |func, onFailure, timeout = 3|
		var id = func.hash;
		var resp = OSCFunc({ |msg| if(msg[1] == id, { func.value; task.stop }) }, "/synced", addr);
		var task = timeout !? { fork { timeout.wait; resp.free;  onFailure.value } };
		addr.sendMsg("/sync", id);
	}

	/* CmdPeriod support for Server-scope and Server-record and Server-volume */

	cmdPeriod {
		addr = addr.recover;
		this.changed(\cmdPeriod)
	}

	queryAllNodes { | queryControls = false |
		var resp, done = false;
		if(isLocal) {
			this.sendMsg("/g_dumpTree", 0, queryControls.binaryValue)
		} {
			resp = OSCFunc({ |msg|
				var i = 2, tabs = 0, printControls = false, dumpFunc;
				if(msg[1] != 0) { printControls = true };
				("NODE TREE Group" + msg[2]).postln;
				if(msg[3] > 0) {
					dumpFunc = {|numChildren|
						var j;
						tabs = tabs + 1;
						numChildren.do {
							if(msg[i + 1] >= 0) { i = i + 2 } {
								i = i + 3 + if(printControls) { msg[i + 3] * 2 + 1 } { 0 };
							};
							tabs.do { "   ".post };
							msg[i].post; // nodeID
							if(msg[i + 1] >= 0) {
								" group".postln;
								if(msg[i + 1] > 0) { dumpFunc.value(msg[i + 1]) };
							} {
								(" " ++ msg[i + 2]).postln; // defname
								if(printControls) {
									if(msg[i + 3] > 0) {
										" ".post;
										tabs.do { "   ".post };
									};
									j = 0;
									msg[i + 3].do {
										" ".post;
										if(msg[i + 4 + j].isMemberOf(Symbol)) {
											(msg[i + 4 + j] ++ ": ").post;
										};
										msg[i + 5 + j].post;
										j = j + 2;
									};
									"\n".post;
								}
							}
						};
						tabs = tabs - 1;
					};
					dumpFunc.value(msg[3]);
				};
				done = true;
			}, '/g_queryTree.reply', addr).oneShot;

			this.sendMsg("/g_queryTree", 0, queryControls.binaryValue);
			SystemClock.sched(3, {
				if(done.not) {
					resp.free;
					"Remote server failed to respond to queryAllNodes!".warn;
				};
			})
		}
	}

	printOn { |stream|
		stream << name;
	}

	storeOn { |stream|
		var codeStr = switch(this,
			Server.default, { if(sync_s) { "s" } { "Server.default" } },
			Server.local,	{ "Server.local" },
			Server.internal, { "Server.internal" },
			{ "Server.fromName(" + name.asCompileString + ")" }
		);
		stream << codeStr;
	}

	archiveAsCompileString { ^true }
	archiveAsObject { ^true }

	getControlBusValue {|busIndex|
		if(serverInterface.isNil) {
			Error("Server-getControlBusValue only supports local servers").throw;
		} {
			^serverInterface.getControlBusValue(busIndex)
		}
	}

	getControlBusValues {|busIndex, busChannels|
		if(serverInterface.isNil) {
			Error("Server-getControlBusValues only supports local servers").throw;
		} {
			^serverInterface.getControlBusValues(busIndex, busChannels)
		}
	}

	setControlBusValue {|busIndex, value|
		if(serverInterface.isNil) {
			Error("Server-getControlBusValue only supports local servers").throw;
		} {
			^serverInterface.setControlBusValue(busIndex, value)
		}
	}

	setControlBusValues {|busIndex, valueArray|
		if(serverInterface.isNil) {
			Error("Server-getControlBusValues only supports local servers").throw;
		} {
			^serverInterface.setControlBusValues(busIndex, valueArray)
		}
	}

	*scsynth {
		this.program = this.program.replace("supernova", "scsynth")
	}

	*supernova {
		this.program = this.program.replace("scsynth", "supernova")
	}

}
