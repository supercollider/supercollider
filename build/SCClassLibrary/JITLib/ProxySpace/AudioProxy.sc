
AudioProxy : AbstractFunction {

	var <group, <server, <bus;
	var <nodeMap, <obj; 
	var <lastDef, <parents;


	*new { arg server, numChannels;
		^super.new.ninit(server, numChannels ? this.defaultNumChannels)	}
	
	ninit { arg  argServer, argNumChannels;
		server = argServer ? Server.local;
		nodeMap = ProxyNodeMap.new;
		this.alloc(argNumChannels);
	}
	
	*initClass {
		SynthDef("proxyOut", { arg busOut=0, busIn=16; 
			Out.ar(busOut, InFeedback.ar(busIn, 1)) 
		}).writeDefFile;
	}
	
	initParents {
		parents = IdentitySet.new;
	}
	
	ar { 
		^this.getOutput
	}
	
	kr { 
		^this.getOutput
	}
	
	value { ^this.ar }
	
	
	play { arg outBus=0, nChan;
		var playGroup, cmd;
		
		this.wakeUpParents;
		
		cmd = List.new;
		playGroup = Group.newCommand(cmd, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		nChan.do({ arg i;
			Synth.newCommand(cmd, "proxyOut", [\busOut, outBus+i, \busIn, bus.index+i], playGroup, \addToTail);
		});
		server.sendCmdList(cmd);
		^playGroup
	}
	
	obj_ { arg argObj; this.setObj(argObj, true) }
	
	setObj { arg argObj, send=false; //anything that returns a valid ugen input
		
		lastDef = nil;
		
		if(argObj.notNil,{
			this.initParents;
			obj = argObj.asPFunction(this);
			this.updateSynthDef;
			if(send, { this.sendToServer });
		}, { 
			this.free 
		})
	}
	
	
	fadeTime_ { arg t;
		this.set(\synthFadeTime, t);
	}
	
	getOutput {
		var out;
		this.addToBuildSynthDef;
		this.wakeUpParents;
		^InFeedback.ar( bus.index, bus.numChannels);
		
	}

	
	alloc { arg numChannels;
		this.free;
		bus = Bus.perform(this.rate, server, numChannels);
	}
	
	////////////behave like my group////////////
	
	free {
		if(this.isPlaying, { group.free });
		//not yet implemented: if(bus.notNil, { bus.free });
	}
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		group.performList(\set, args);
	}
	
	map { arg ... args;
		
		nodeMap.performList(\map, args);
		nodeMap.send(group);
	}
	
	unset { arg ... args;
		nodeMap.performList(\unset, args);
	}
	
	unmap { arg ... args;
		nodeMap.performList(\unmap, args);
		nodeMap.send(group);
	}
	release {
		group.set(\synthGate, 0.0);
	}
	/////////////////////////////////////////////
	
	*defaultNumChannels { ^2 }
	*rate { ^\audio }
	rate { ^this.class.rate }
	
	
	numChannels {
		^bus.numChannels
	}
	numChannels_ { arg n;
		this.free;
		this.alloc(n);
	}
	isPlaying { 
		^(group.notNil and: {group.isPlaying}) // and: isPlaying)
	}
	asDefName { ^if(lastDef.notNil, { lastDef.name }, { nil }) }
	
		
	
	sendToServer {
		var cmd;
		if(obj.notNil and: { lastDef.notNil } and: { server.serverRunning }, {
				
				
				cmd = List.new;
				this.sendSynthCommand(cmd);
				Routine({ 0.4.wait; 
					server.sendCmdList(cmd);
				 }).play;
		});
	}
	
	
	updateSynthDef {
		var def;
		def = ProxySynthDef(this, obj).writeDefFile;
		if(def.notNil, { 
			if(server.isLocal, {
				server.loadSynthDef(def.name)
				//server.sendSynthDef(def.name)
			}, {
				server.sendSynthDef(def.name)
			}); 
			lastDef = def 
		}, { "writing synthDef failed".inform });
	}
	
	send {
			var cmd, synth;
				cmd = List.new;
				synth = Synth.newCommand(cmd, lastDef.name, nil, group); 
				nodeMap.updateCommand(cmd, synth);
				server.sendCmdList(cmd);
	}
	
	load {
		if(server.serverRunning, {
			parents.do({ arg item; item.load });
			this.updateSynthDef
		}, { "server not running".inform });
	}
	
	
	sendSynthCommand { arg cmd;
		var synth;
				if(this.isPlaying.not, {
					group = Group.newCommand(cmd, server, \addToHead);
					group.prIsPlaying(true);
				
				});
		group.addCommand(cmd, "/n_set", \synthGate, 0.0); //release the current synth
		synth = Synth.newCommand(cmd, lastDef.name, nil, group); 
		nodeMap.updateCommand(cmd, synth);
		
	
	}
	
	////// private /////
	
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

ControlProxy : AudioProxy {
	
	*rate { ^\control }
	
		
	*defaultNumChannels { ^1 }
	
	getOutput {
		var out;
		this.wakeUpParents;
		this.addToBuildSynthDef;
		^In.kr(bus.index, bus.numChannels);
	}
		
	value { ^this.kr }
	
}

