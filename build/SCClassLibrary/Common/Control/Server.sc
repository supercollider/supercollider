
ServerOptions
{
	classvar	<>default;

	var <>numAudioBusChannels=128;
	var <>numControlBusChannels=4096;
	var <>numInputBusChannels=8;
	var <>numOutputBusChannels=8;
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

	var <>device = nil;
	
	var <>blockAllocClass;

	*initClass {
		default = this.new.blockAllocClass_(PowerOfTwoAllocator);
	}
	
	*new {
		default.notNil.if({
			^default.copy
		}, {
			^super.new
		});
	}

	// max logins
	// session-password

	// prevent buffer conflicts in Server-prepareForRecord and Server-scope by
	// ensuring reserved buffers
	numBuffers { ^(numBuffers -2) }
	numBuffers_ { | argNumBuffers | numBuffers = argNumBuffers + 2; }
	
	asOptionsString { arg port=57110;
		var o;
		o = if (protocol == \tcp, " -t ", " -u ");
		o = o ++ port;
		
		if (numAudioBusChannels != 128, { 
			o = o ++ " -a " ++ numAudioBusChannels;
		});
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
		if (device.notNil, {
			o = o ++ " -H \"" ++ device ++ "\"" ;
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
}

Server : Model {
	classvar <>local, <>internal, <>default, <>named, <>set, <>program;
	
	var <name, <addr, <clientID=0;
	var <isLocal, <inProcess;
	var <serverRunning = false, <serverBooting=false, bootNotifyFirst=false;
	var <>options,<>latency = 0.2,<dumpMode=0, <notified=true;
	var <nodeAllocator;
	var <controlBusAllocator;
	var <audioBusAllocator;
	var <bufferAllocator;
	var <syncThread, <syncTasks;

	var <numUGens=0,<numSynths=0,<numGroups=0,<numSynthDefs=0;
	var <avgCPU, <peakCPU;
	var <sampleRate, <actualSampleRate;
	
	var alive = false, booting = false, aliveThread, <>aliveThreadPeriod = 0.7, statusWatcher;
	var <>tree;
	
	var <window, <>scopeWindow;
	var recordBuf, <recordNode, <>recHeaderFormat="aiff", <>recSampleFormat="float"; 	var <>recChannels=2;
	
	var <bufferArray, <bufInfoResponder, <waitingForBufInfo = false, <waitingBufs = 0;
	
	*new { arg name, addr, options, clientID=0;
		^super.new.init(name, addr, options, clientID)
	}
	init { arg argName, argAddr, argOptions, argClientID;
		name = argName;
		addr = argAddr;
		clientID = argClientID;
		options = argOptions ? ServerOptions.new;
		if (addr.isNil, { addr = NetAddr("127.0.0.1", 57110) });
		inProcess = addr.addr == 0;
		isLocal = inProcess || { addr.addr == 2130706433 };
		serverRunning = false;
		named.put(name, this);
		set.add(this);
		this.newAllocators;	
		bufferArray = Array.newClear(options.numBuffers + 2);
		Server.changed(\serverAdded, this);
	}
	initTree {
		nodeAllocator = NodeIDAllocator(clientID);	
		this.sendMsg("/g_new", 1);
		tree.value(this);
	}
	newAllocators {
		nodeAllocator = NodeIDAllocator(clientID);
		controlBusAllocator = options.blockAllocClass.new(options.numControlBusChannels);
		audioBusAllocator = options.blockAllocClass.new(options.numAudioBusChannels, 
			options.firstPrivateBus);
		bufferAllocator = options.blockAllocClass.new(options.numBuffers);
	}
	nextNodeID {
		^nodeAllocator.alloc
	}
	
	*initClass {
		Class.initClassTree(ServerOptions);
		named = IdentityDictionary.new;
		set = Set.new;
		internal = Server.new(\internal, NetAddr.new);
		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		program = "cd " ++ String.scDir.quote ++ "; ./scsynth";
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
		resp = OSCresponderNode(addr, "/done", {|time, resp, msg|
			if (msg[1].asString == cmdName) {
				resp.remove;
				condition.test = true;
				condition.signal;
			};
		}).add;
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
		addr.sendBundle(time, *msgs);
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
				serverRunning = val;
				if (serverRunning.not) { recordNode = nil };
				{ this.changed(\serverRunning); }.defer;
			}
		};
	}
	
	wait { arg responseName;
		var resp, routine;
		routine = thisThread;
		resp = OSCresponderNode(addr, responseName, { 
			resp.remove; routine.resume(true); 
		});
		resp.add;
	}
	
	waitForBoot { arg onComplete, limit=100;
		if(serverBooting,{ ^this.doWhenBooted(onComplete,limit) });
		if(serverRunning.not, { 
			this.boot;
			this.doWhenBooted(onComplete, limit);
		},onComplete);
	}
	
	doWhenBooted { arg onComplete, limit=100;
		var mBootNotifyFirst = bootNotifyFirst;
		bootNotifyFirst = false;
	
		^Routine({
			while({
				(serverRunning.not or: (serverBooting and: mBootNotifyFirst.not)) and: {(limit = limit - 1) > 0}
			},{
				0.2.wait;	
			});

			if(serverRunning.not,{
				"server failed to start".error;
				serverBooting = false;
			}, onComplete);
		}).play(AppClock);
	}

	bootSync { arg condition;
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
		statusWatcher = 
			OSCresponderNode(addr, 'status.reply', { arg time, resp, msg;
				var cmd, one;
				alive = true;
				#cmd, one, numUGens, numSynths, numGroups, numSynthDefs, 
					avgCPU, peakCPU, sampleRate, actualSampleRate = msg;
				{
					this.serverRunning_(true);
					this.changed(\counts);
					nil // no resched
				}.defer;
			}).add;	
	}
	// Buffer objects are cached in an Array for easy
	// auto buffer info updating
	addBuf { |buffer|
		bufferArray.put(buffer.bufnum, buffer);
	}
	
	freeBuf { |i|
		bufferArray.put(i, nil);
	}
	
	// /b_info on the way
	// keeps a reference count of waiting Buffers so that only one responder is needed
	waitForBufInfo {
		waitingForBufInfo.not.if({
			bufInfoResponder = OSCresponderNode(addr, "/b_info", { arg time, responder, message;
				var buffer;
				(buffer = bufferArray.at(message.at(1))).notNil.if({
					buffer.numFrames = message.at(2);
					buffer.numChannels = message.at(3);
					buffer.sampleRate = message.at(4);
					buffer.queryDone;
					((waitingBufs = waitingBufs - 1) == 0).if({
						responder.remove; waitingForBufInfo = false;
					});
				});
			}).add;
			waitingForBufInfo = true;
		});
		waitingBufs = waitingBufs + 1;	
	}
	
	resetBufferAutoInfo {
		// Buffer info support
		// allow room for protected scope and record buffers
		bufInfoResponder.notNil.if({bufInfoResponder.remove;});
		bufferArray = Array.newClear(options.numBuffers + 2);
		waitingBufs = 0;
		waitingForBufInfo = false;
	}
	
	startAliveThread { arg delay=4.0;
		^aliveThread ?? {
			this.addStatusWatcher;
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
			statusWatcher.remove;
			statusWatcher = nil;
		});
	}
	*resumeThreads {
		set.do({ arg server;
			server.stopAliveThread;
			server.startAliveThread(server.aliveThreadPeriod);
		});
	}
	
	boot { arg startAliveThread=true;
		var resp;
		if (serverRunning, { "server already running".inform; ^this });
		if (serverBooting, { "server already booting".inform; ^this });
		
		serverBooting = true;
		if(startAliveThread, { this.startAliveThread });
		this.newAllocators;
		this.resetBufferAutoInfo;
		bootNotifyFirst = true;
		this.doWhenBooted({ 
			if(notified, { 
				this.notify;
				"notification is on".inform;
			}, { 
				"notification is off".inform; 
			});
			serverBooting = false;
			this.initTree;
		});
		if (isLocal.not, { 
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
		},{
			unixCmd(program ++ options.asOptionsString(addr.port));
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
	notify { arg flag=true;
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
		addr.sendMsg("/quit");
		if (inProcess, { 
			this.quitInProcess;
			"quit done\n".inform;
		},{
			"/quit sent\n".inform;
		});
		alive = false;
		dumpMode = 0;
		serverBooting = false;
		this.serverRunning = false;
		if(scopeWindow.notNil) { scopeWindow.quit };
		RootNode(this).freeAll;
		this.newAllocators;
		this.resetBufferAutoInfo;
	}

	*quitAll {
		set.do({ arg server; if(server.isLocal or: {server.inProcess} ) {server.quit}; })
	}
	*killAll {
		// if you see Exception in World_OpenUDP: unable to bind udp socket
		// its because you have multiple servers running, left
		// over from crashes, unexpected quits etc.
		// you can't cause them to quit via OSC (the boot button)

		// this brutally kills them all off		
		"killall -9 scsynth".unixCmd;
		this.quitAll;
	}
	freeAll {
		this.sendMsg("/g_freeAll", 0);
		this.sendMsg("/clearSched");
		this.initTree;
	}
	*freeAll {
		set.do({ arg server;
			if(server.isLocal,{ // debatable ?
				server.freeAll;
			})
		})
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
		bundle = addr.closeBundle(time);
		addr = addr.saveAddr;
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
	
	record {
		recordBuf.isNil.if({"Please execute Server-prepareForRecord before recording".warn; }, {
			recordNode.isNil.if({
				recordNode = Synth.tail(RootNode(this), "server-record", [\bufnum, 
					recordBuf.bufnum]);
			}, { recordNode.run(true) });
			"Recording".postln;
		});
	}

	pauseRecording {
		recordNode.notNil.if({ recordNode.run(false); "Paused".postln }, { "Not Recording".warn });
	}
	
	stopRecording {
		recordNode.notNil.if({ 
			recordNode.free;
			recordNode = nil;
			recordBuf.close({ arg buf; buf.free; });
			recordBuf = nil; 
			"Recording Stopped".postln }, 
		{ "Not Recording".warn });
	}
	
	prepareForRecord { arg path;
		if (path.isNil) { path = "recordings/SC_" ++ Date.localtime.stamp ++ "." ++ 
			recHeaderFormat; };
		recordBuf = Buffer.alloc(this, 65536, recChannels,
			{arg buf; buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true);},
			this.options.numBuffers + 1); // prevent buffer conflicts by using reserved bufnum
		SynthDef("server-record", { arg bufnum;
			DiskOut.ar(bufnum, In.ar(0, recChannels)) 
		}).send(this);
		// cmdPeriod support
		CmdPeriod.add(this);
	}
	
	// CmdPeriod support for Server-scope and Server-record
	cmdPeriod {
		if(recordNode.notNil) { recordNode = nil; };
		if(recordBuf.notNil) { recordBuf.close {|buf| buf.free; }; recordBuf = nil; };
		addr = addr.recover;
		this.changed(\cmdPeriod);
		if(scopeWindow.notNil) {
			fork { 0.5.wait; scopeWindow.run } // wait until synth is freed
		}{
			CmdPeriod.remove(this)
		};
	}
	
	defaultGroup { ^Group.basicNew(this, 1) }
	
	queryAllNodes { arg queryControls = false;
		var resp, done = false;
		if(isLocal, {this.sendMsg("/g_dumpTree", 0, queryControls.binaryValue);}, {
			resp = OSCresponderNode(addr, '/g_queryTree.reply', { arg time, responder, msg;
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
										if(msg[i + 4 + j].isSymbol, {
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
			}).add.removeWhenDone;
			this.sendMsg("/g_queryTree", 0, queryControls.binaryValue);
			SystemClock.sched(3, { 
				done.not.if({
					resp.remove;
					"Remote server failed to respond to queryAllNodes!".warn;
				}); 
			});
		})
	}
	printOn { |stream|
		stream << name;
	}
	storeOn { arg stream;
		stream << "(Server.named[" << name.asCompileString << "] ?? { Server(" <<<* [name, addr, options, clientID] << ") })" ;
	}
	archiveAsCompileString { ^true }
	archiveAsObject { ^true }
}
