
ServerOptions
{
	var <>numAudioBusChannels=128;
	var <>numControlBusChannels=4096;
	var <>numInputBusChannels=2;
	var <>numOutputBusChannels=2;
	var <>numBuffers=1024;
	
	var <>maxNodes=1024;
	var <>maxSynthDefs=1024;
	var <>protocol = \udp;
	var <>blockSize = 64;
	var <>hardwareBufferSize = 0;
	
	var <>memSize = 2048;
	var <>numRGens = 64;
	var <>numWireBufs = 64;

	var <>sampleRate = 0;
	var <>loadDefs = true;
	
	var <>inputStreamsEnabled;
	var <>outputStreamsEnabled;

// max logins
// session-password

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
		if (numInputBusChannels != 2, { 
			o = o ++ " -i " ++ numInputBusChannels;
		});
		if (numOutputBusChannels != 2, { 
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
		if (hardwareBufferSize != 0, {
			o = o ++ " -Z " ++ hardwareBufferSize;
		});
		if (memSize != 2048, {
			o = o ++ " -m " ++ memSize;
		});
		if (numRGens != 64, {
			o = o ++ " -r " ++ numRGens;
		});
		if (numWireBufs != 64, {
			o = o ++ " -w " ++ numWireBufs;
		});
		if (sampleRate != 0, {
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
	var <serverRunning = false, <serverBooting=false;
	var <>options,<>latency = 0.2,<dumpMode=0, <notified=true;
	var <nodeAllocator;
	var <controlBusAllocator;
	var <audioBusAllocator;
	var <bufferAllocator;
	var <syncThread, <syncTasks;

	var <numUGens=0,<numSynths=0,<numGroups=0,<numSynthDefs=0;
	var <avgCPU, <peakCPU;
	
	var alive = false, booting = false, aliveThread, statusWatcher;
	var <>tree;
	
	var <window, <>scopeWindow;
	var recordBuf, <recordNode, <>recHeaderFormat="aiff", <>recSampleFormat="float"; 	var <>recChannels=2;
	
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
	}
	initTree {
		this.sendMsg("/g_new", 1);
		tree.value(this);
	}
	newAllocators {
		nodeAllocator = NodeIDAllocator(clientID);
		controlBusAllocator = PowerOfTwoAllocator(options.numControlBusChannels);
		audioBusAllocator = PowerOfTwoAllocator(options.numAudioBusChannels, 
		options.numInputBusChannels + options.numOutputBusChannels);
		bufferAllocator = PowerOfTwoAllocator(options.numBuffers);
	}
	nextNodeID {
		^nodeAllocator.alloc
	}
	
	*initClass {
		named = IdentityDictionary.new;
		set = Set.new;
		internal = Server.new(\internal, NetAddr.new);
		default = local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		program = "./scsynth";
	}
	sendMsg { arg ... args;
		addr.sendMsg(*args);
	}
	sendBundle { arg time ... messages;
		addr.sendBundle(time, *messages);
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
	
	sync { arg condition;
		var resp, id;
		if (condition.isNil) { condition = Condition.new };
		id = UniqueID.next;
		resp = OSCresponderNode(addr, "/synced", {|time, resp, msg|
			if (msg[1] == id) {
				resp.remove;
				condition.test = true;
				condition.signal;
			};
		}).add;
		condition.test = false;
		addr.sendBundle(nil, ["/sync", id]);
		condition.wait;
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
	
	listSendMsg { arg msg;
		addr.sendBundle(nil,msg);
	}
 	listSendBundle { arg time, bundle;
		addr.sendBundle(time, *bundle);
	}
	
	// load from disk locally, send remote
	sendSynthDef { arg name,dir="synthdefs/";
		var file, buffer;
		file = File(dir ++ name ++ ".scsyndef","r");
		if (file.isNil, { ^nil });
		buffer = Int8Array.newClear(file.length);
		file.read(buffer);
		file.close;
		this.sendMsg("/d_recv", buffer);
	}
	// tell server to load from disk
	loadSynthDef { arg name, completionMsg,dir="synthdefs/";
		this.listSendMsg(
			["/d_load", dir ++ name ++ ".scsyndef", completionMsg ]
		) 
	}
	//loadDir
	loadDirectory { arg dir, completionMsg;
		this.listSendMsg(["/d_loadDir", dir, completionMsg]);
	}
	
	serverRunning_ { arg val;
		if (val != serverRunning, {
			serverRunning = val;
			if (serverRunning.not) { recordNode = nil; };
			{ this.changed(\serverRunning); }.defer;
		});
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
		^Routine({
			while({
				serverRunning.not and: {(limit = limit - 1) > 0}
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
	
	addStatusWatcher {
		statusWatcher = 
			OSCresponder(addr, 'status.reply', { arg time, resp, msg;
				alive = true;
				numUGens = msg.at(2);
				numSynths = msg.at(3);
				numGroups = msg.at(4);
				numSynthDefs = msg.at(5);
				avgCPU = msg.at(6);
				peakCPU = msg.at(7);
				{
					this.serverRunning_(true);
					this.changed(\counts);
					nil // no resched
				}.defer;
			}).add;	
	}
	startAliveThread { arg delay=4.0, period=0.7;
		^aliveThread ?? {
			this.addStatusWatcher;
			aliveThread = Routine({
				// this thread polls the server to see if it is alive
				delay.wait;
				loop({
					this.status;
					period.wait;
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
			server.startAliveThread(0.7);
		});
	}
	
	boot { arg startAliveThread=true;
		var resp;
		if (serverRunning, { "server already running".inform; ^this });
		if (serverBooting, { "server already booting".inform; ^this });
		if (isLocal.not, { "can't boot a remote server".inform; ^this });
		
		serverBooting = true;
		if(startAliveThread, { this.startAliveThread });
		this.newAllocators;	
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
		this.bootServerApp;
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
	reboot {
		var resp;
		if (isLocal.not, { "can't reboot a remote server".inform; ^this });
		if(serverRunning, {
			resp = OSCresponderNode(addr, '/done', {
				this.boot;
				resp.remove;
			}).add;
			this.quit;
		}, {
			this.boot
		})
	}
	
	status {
		addr.sendMsg("/status");
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
		RootNode(this).freeAll;
		this.newAllocators;
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
		//	ps -axo pid,command | grep -i "[s]csynth" | awk '{system("kill " $1)}'
		unixCmd("ps axo pid,command | grep -i \"[s]csynth\" | awk '{system(\"kill \" $1)}'");
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
				recordNode = Synth.new("server-record", [\bufnum, recordBuf.bufnum], this, 
					\addAfter);  
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
		if (path.isNil) { path = "recordings/SC_" ++ Date.localtime.stamp ++ "." ++ recHeaderFormat; };
		recordBuf = Buffer.alloc(this, 65536, recChannels,
			{arg buf; buf.writeMsg(path, recHeaderFormat, recSampleFormat, 0, 0, true);});
		SynthDef("server-record", { arg bufnum;
			DiskOut.ar(bufnum, In.ar(0, recChannels)) 
		}).send(this);
		// cmdPeriod support
		CmdPeriod.add(this);
	}
	
	// CmdPeriod support for Server-scope and Server-record
	cmdPeriod {
		recordNode.notNil.if({ recordNode = nil; });
		recordBuf.notNil.if({recordBuf.close({ arg buf; buf.free; }); recordBuf = nil;});
		this.changed(\cmdPeriod);
		CmdPeriod.remove(this);
	}
	
	defaultGroup { ^Group.basicNew(this, 1) }
	
	// from Crucial
	queryAllNodes {
	
			var probe,probing,resp,nodes,server,report,indent = 0,order=0;
			var nodeWatcher;
			
			nodeWatcher = NodeWatcher.newFrom(this);
			
			nodes = IdentityDictionary.new;
			probing = List.new;
			
			probe = { arg nodeID;
				probing.add(nodeID);
				this.sendMsg("/n_query",nodeID);
			};
			
			report = { arg nodeID=0;
				var child,synth;
				indent.do({ " ".post });
				nodes.at(nodeID).use({
					~order = order;
					if(~isGroup,{ 
						("Group(" ++ nodeID ++ ")").postln;
						child = ~head;
						indent = indent + 8;
						while({
							child > 0
						},{
							order = order + 1;
							report.value(child);
							child = nodes.at(child).at(\next);
						});
						indent = indent - 8;
					},{
						synth = nodeWatcher.nodes.at(nodeID);
						if(synth.notNil,{ // get defName if available
							synth.asString.postln;
						},{
							("Synth" + nodeID).postln;
						});
					});
				});
			};
				
			resp = OSCresponder(this.addr,'/n_info',{ arg a,b,c;
						var cmd,nodeID,parent,prev,next,isGroup,head,tail;
						# cmd,nodeID,parent,prev,next,isGroup,head,tail = c;
						
						//[cmd,nodeID,parent,prev,next,isGroup,head,tail].debug;
						
						nodes.put(nodeID,
							Environment.make({
								~nodeID = nodeID;
								~parent = parent;
								~prev = prev;
								~next = next;
								~isGroup = isGroup == 1;
								~head = head;
								~tail = tail;
							})
						);
						
						if(next > 0,{
							probe.value(next);
						});
						if(isGroup==1,{
							if(head > 0,{
								probe.value(head);
							});
						});
						probing.remove(nodeID);
						if(probing.size == 0,{
							resp.remove;
							report.value;
						});
					}).add;
					
			probe.value(0);

		
	}
	
}
