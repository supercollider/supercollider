
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
	
	
	play { arg outBus=0, nChan, clock;
		var playGroup, cmd;
		if(bus.isNil, {this.allocBus(\audio, nChan ? 2) });
		this.wakeUpParents(clock);
		
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
	
	send { arg extraArgs;
			this.sendToServer(false, true, extraArgs);
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
		^(group.notNil and: {group.isPlaying})
	}
	
	asDefName { ^if(synthDef.notNil, { synthDef.name }, { nil }) }
	
	
	// setting the source to anything that returns a valid ugen input
	
	
	source_ { arg argObj; this.setObj(argObj, true) }
	
	setObj { arg argObj, send=false, freeLast=true, clock; 		
		synthDef = nil;
		
		if(argObj.notNil,{
			if(bus.isNil, {this.initFor(argObj) });
			this.initParents;
			synthDef = argObj.asProxySynthDef(this);
			if(synthDef.isNil, 
				{ "creating synthDef failed".inform },
				{
					synthDef.writeDefFile;
					if(send, 
						{ this.sendToServer(freeLast, false, nil, clock) },
						{ this.updateSynthDef } //only load def. maybe we are on server already? 
					);
			})
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
	
	//map to a control proxy
	map { arg key, proxy; //...args doesn't work. why?
		var args;
		if(proxy.rate === 'control', { 
			args = [key, proxy];
			nodeMap.performList(\map, args);
			if(this.isPlaying, { nodeMap.send(group) })
		}, {
			"can only map to control bus".inform
		})
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
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	/////////////////////////////////////////////
	
	
			
	// server communications, updating
	
	sendToServer { arg freeLast=true, loaded=false, extraArgs, clock;
		var cmd, resp;
		if( synthDef.notNil and: { server.serverRunning }, {
				cmd = List.new;
				this.sendSynthCommand(cmd, freeLast, extraArgs);
				if(loaded.not, { 
					this.updateSynthDef;
					resp = OSCresponder(server.addr, '/done', {
						clock.sched({ server.sendCmdList(cmd) });
						resp.remove;
					}).add
				}, { 
						clock.sched({ server.sendCmdList(cmd) })
				});
		});
	}
	
	
	updateSynthDef {
		if(synthDef.notNil, { 
			if(server.isLocal, {//maybe direct send would be better? server reboot would destroy them?
				server.loadSynthDef(synthDef.name)
			}, {
				server.sendSynthDef(synthDef.name)
			}); 
		});
	}
	
	
	sendSynthCommand { arg cmd, freeLast=true, extraArgs;
		var synth;
				if(this.isPlaying.not, {
					group = Group.newCommand(cmd, server, \addToHead);
					group.prIsPlaying(true);
				
				}, {
					//release current synth
					if(freeLast, {group.addCommand(cmd, "/n_set", [\synthGate, 0.0]) });
				});
		
		synth = Synth.newCommand(cmd, synthDef.name, extraArgs, group); 
		nodeMap.updateCommand(cmd, synth);
		
	
	}
	
		
	////// private /////
	
	initFor { arg obj;
		var rate, array, numChannels;
			if(obj.notNil, {
				//for speed
				if(obj.isKindOf(NodeProxy), {
					rate = obj.rate;
					numChannels = obj.numChannels
				},{
					array = obj.value.asArray;
					rate = array.rate ? 'control'; 
					numChannels = array.size;
				});
				bus = Bus.perform(rate, server, numChannels);
				nodeMap = ProxyNodeMap.new;
				this.initParents;
			});
		
	}

	getOutput { arg numChannels;
		var out, n;
		this.addToBuildSynthDef;
		this.wakeUpParents;
		n = bus.numChannels;
		out = if(this.rate === 'audio', 
				{ InFeedback.ar( bus.index, n) },
				{ In.kr( bus.index, n) }
			);
		//test that
		//if(numChannels.notNil and: { numChannels != n }, {
//			out = NumChannels.ar(out, numChannels, true)
//		}); 
		^out
		
	}
	
	addToBuildSynthDef {
		var parentProxy;
		parentProxy = UGen.buildSynthDef.tryPerform(\proxy);
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
	}
	
	wakeUp { arg clock;
		
		if(this.isPlaying.not, { this.sendToServer(true, true, nil, clock) });
	
	}
	
	wakeUpParents { arg clock;
		
		parents.do({ arg item; item.wakeUpParents(clock) });
		this.wakeUp(clock);
	}
	
		
	
	
}