ServerOptions
{
	// order of variables is important here. Only add new instance variables to the end.
	var <numAudioBusChannels=128;
	var <>numControlBusChannels=4096;
	var <numInputBusChannels=8;
	var <numOutputBusChannels=8;
	var numBuffers=1026;

	var <>maxNodes=1024;
	var <>maxSynthDefs=1024;
	var <>protocol = \udp;
	var <>blockSize = 64;
	var <>hardwareBufferSize = nil;

	var <>memSize = 8192;
	var <>numRGens = 64;
	var <>numWireBufs = 64;

	var <>sampleRate = nil;
	var <>loadDefs = true;

	var <>inputStreamsEnabled;
	var <>outputStreamsEnabled;

	var <>inDevice = nil;
	var <>outDevice = nil;

	var <>verbosity = 0;
	var <>zeroConf = false; // Whether server publishes port to Bonjour, etc.

	var <>restrictedPath = nil;

	var <>initialNodeID = 1000;
	var <>remoteControlVolume = false;

	var <>memoryLocking = false;
	var <>threads = nil; // for supernova

	var <numPrivateAudioBusChannels=112;

	device {
		^if(inDevice == outDevice)
		{
			inDevice
		}
		{
			[inDevice, outDevice]
		}
	}

	device_ {
		|dev|
		inDevice = outDevice = dev;
	}

	// max logins
	// session-password

	// prevent buffer conflicts in Server-prepareForRecord and Server-scope by
	// ensuring reserved buffers

	numBuffers {
		^numBuffers - 2
	}

	numBuffers_ { | argNumBuffers |
		numBuffers = argNumBuffers + 2
	}

	asOptionsString { | port = 57110 |
		var o;
		o = if (protocol == \tcp, " -t ", " -u ");
		o = o ++ port;

	    o = o ++ " -a " ++ (numPrivateAudioBusChannels + numInputBusChannels + numOutputBusChannels) ;

		if (numControlBusChannels != 4096, {
			o = o ++ " -c " ++ numControlBusChannels;
		});
		if (numInputBusChannels != 8, {
			o = o ++ " -i " ++ numInputBusChannels;
		});
		if (numOutputBusChannels != 8, {
			o = o ++ " -o " ++ numOutputBusChannels;
		});
		if (numBuffers != 1024, {
			o = o ++ " -b " ++ numBuffers;
		});
		if (maxNodes != 1024, {
			o = o ++ " -n " ++ maxNodes;
		});
		if (maxSynthDefs != 1024, {
			o = o ++ " -d " ++ maxSynthDefs;
		});
		if (blockSize != 64, {
			o = o ++ " -z " ++ blockSize;
		});
		if (hardwareBufferSize.notNil, {
			o = o ++ " -Z " ++ hardwareBufferSize;
		});
		if (memSize != 8192, {
			o = o ++ " -m " ++ memSize;
		});
		if (numRGens != 64, {
			o = o ++ " -r " ++ numRGens;
		});
		if (numWireBufs != 64, {
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
		if ((thisProcess.platform.name!=\osx) or: {inDevice == outDevice})
		{
			if (inDevice.notNil,
			{
				o = o ++ " -H %".format(inDevice.quote);
			});
		}
		{
			o = o ++ " -H % %".format(inDevice.asString.quote, outDevice.asString.quote);
		};
		if (verbosity != 0, {
			o = o ++ " -v " ++ verbosity;
		});
		if (zeroConf.not, {
			o = o ++ " -R 0";
		});
		if (restrictedPath.notNil, {
			o = o ++ " -P " ++ restrictedPath;
		});
		if (memoryLocking, {
			o = o ++ " -L";
		});
		if (threads.notNil, {
			if (Server.program.asString.endsWith("supernova")) {
				o = o ++ " -T " ++ threads;
			}
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

	numPrivateAudioBusChannels_ {arg numChannels = 112;
		numPrivateAudioBusChannels = numChannels;
		this.recalcChannels;
	}

	numAudioBusChannels_ {arg numChannels=128;
		numAudioBusChannels = numChannels;
		numPrivateAudioBusChannels = numAudioBusChannels - numInputBusChannels - numOutputBusChannels;
	}

	numInputBusChannels_ {arg numChannels=8;
		numInputBusChannels = numChannels;
		this.recalcChannels;
	}

	numOutputBusChannels_ {arg numChannels=8;
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
	classvar <>local, <>internal, <default, <>named, <>set, <>program, <>sync_s = true;

	var <name, <>addr, <clientID=0;
	var <isLocal, <inProcess, <>sendQuit, <>remoteControlled;
	var <serverRunning = false, <serverBooting = false, bootNotifyFirst = false;
	var <>options, <>latency = 0.2, <dumpMode = 0, <notify = true, <notified=false;
	var <nodeAllocator;
	var <controlBusAllocator;
	var <audioBusAllocator;
	var <bufferAllocator;
	var <scopeBufferAllocator;
	var <syncThread, <syncTasks;

	var <numUGens=0, <numSynths=0, <numGroups=0, <numSynthDefs=0;
	var <avgCPU, <peakCPU;
	var <sampleRate, <actualSampleRate;

	var alive = false, booting = false, aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;
	var <>tree;

	var <window, <>scopeWindow;
	var <emacsbuf;
	var recordBuf, <recordNode, <>recHeaderFormat="aiff", <>recSampleFormat="float";
	var <>recChannels=2;

	var <volume;

	var <pid;
	var serverInterface;

	*default_ { |server|
		default = server; // sync with s?
		if (sync_s, { thisProcess.interpreter.s = server });
		this.all.do(_.changed(\default, server));
	}

	*new { arg name, addr, options, clientID=0;
		^super.new.init(name, addr, options, clientID)
	}

	*all { ^set }
	*all_ { arg dict; set = dict }

	init { arg argName, argAddr, argOptions, argClientID;
		name = argName.asSymbol;
		addr = argAddr;
		clientID = argClientID;
		options = argOptions ? ServerOptions.new;
		if (addr.isNil, { addr = NetAddr("127.0.0.1", 57110) });
		inProcess = addr.addr == 0;
		isLocal = inProcess || { addr.addr == 2130706433 };
		remoteControlled = isLocal;
		serverRunning = false;
		named.put(name, this);
		set.add(this);
		this.newAllocators;
		Server.changed(\serverAdded, this);
		volume = Volume.new(server: this, persist: true);
	}

	initTree {
		nodeAllocator = NodeIDAllocator(clientID, options.initialNodeID);
		this.sendMsg("/g_new", 1, 0, 0);
		tree.value(this);
		ServerTree.run(this);
	}
	newAllocators {
		this.newNodeAllocators;
		this.newBusAllocators;
		this.newBufferAllocators;
		this.newScopeBufferAllocators;
		NotificationCenter.notify(this, \newAllocators);
	}

	newNodeAllocators {
		nodeAllocator = NodeIDAllocator(clientID, options.initialNodeID);
	}

	newBusAllocators {
		controlBusAllocator = ContiguousBlockAllocator.new(options.numControlBusChannels);
		audioBusAllocator = ContiguousBlockAllocator.new(options.numAudioBusChannels,
			options.firstPrivateBus);
	}

	newBufferAllocators {
		bufferAllocator = ContiguousBlockAllocator.new(options.numBuffers);
	}

	newScopeBufferAllocators {
		if (isLocal) {
			scopeBufferAllocator = StackNumberAllocator.new(0, 127);
		}
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

	*initClass {
		Class.initClassTree(ServerOptions);
		Class.initClassTree(NotificationCenter);
		named = IdentityDictionary.new;
		set = Set.new;
		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		internal = Server.new(\internal, NetAddr.new);
	}

	*fromName { arg name;
		^Server.named[name] ?? {
			Server(name, NetAddr.new("127.0.0.1", 57110), ServerOptions.new, 0)
		}
	}

	// bundling support added
	sendMsg { arg ... msg;
		addr.sendMsg(*msg);
	}
	sendBundle { arg time ... msgs;
		addr.sendBundle(time, *msgs)
	}

	sendRaw { arg rawArray;
		addr.sendRaw(rawArray);
	}

	sendMsgSync { arg condition ... args;
		var cmdName, resp;
		if (condition.isNil) { condition = Condition.new };
		cmdName = args[0].asString;
		if (cmdName[0] != $/) { cmdName = cmdName.insert(0, $/) };
		resp = OSCFunc({|msg|
			if (msg[1].asString == cmdName) {
				resp.free;
				condition.test = true;
				condition.signal;
			};
		}, '/done', addr);
		condition.test = false;
		addr.sendBundle(nil, args);
		condition.wait;
	}

	sync { arg condition, bundles, latency; // array of bundles that cause async action
		addr.sync(condition, bundles, latency)
	}

	schedSync { arg func;
		syncTasks = syncTasks.add(func);
		if(syncThread.isNil) {
			syncThread = Routine.run {
				var c; c = Condition.new;
				while { syncTasks.notEmpty } { syncTasks.removeAt(0).value(c) };
				syncThread = nil;
		 	};
		};

	}

	// bundling support added
	listSendMsg { arg msg;
		addr.sendMsg(*msg);
	}
 	listSendBundle { arg time, msgs;
		addr.sendBundle(time, *(msgs.asArray));
	}

	// load from disk locally, send remote
	sendSynthDef { arg name, dir;
		var file, buffer;
		dir = dir ? SynthDef.synthDefDir;
		file = File(dir ++ name ++ ".scsyndef","r");
		if (file.isNil, { ^nil });
		protect {
			buffer = Int8Array.newClear(file.length);
			file.read(buffer);
		}{
			file.close;
		};
		this.sendMsg("/d_recv", buffer);
	}
	// tell server to load from disk
	loadSynthDef { arg name, completionMsg, dir;
		dir = dir ? SynthDef.synthDefDir;
		this.listSendMsg(
			["/d_load", dir ++ name ++ ".scsyndef", completionMsg ]
		)
	}
	//loadDir
	loadDirectory { arg dir, completionMsg;
		this.listSendMsg(["/d_loadDir", dir, completionMsg]);
	}

	serverRunning_ { arg val;
		if(addr.hasBundle) {
			{ this.changed(\bundling); }.defer;
		} {
			if (val != serverRunning) {
				if(thisProcess.platform.isSleeping.not) {
					serverRunning = val;
					if (serverRunning.not) {

						ServerQuit.run(this);

						if (serverInterface.notNil) {
							serverInterface.disconnect;
							serverInterface = nil;
						};

						AppClock.sched(5.0, {
							// still down after 5 seconds, assume server is really dead
							// if you explicitly shut down the server then newAllocators
							// and the \newAllocators notification will happen immediately
							if(serverRunning.not) {
								NotificationCenter.notify(this,\didQuit);
							};
							recordNode = nil;
							if(this.isLocal.not){
								notified = false;
							}
						})

					}{
						ServerBoot.run(this);
					};
					{ this.changed(\serverRunning); }.defer;
				}
			}
		};
	}

	wait { arg responseName;
		var routine;
		routine = thisThread;
		OSCFunc({
			routine.resume(true);
		}, responseName, addr).oneShot;
	}

	waitForBoot { arg onComplete, limit=100, onFailure;
		// onFailure.true: why is this necessary?
		// this.boot also calls doWhenBooted.
		// doWhenBooted prints the normal boot failure message.
		// if the server fails to boot, the failure error gets posted TWICE.
		// So, we suppress one of them.
		if(serverRunning.not) { this.boot(onFailure: true) };
		this.doWhenBooted(onComplete, limit, onFailure);
	}

	doWhenBooted { arg onComplete, limit=100, onFailure;
		var mBootNotifyFirst = bootNotifyFirst, postError = true;
		bootNotifyFirst = false;

		^Routine({
			while({
				((serverRunning.not
				  or: (serverBooting and: mBootNotifyFirst.not))
				 and: {(limit = limit - 1) > 0})
				and: { pid.tryPerform(\pidRunning) == true }
			},{
				0.2.wait;
			});

			if(serverRunning.not,{
				if(onFailure.notNil) {
					postError = (onFailure.value == false);
				};
				if(postError) {
					"server failed to start".error;
					"For advice: [http://supercollider.sf.net/wiki/index.php/ERROR:_server_failed_to_start]".postln;
				};
				serverBooting = false;
				this.changed(\serverRunning);
			}, onComplete);
		}).play(AppClock);
	}

	bootSync { arg condition;
		condition ?? { condition = Condition.new };
		condition.test = false;
		this.waitForBoot({
			// Setting func to true indicates that our condition has become true and we can go when signaled.
			condition.test = true;
			condition.signal
		});
		condition.wait;
	}

	ping { arg n=1, wait=0.1, func;
		var result=0, pingFunc;
		if(serverRunning.not) { "server not running".postln; ^this };
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
						SystemClock.sched(wait, {pingFunc.value; nil })
					} {
						("maximum determined latency of" + name + ":" + result + "s").postln;
						func.value(result)
					}
				};
		};
		pingFunc.value;
	}

	addStatusWatcher {
		if(statusWatcher.isNil) {
			statusWatcher =
				OSCFunc({ arg msg;
					var cmd, one;
					if(notify){
						if(notified.not){
							this.sendNotifyRequest;
							"Receiving notification messages from server %\n".postf(this.name);
						}
					};
					alive = true;
					#cmd, one, numUGens, numSynths, numGroups, numSynthDefs,
						avgCPU, peakCPU, sampleRate, actualSampleRate = msg;
					{
						this.serverRunning_(true);
						this.changed(\counts);
						nil // no resched
					}.defer;
				}, '/status.reply', addr).fix;
		} {
			statusWatcher.enable;
		};
	}

	cachedBuffersDo { |func| Buffer.cachedBuffersDo(this, func) }
	cachedBufferAt { |bufnum| ^Buffer.cachedBufferAt(this, bufnum) }

	inputBus {
		^Bus(\audio, this.options.numOutputBusChannels, this.options.numInputBusChannels, this);
	}

	outputBus {
		^Bus(\audio, 0, this.options.numOutputBusChannels, this);
	}

	startAliveThread { arg delay=0.0;
		this.addStatusWatcher;
		^aliveThread ?? {
			aliveThread = Routine({
				// this thread polls the server to see if it is alive
				delay.wait;
				loop({
					this.status;
					aliveThreadPeriod.wait;
					this.serverRunning = alive;
					alive = false;
				});
			});
			AppClock.play(aliveThread);
			aliveThread
		}
	}
	stopAliveThread {
		if( aliveThread.notNil, {
			aliveThread.stop;
			aliveThread = nil;
		});
		if( statusWatcher.notNil, {
			statusWatcher.free;
			statusWatcher = nil;
		});
	}
	aliveThreadIsRunning {
		^aliveThread.notNil and: {aliveThread.isPlaying}
	}
	*resumeThreads {
		set.do({ arg server;
			server.stopAliveThread;
			server.startAliveThread(server.aliveThreadPeriod);
		});
	}

	boot { arg startAliveThread=true, recover=false, onFailure;
		var resp;
		if (serverRunning, { "server already running".inform; ^this });
		if (serverBooting, { "server already booting".inform; ^this });

		serverBooting = true;
		if(startAliveThread, { this.startAliveThread });
		if(recover) { this.newNodeAllocators } { this.newAllocators };
		bootNotifyFirst = true;
		this.doWhenBooted({
			serverBooting = false;
			if (sendQuit.isNil) {
				sendQuit = this.inProcess or: {this.isLocal};
			};

			if (this.inProcess) {
				serverInterface = ServerShmInterface(thisProcess.pid);
			} {
				if (isLocal) {
					serverInterface = ServerShmInterface(addr.port);
				}
			};

			this.initTree;
		}, onFailure: onFailure ? false);
		if (remoteControlled.not, {
			"You will have to manually boot remote server.".inform;
		},{
			this.bootServerApp;
		});
	}

	bootServerApp {
		if (inProcess, {
			"booting internal".inform;
			this.bootInProcess;
			//alive = true;
			//this.serverRunning = true;
			pid = thisProcess.pid;
		},{
			if (serverInterface.notNil) {
				serverInterface.disconnect;
				serverInterface = nil;
			};

			pid = (program ++ options.asOptionsString(addr.port)).unixCmd;
			//unixCmd(program ++ options.asOptionsString(addr.port)).postln;
			("booting " ++ addr.port.asString).inform;
		});
	}

	reboot { arg func; // func is evaluated when server is off
		if (isLocal.not) { "can't reboot a remote server".inform; ^this };
		if(serverRunning) {
			Routine.run {
				this.quit;
				this.wait(\done);
				0.1.wait;
				func.value;
				this.boot;
			}
		} {
			func.value;
			this.boot
		}
	}

	status {
		addr.sendStatusMsg
	}

	notify_ { |flag = true|
		notify = flag;
		if(flag){
			if(serverRunning){
				this.sendNotifyRequest(true);
				notified = true;
				"Receiving notification messages from server %\n".postf(this.name);
			}
		}{
			this.sendNotifyRequest(false);
			notified = false;
			"Switched off notification messages from server %\n".postf(this.name);
		}
	}

	sendNotifyRequest { arg flag=true;
		notified = flag;
		addr.sendMsg("/notify", flag.binaryValue);
	}

	dumpOSC { arg code=1;
		/*
			0 - turn dumping OFF.
			1 - print the parsed contents of the message.
			2 - print the contents in hexadecimal.
			3 - print both the parsed and hexadecimal representations of the contents.
		*/
		dumpMode = code;
		this.sendMsg(\dumpOSC,code);
	}

	quit {
		var	serverReallyQuitWatcher, serverReallyQuit = false;
		statusWatcher !? {
			statusWatcher.disable;
			if(notified) {
				serverReallyQuitWatcher = OSCFunc({ |msg|
					if(msg[1] == '/quit') {
						statusWatcher.enable;
						serverReallyQuit = true;
						serverReallyQuitWatcher.free;
					};
				}, '/done', addr);
				// don't accumulate quit-watchers if /done doesn't come back
				AppClock.sched(3.0, {
					if(serverReallyQuit.not) {
						"Server % failed to quit after 3.0 seconds.".format(this.name).warn;
						serverReallyQuitWatcher.free;
					};
				});
			};
		};
		addr.sendMsg("/quit");
		if (inProcess, {
			this.quitInProcess;
			"quit done\n".inform;
		},{
			"/quit sent\n".inform;
		});
		alive = false;
		notified = false;
		dumpMode = 0;
		pid = nil;
		serverBooting = false;
		sendQuit = nil;
		this.serverRunning = false;
		if(scopeWindow.notNil) { scopeWindow.quit };
		if(volume.isPlaying) {
			volume.free
		};
		RootNode(this).freeAll;
		this.newAllocators;
	}

	*quitAll {
		set.do({ arg server;
			if (server.sendQuit === true) {
				server.quit
			};
		})
	}
	*killAll {
		// if you see Exception in World_OpenUDP: unable to bind udp socket
		// its because you have multiple servers running, left
		// over from crashes, unexpected quits etc.
		// you can't cause them to quit via OSC (the boot button)

		// this brutally kills them all off
		thisProcess.platform.killAll(this.program.basename);
		this.quitAll;
	}
	freeAll {
		this.sendMsg("/g_freeAll", 0);
		this.sendMsg("/clearSched");
		this.initTree;
	}

	*freeAll { arg evenRemote = false;
		if (evenRemote) {
			set.do { arg server;
				if ( server.serverRunning ) { server.freeAll }
			}
		} {
			set.do { arg server;
				if (server.isLocal and:{ server.serverRunning }) { server.freeAll }
			}
		}
	}

	*hardFreeAll { arg evenRemote = false;
		if (evenRemote) {
			set.do { arg server;
				server.freeAll
			}
		} {
			set.do { arg server;
				if (server.isLocal) { server.freeAll }
			}
		}
	}

	*allRunningServers {
		^this.all.select(_.serverRunning)
	}

	// bundling support


	openBundle { arg bundle;	// pass in a bundle that you want to
							// continue adding to, or nil for a new bundle.
		if(addr.hasBundle) {
			bundle = addr.bundle.addAll(bundle);
			addr.bundle = []; // debatable
		};
		addr = BundleNetAddr.copyFrom(addr, bundle);
	}
	closeBundle { arg time; // set time to false if you don't want to send.
		var bundle;
		if(addr.hasBundle) {
			bundle = addr.closeBundle(time);
			addr = addr.saveAddr;
		} {
			"there is no open bundle.".warn
		};
		^bundle;
	}
	makeBundle { arg time, func, bundle;
		this.openBundle(bundle);
		try {
			func.value(this);
			bundle = this.closeBundle(time);
		}{|error|
			addr = addr.saveAddr; // on error restore the normal NetAddr
			error.throw
		}
		^bundle
	}
	bind { arg func;
		^this.makeBundle(this.latency, func)
	}

	// internal server commands
	bootInProcess {
		^options.bootInProcess;
	}
	quitInProcess {
		_QuitInProcessServer
		^this.primitiveFailed
	}
	allocSharedControls { arg numControls=1024;
		_AllocSharedControls
		^this.primitiveFailed
	}
	setSharedControl { arg num, value;
		_SetSharedControl
		^this.primitiveFailed
	}
	getSharedControl { arg num;
		_GetSharedControl
		^this.primitiveFailed
	}

	// recording output
	record { |path|
		if(recordBuf.isNil){
			this.prepareForRecord(path);
			Routine({
				this.sync;
				this.record;
			}).play;
		}{
			if(recordNode.isNil){
				recordNode = Synth.tail(RootNode(this), "server-record",
						[\bufnum, recordBuf.bufnum]);
				CmdPeriod.doOnce {
					recordNode = nil;
					if (recordBuf.notNil) { recordBuf.close {|buf| buf.freeMsg }; recordBuf = nil; };
				}
			} {
				recordNode.run(true)
			};
			"Recording: %\n".postf(recordBuf.path);
		};
	}

	pauseRecording {
		recordNode.notNil.if({ recordNode.run(false); "Paused".postln }, { "Not Recording".warn });
	}

	stopRecording {
		if(recordNode.notNil) {
			recordNode.free;
			recordNode = nil;
			recordBuf.close({ |buf| buf.freeMsg });
			"Recording Stopped: %\n".postf(recordBuf.path);
			recordBuf = nil;
		} {
			"Not Recording".warn
		};
	}

	prepareForRecord { arg path;
		if (path.isNil) {
			if(File.exists(thisProcess.platform.recordingsDir).not) {
				thisProcess.platform.recordingsDir.mkdir
			};

			// temporary kludge to fix Date's brokenness on windows
			if(thisProcess.platform.name == \windows) {
				path = thisProcess.platform.recordingsDir +/+ "SC_" ++ Main.elapsedTime.round(0.01) ++ "." ++ recHeaderFormat;

			} {
				path = thisProcess.platform.recordingsDir +/+ "SC_" ++ Date.localtime.stamp ++ "." ++ recHeaderFormat;
			};
		};
		recordBuf = Buffer.alloc(this, 65536, recChannels,
			{arg buf; buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true);},
			this.options.numBuffers + 1); // prevent buffer conflicts by using reserved bufnum
		recordBuf.path = path;
		SynthDef("server-record", { arg bufnum;
			DiskOut.ar(bufnum, In.ar(0, recChannels))
		}).send(this);
	}

	// CmdPeriod support for Server-scope and Server-record and Server-volume
	cmdPeriod {
		addr = addr.recover;
		this.changed(\cmdPeriod);
	}

	doOnServerTree {
		if(scopeWindow.notNil) { scopeWindow.run }
	}

	defaultGroup { ^Group.basicNew(this, 1) }

	queryAllNodes { arg queryControls = false;
		var resp, done = false;
		if(isLocal, {this.sendMsg("/g_dumpTree", 0, queryControls.binaryValue);}, {
			resp = OSCFunc({ arg msg;
				var i = 2, tabs = 0, printControls = false, dumpFunc;
				if(msg[1] != 0, {printControls = true});
				("NODE TREE Group" + msg[2]).postln;
				if(msg[3] > 0, {
					dumpFunc = {|numChildren|
						var j;
						tabs = tabs + 1;
						numChildren.do({
							if(msg[i + 1] >=0, {i = i + 2}, {
								i = i + 3 + if(printControls, {msg[i + 3] * 2 + 1}, {0});
							});
							tabs.do({ "   ".post });
							msg[i].post; // nodeID
							if(msg[i + 1] >=0, {
								" group".postln;
								if(msg[i + 1] > 0, { dumpFunc.value(msg[i + 1]) });
							}, {
								(" " ++ msg[i + 2]).postln; // defname
								if(printControls, {
									if(msg[i + 3] > 0, {
										" ".post;
										tabs.do({ "   ".post });
									});
									j = 0;
									msg[i + 3].do({
										" ".post;
										if(msg[i + 4 + j].isMemberOf(Symbol), {
											(msg[i + 4 + j] ++ ": ").post;
										});
										msg[i + 5 + j].post;
										j = j + 2;
									});
									"\n".post;
								});
							});
						});
						tabs = tabs - 1;
					};
					dumpFunc.value(msg[3]);
				});
				done = true;
			}, '/g_queryTree.reply', addr).oneShot;
			this.sendMsg("/g_queryTree", 0, queryControls.binaryValue);
			SystemClock.sched(3, {
				done.not.if({
					resp.free;
					"Remote server failed to respond to queryAllNodes!".warn;
				});
			});
		})
	}
	printOn { |stream|
		stream << name;
	}
	storeOn { arg stream;
		var codeStr = this.switch (
			Server.default, 			{ if (sync_s) { "s" } { "Server.default" } },
			Server.local,				{ "Server.local" },
			Server.internal,			{ "Server.internal" },
			{ "Server.fromName(" + name.asCompileString + ")" }
		);
		stream << codeStr;
	}

	archiveAsCompileString { ^true }
	archiveAsObject { ^true }

	volume_ {arg newVolume;
		volume.volume_(newVolume);
	}

	mute {
		volume.mute;
	}

	unmute {
		volume.unmute;
	}

	hasShmInterface { ^serverInterface.notNil }

	reorder { arg nodeList, target, addAction=\addToHead;
		target = target.asTarget;
		this.sendMsg(62, Node.actionNumberFor(addAction), target.nodeID, *(nodeList.collect(_.nodeID))); //"/n_order"
	}

	getControlBusValue {|busIndex|
		if (serverInterface.isNil) {
			Error("Server-getControlBusValue only supports local servers").throw;
		} {
			^serverInterface.getControlBusValue(busIndex)
		}
	}

	getControlBusValues {|busIndex, busChannels|
		if (serverInterface.isNil) {
			Error("Server-getControlBusValues only supports local servers").throw;
		} {
			^serverInterface.getControlBusValues(busIndex, busChannels)
		}
	}

	setControlBusValue {|busIndex, value|
		if (serverInterface.isNil) {
			Error("Server-getControlBusValue only supports local servers").throw;
		} {
			^serverInterface.setControlBusValue(busIndex, value)
		}
	}

	setControlBusValues {|busIndex, valueArray|
		if (serverInterface.isNil) {
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
