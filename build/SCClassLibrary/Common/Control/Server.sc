
ServerOptions
{
	var <>numAudioBusChannels=128;
	var <>numControlBusChannels=4096;
	var <>numInputBusChannels=2;
	var <>numOutputBusChannels=2;
	var <>numBuffers=1024;
	var <>maxNodes=1024;
	var <>maxSynthDefs=1024;

	asString {
		var o;
		o = "";
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
		^o
	}
}

Server : Model {
	classvar <>local, <>internal, <>named, <>set;
	
	var <name, <addr;
	var <isLocal, <inProcess;
	var <serverRunning = false, <alive = false;
	var <window;

	var <>options = "";
	
	*new { arg name, addr;
		^super.new.init(name, addr)
	}
	init { arg n,a;
		name = n;
		addr = a;
		if (addr.isNil, { addr = NetAddr("127.0.0.1", 57110) });
		inProcess = addr.addr == 0;
		isLocal = inProcess || { addr.addr == 2130706433 };
		serverRunning = false;
		named.put(name, this);
		set.add(this);
	}
	
	*initClass {
		named = IdentityDictionary.new;
		set = Set.new;
		local = Server.new(\localhost, NetAddr("127.0.0.1", 57110));
		internal = Server.new(\internal, NetAddr.new);
		local.makeWindow;
		internal.makeWindow;
	}
	
	sendMsg { arg ... args;
		addr.sendBundle(nil, args);
	}
	sendBundle { arg time ... args;
		addr.performList(\sendBundle, time, args);
	}
	sendSynthDef { arg name;
		var file, buffer;
		file = File("synthdefs/" ++ name ++ ".scsyndef","r");
		if (file.isNil, { ^nil });
		buffer = Int8Array.newClear(file.length);
		file.read(buffer);
		file.close;
		this.sendMsg("/d_recv", buffer);

	}
	loadSynthDef { arg name;
		this.sendMsg("/d_load", "synthdefs/" ++ name ++ ".scsyndef");
	}
	
	serverRunning_ { arg val;
		if (val != serverRunning, {
			serverRunning = val;
			this.changed(\serverRunning);
		});
	}	
	
	wait { arg cmdName;
		var resp, routine;
		routine = thisThread;
		resp = OSCresponder(addr, cmdName, { resp.remove; routine.resume(true); });
		resp.add;
	}
	
	boot {
		if (this.serverRunning, { "already running".postln; ^nil });
		
		if (isLocal.not, { ^nil }); // cant boot a remote server.
		if (inProcess, { 
			this.bootInProcess; 
		},{
			unixCmd("./scsynth -u " ++ addr.port ++ " " ++ options.asString);
		});
		
		alive = true;
		this.serverRunning = true;
		this.notify(true);

	}
	quit {
		addr.sendMsg("/quit");
		if (inProcess, { 
			this.quitInProcess;
			"quit done\n".postln
		});
		alive = false;
		this.serverRunning = false;
	}
	
	status {
		addr.sendMsg("/status");
	}
	
	notify { arg flag;
		addr.sendMsg("/notify", flag.binaryValue);
	}	

	firstPrivateBus { // after the outs and ins
		^this.numOutputBusChannels + this.numInputBusChannels
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
	
	makeWindow {
		var w, b0, b1, b2, b3, aliveThread;
		var s0, s1, s1s, ctlr;
		
		if (window.notNil, { ^window.front });
		
		w = window = SCWindow(name.asString ++ " server", Rect(128+200.rand, 64+80.rand, 240, 140));
		w.view.decorator = FlowLayout(w.view.bounds);
		
		if (isLocal, {
			w.onClose = {
				OSCresponder.removeAddr(addr);
				aliveThread.stop; 
				this.quit;
				window = nil;
				ctlr.remove;
			};
		
			b0 = SCButton(w, Rect(0,0, 80, 24));
			b0.states = [["Start Server", Color.black, Color.green],["Stop Server", Color.white, Color.red]];
			b0.enabled = true;
			b0.action = { arg view; 
				if(view.value == 1, {
					this.boot;
				},{
					this.quit;
				});
			};
		
		},{
			w.onClose = {
				OSCresponder.removeAddr(addr);
				aliveThread.stop; 
				ctlr.remove;
			};
		});
		
		w.view.decorator.nextLine;

		["UGens", "Synths", "Groups", "SynthDefs"].do({ arg label;
			s0 = SCStaticText(w, Rect(0,0, 120, 18));
			s0.string = label ++ " :";
			s0.align = \right;
		
			s1 = SCStaticText(w, Rect(0,0, 80, 18));
			s1.string = "?";
			s1.align = \left;
			s1s = s1s.add(s1);
			
			w.view.decorator.nextLine;
		});
		
		w.front;
		
		aliveThread = Routine({
			// this thread polls the server to see if it is alive
			OSCresponder.add(OSCresponder(addr, 'status.reply', { arg time, resp, msg;
				alive = true;
				AppClock.sched(0, {
					// have to sched with AppClock because cocoa 
					// cannot be called from socket routine
					msg.copyRange(2, 5).do({ arg val, i; s1s.at(i).string = val.asString; });
				});
			}));
			4.wait;
			loop({
				this.status;
				0.7.wait;
				this.serverRunning = alive;
				alive = false;
			});
		});
		AppClock.play(aliveThread);
		
		ctlr = SimpleController(this)
			.put(\serverRunning, {
					b0.setProperty(\value, serverRunning.binaryValue);
			});		
	}
}

Module {
	var server, name, id, values, presets, presetIndex, <isOn = false;
	
	*new { arg server, name, id;
		^super.newCopyArgs(server, name, id).init
	}
	init {
		this.load;
	}
	store {
		if (presetIndex > presets.size, {
			presets = presets.add(values);
		});
		presets.put(presetIndex, values);
	}
	recall {
		if (presetIndex < presets.size, {
			values = presets.at(presetIndex).copy;
			// set gui elems and synth controls		
		});
	}
	save {
		presets.writeArchive("server/presets/" ++ name ++ ".scpreset");
	}
	load {
		var filename;
		filename = "server/presets/" ++ name ++ ".scpreset";
		if (File.exists(filename), {
			presets = Object.readArchive(filename);
		});
	}
	start {
		var msg;
		isOn = true;
		msg = ["/s_new", name, id, 0];
		values.keysValuesDo({ arg key, value; msg = msg.addAll([key, value]); });
		server.addr.sendBundle(nil, msg); 
	}
	stop {
		isOn = false;
		server.sendMsg("/n_set", id, \gate, 0); 
	}
}


