
NodeProxy : AbstractFunction {

	var <server, <group, <bus;
	var <nodeMap, <synthDef, <parents;
	
	*new { arg server;
		^super.newCopyArgs(server).ninit(server)	
	}
	*audio { arg server, numChannels=2;
		^this.new(server).allocBus(\audio, numChannels);
	}
	*control { arg server, numChannels=1;
		^this.new(server).allocBus(\control, numChannels);
	}
	ninit { arg  argServer, argNumChannels;
		server = argServer ? Server.local;
		this.clear;
	}
	
	*initClass {
		//supposes it is on server already. revisit
		SynthDef("proxyOut-linkDefAr", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 1)) 
		}).writeDefFile;
	}
	
	clear {
		nodeMap = ProxyNodeMap.new;
		this.initParents;
		bus = nil;
		//not yet implemented: if(bus.notNil, { bus.free });
	}

	initParents {
		parents = IdentitySet.new;
	}
	
	allocBus { arg rate=\audio, numChannels=2;
		bus = Bus.perform(rate, server, numChannels);
	}
	
	fadeTime_ { arg t;
		this.set(\synthFadeTime, t);
	}
	
	// playing and access
	
	ar { arg numChannels;
		if(bus.isNil, { this.allocBus(\audio, 2) });
		^this.getOutput(numChannels)
	}
	
	kr { arg numChannels;
		if(bus.isNil, { this.allocBus(\control, 1) });
		^this.getOutput(numChannels)
	}
	
	value { ^this.ar }
	
	
	play { arg outBus=0, nChan;
		var playGroup, cmd;
		if(bus.isNil, {this.allocBus(\audio, nChan ? 2) });
		this.wakeUpParents;
		
		
		cmd = List.new;
		playGroup = Group.newCommand(cmd, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		nChan.do({ arg i;
			Synth.newCommand(cmd, "proxyOut-linkDefAr", [\i_busOut, outBus+i, \i_busIn, bus.index+i], playGroup, \addToTail);
		});
		server.sendCmdList(cmd);
		^playGroup
	}
	
	send {
			this.sendToServer(false);
	}
	
	load {
		if(server.serverRunning, {
			parents.do({ arg item; item.load });
			this.updateSynthDef
		}, { "server not running".inform });
	}
	
	rate { ^bus.tryPerform(\rate) }
	
	
	numChannels {
		^bus.tryPerform(\numChannels);
	}
	
	isPlaying { 
		^(group.notNil and: {group.isPlaying}) // and: isPlaying)
	}
	
	asDefName { ^if(synthDef.notNil, { synthDef.name }, { nil }) }
	

	
	// setting the source 
	
	
	source_ { arg argObj; this.setObj(argObj, true) }
	
	setObj { arg argObj, send=false, freeLast=true; //anything that returns a valid ugen input
		
		synthDef = nil;
		
		if(argObj.notNil,{
			if(bus.isNil, {this.initFor(argObj) });
			
			synthDef = argObj.asProxySynthDef(this);
			synthDef.writeDefFile;
			
			this.updateSynthDef;
			if(send, { this.sendToServer(freeLast) });
		})
	}
	
	
	
	

	
	
	////////////behave like my group////////////
	
	free {
		if(this.isPlaying, { group.free });
	}
	
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying, { group.performList(\set, args) });
	}
	
	setn { arg ... args;
		nodeMap.performList(\setn, args);
		if(this.isPlaying, { group.performList(\setn, args) });
	}
	
	map { arg ... args;
		
		nodeMap.performList(\map, args);
		if(this.isPlaying, { nodeMap.send(group) });
	}
	
	unset { arg ... args;
		nodeMap.performList(\unset, args);
	}
	
	unmap { arg ... args;
		nodeMap.performList(\unmap, args);
		if(this.isPlaying, { nodeMap.send(group) });
	}
	release {
		if(this.isPlaying, { group.set(\synthGate, 0.0) });
	}
	
	/////////////////////////////////////////////
	
	
			
	// server communications, updating
	
	sendToServer { arg freeLast=true;
		var cmd;
		if( synthDef.notNil and: { server.serverRunning }, {
				cmd = List.new;
				this.sendSynthCommand(cmd, freeLast);
				Routine({ 0.4.wait; 
					server.sendCmdList(cmd);
				 }).play;
		});
	}
	
	
	updateSynthDef {
		
		if(synthDef.notNil, { 
			server.sendSynthDef(synthDef.name);
			/*
			if(server.isLocal, {
				server.loadSynthDef(def.name)
			}, {
				server.sendSynthDef(def.name)
			}); 
			*/
			
		}, { "writing synthDef failed".inform });
	}
	
	
	
		
	
	sendSynthCommand { arg cmd, freeLast=true;
		var synth;
				if(this.isPlaying.not, {
					group = Group.newCommand(cmd, server, \addToHead);
					group.prIsPlaying(true);
				
				});
		//release current synth
		if(freeLast, {group.addCommand(cmd, "/n_set", \synthGate, 0.0) });
		synth = Synth.newCommand(cmd, synthDef.name, nil, group); 
		nodeMap.updateCommand(cmd, synth);
		
	
	}
	
		
	////// private /////
	
	initFor { arg obj;
		var rate, array;
			if(obj.notNil, {
				
				array = obj.value.asArray;
				array = obj.rate; 
				bus = Bus.perform(rate, server, array.size);
				nodeMap = ProxyNodeMap.new;
				this.initParents;
			});
		
	}

	getOutput { arg numChannels;
		var out;
		this.addToBuildSynthDef;
		this.wakeUpParents;
		out = if(this.rate === 'audio', 
				{ InFeedback.ar( bus.index, bus.numChannels) },
				{ In.kr( bus.index, bus.numChannels) }
			);
		if(numChannels.notNil, {
			out = NumChannels.ar(out, numChannels, true)
		}); 
		^out
		
	}
	
	addToBuildSynthDef {
		var parentProxy;
		parentProxy = UGen.buildSynthDef.tryPerform(\proxy);
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
	}
	
	wakeUp {
		
		if(this.isPlaying.not, { this.sendToServer });
	
	}
	
	wakeUpParents {
		
		parents.do({ arg item; item.wakeUpParents });
		this.wakeUp;
	}
	
		
	
	
}