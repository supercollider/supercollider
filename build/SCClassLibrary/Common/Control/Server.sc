
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

// max logins
// session-password

	asOptionsString { arg port;
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
		^o
	}
	
	firstPrivateBus { // after the outs and ins
		^numOutputBusChannels + numInputBusChannels
	}
}

Server : Model {
	classvar <>local, <>internal, <>named, <>set;
	
	var <name, <addr;
	var <isLocal, <inProcess;
	var <serverRunning = false;
	var <>options,<>latency = 0.2,<dumpMode=0, <notified=true;
	var <nodeAllocator; 
	var <staticNodeAllocator;
	var <controlBusAllocator;
	var <audioBusAllocator;
	var <bufferAllocator;
	var <nodeWatcher;

	var <numUGens=0,<numSynths=0,<numGroups=0,<numSynthDefs=0;
	var <avgCPU, <peakCPU;
	
	var alive = false,aliveThread,statusWatcher;
	
	var <window;
	
	*new { arg name, addr, options;
		^super.new.init(name, addr, options)
	}
	init { arg argName, argAddr, argOptions;
		name = argName;
		addr = argAddr;
		options = argOptions ? ServerOptions.new;
		if (addr.isNil, { addr = NetAddr("127.0.0.1", 57110) });
		inProcess = addr.addr == 0;
		isLocal = inProcess || { addr.addr == 2130706433 };
		serverRunning = false;
		named.put(name, this);
		set.add(this);
		this.newAllocators;	
		this.newNodeWatcher;
	}
	newNodeWatcher {
		nodeWatcher = BasicNodeWatcher.new(addr);
	}
	newAllocators {
		nodeAllocator = RingNumberAllocator(1000, 1000 + options.maxNodes);
		staticNodeAllocator = RingNumberAllocator(1000 + options.maxNodes, 
										1001 + (2*options.maxNodes));
		controlBusAllocator = PowerOfTwoAllocator(options.numControlBusChannels);
		audioBusAllocator = PowerOfTwoAllocator(options.numAudioBusChannels, 
		options.numInputBusChannels + options.numOutputBusChannels);
		bufferAllocator = PowerOfTwoAllocator(options.numBuffers);
	}
	
	*initClass {
		named = IdentityDictionary.new;
		set = Set.new;
		internal = Server.new(\internal, NetAddr.new);
		local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
	}
	sendMsg { arg ... args;
		addr.sendBundle(nil, args);
	}
	sendBundle { arg time ... messages;
		addr.performList(\sendBundle, time, messages);
	}
	sendRaw { arg rawArray;
		addr.sendRaw(rawArray);
	}
	
	listSendMsg { arg msg;
		addr.sendBundle(nil,msg);
	}
 	listSendBundle { arg time,bundle;
		addr.performList(\sendBundle, [time] ++ bundle);
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
	
	nextNodeID {
		^nodeAllocator.alloc
	}
	nextStaticNodeID {
		^staticNodeAllocator.alloc;
	}
	nodeIsPlaying { arg nodeID;
		^nodeWatcher.nodeIsPlaying(nodeID)
	}
		
	
	//loadDir
	
	serverRunning_ { arg val;
		if (val != serverRunning, {
			serverRunning = val;
			this.changed(\serverRunning);
		});
	}
	
	wait { arg responseName;
		var resp, routine;
		routine = thisThread;
		resp = OSCresponder(addr, responseName, { 
			resp.remove; routine.resume(true); 
		});
		resp.add;
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
	
	boot {
		var resp;
		if (serverRunning, { "server already running".inform; ^this });
		if (isLocal.not, { "can't boot a remote server".inform; ^this });
		nodeWatcher.start;
	
		if (inProcess, { 
			"booting internal".inform;
			this.bootInProcess; 
			//alive = true;
			//this.serverRunning = true;
		},{
			//isBooting = true;
			if(notified, { 
				//assumes four seconds startup time. should be replaced by something else.
				WaitUntilServerBoot(this, { this.notify; "sent notify on".inform; });
			});
			unixCmd("./scsynth" ++ options.asOptionsString(addr.port));
			("booting " ++ addr.port.asString).inform;
		});
		
		
		
		
	}
	
	reboot {
		var resp;
		if (isLocal.not, { "can't reboot a remote server".inform; ^this });
		if(serverRunning, {
			resp = OSCresponder(addr, '/done', {
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
		this.serverRunning = false;
		nodeWatcher.stop;
		this.newAllocators;
		RootNode(this).freeAll;
	}
	*quitAll {
		set.do({ arg server; server.quit; })
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
		this.sendMsg("/g_freeAll",0);
		nodeWatcher.clear; //to be sure
	}
	*freeAll {
		set.do({ arg server;
			if(server.isLocal,{ // debatable ?
				server.freeAll;
			})
		})
	}
	*resumeThreads {
		set.do({ arg server;
			server.stopAliveThread;
			server.startAliveThread(0.7);
		});
	}
	
	// internal server commands
	bootInProcess {
		_BootInProcessServer
		^this.primitiveFailed
	}
	quitInProcess {
		_QuitInProcessServer
		^this.primitiveFailed
	}
	allocSharedControls { arg numControls=1024;
		_AllocSharedControls
		^this.primitiveFailed
	}
	allocSharedBufs { arg numScopeBufs=16, samples=1024, channels=1;
		_AllocSharedSndBufs
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
	
	////
	makeWindow { arg w;
		var active,booter,running,booting,stopped;
		var countsViews, ctlr;
		
		if (window.notNil, { ^window.front });
		
		if(w.isNil,{
			w = window = SCWindow(name.asString ++ " server", Rect(10, named.values.indexOf(this) * 140 + 10, 256, 100));
			w.view.decorator = FlowLayout(w.view.bounds);
		});
		
		if(isLocal,{
			booter = SCButton(w, Rect(0,0, 50, 24));
			booter.states = [["Boot", Color.black, Color.clear],
						   ["Quit", Color.black, Color.clear]];
			
			booter.action = { arg view; 
				if(view.value == 1, {
					booting.value;
					this.boot;
				});
				if(view.value == 0,{
					this.quit;
				});
			};
			booter.setProperty(\value,serverRunning.binaryValue);
		});
		
		active = SCStaticText(w, Rect(0,0, 80, 24));
		active.string = this.name.asString;
		active.align = \center;
		active.font = Font("Helvetica-Bold", 16);
		active.background = Color.black;
		if(serverRunning,running,stopped);		
		
		if (isLocal, {
			running = {
				active.stringColor_(Color.red);
				booter.setProperty(\value,1);
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				booter.setProperty(\value,0);
			};
			booting = {
				active.stringColor_(Color.yellow(0.9));
				//booter.setProperty(\value,0);
			};
			
			w.onClose = {
				//OSCresponder.removeAddr(addr);
				//this.stopAliveThread;
				//this.quit;
				window = nil;
				ctlr.remove;
			};
		},{	
			running = {
				active.background = Color.red;
			};
			stopped = {
				active.background = Color.black;
			};
			booting = {
				active.background = Color.yellow;
			};
			w.onClose = {
				// but do not remove other responders
				this.stopAliveThread;
				ctlr.remove;
			};
		});
		if(serverRunning,running,stopped);
			
		w.view.decorator.nextLine;

		countsViews = 
		#[
			"Avg CPU :", "Peak CPU :", 
			"UGens :", "Synths :", "Groups :", "SynthDefs :"
		].collect({ arg name, i;
			var label,numView, pctView;
			label = SCStaticText(w, Rect(0,0, 72, 14));
			label.string = name;
			label.align = \right;
		
			if (i < 2, { 
				numView = SCStaticText(w, Rect(0,0, 30, 14));
				numView.string = "?";
				numView.align = \left;
			
				pctView = SCStaticText(w, Rect(0,0, 12, 14));
				pctView.string = "%";
				pctView.align = \left;
			},{
				numView = SCStaticText(w, Rect(0,0, 42, 14));
				numView.string = "?";
				numView.align = \left;
			});
			
			numView
		});
		
		w.front;

		ctlr = SimpleController(this)
			.put(\serverRunning, {	if(serverRunning,running,stopped) })
			.put(\counts,{
				countsViews.at(0).string = avgCPU.round(0.1);
				countsViews.at(1).string = peakCPU.round(0.1);
				countsViews.at(2).string = numUGens;
				countsViews.at(3).string = numSynths;
				countsViews.at(4).string = numGroups;
				countsViews.at(5).string = numSynthDefs;
			});	
		this.startAliveThread;
	}
}
