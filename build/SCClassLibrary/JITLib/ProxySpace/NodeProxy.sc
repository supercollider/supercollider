
NodeProxy : AbstractFunction {

	var <server, <group, <outbus;
	var <synthDefs, <loaded=false, <parents;
	
	var <nodeMap, <>clock, <env, <>autoRelease=true;
	
	
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
		SynthDef("proxyOut-linkDefAr", { arg i_busOut=0, i_busIn=16; 
			Out.ar(i_busOut, InFeedback.ar(i_busIn, 1)) 
		}).writeDefFile;
	}
	
	clear {
		nodeMap = ProxyNodeMap.new;
		synthDefs = Array.new;
		this.free;
		group = nil;
		this.initParents;
		if(outbus.notNil, { outbus.free });
		outbus = nil;
	}

	initParents {
		parents = IdentitySet.new;
		if(nodeMap.notNil, { //keep the mapped parents
			nodeMap.mappings.do({ arg proxy; parents.add(proxy) });
		});
	}
	
	allocBus { arg rate=\audio, numChannels=2;
		outbus = Bus.perform(rate, server, numChannels);
	}
	
	fadeTime_ { arg t;
		this.set(\synthFadeTime, t);
	}
	
	// playing and access
	
	ar { arg numChannels;
		if(outbus.isNil, { this.allocBus(\audio, numChannels ? 2) });
		^this.getOutput(numChannels)
	}
	
	kr { arg numChannels;
		if(outbus.isNil, { this.allocBus(\control, numChannels ? 1) });
		^this.getOutput(numChannels)
	}
	
	value { ^if(this.rate == 'audio', {this.ar}, {this.kr})  }
	
	
	env_ { arg argEnv;
		this.release;
		env = argEnv;
	}
	
	nodeMap_ { arg map;
		nodeMap = map;
		map.send(group);
	}
	
		
	play { arg outBus=0, nChan;
		var playGroup, msg;
		if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) }); //assumes audio
		
		msg = List.new;
		playGroup = Group.newMsg(msg, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		this.wakeUpParentsToBundle(msg);
		
		
		nChan.do({ arg i;
			Synth.newMsg(msg, "proxyOut-linkDefAr", 
				[\i_busOut, outBus+i, \i_busIn, outbus.index+i], playGroup, \addToTail);
		});
		//server.listSendBundle(nil, msg);
		this.schedSendOSC(msg)
		^playGroup
	}
	
	send { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(false, 0.0, extraArgs);
	}
	sendAll { arg extraArgs;
			//latency is 0, def is on server
			this.sendToServer(true, 0.0, extraArgs);
	}
	
	record { arg path, headerFormat="aiff", sampleFormat="int16", numChannels;
		var rec;
		rec = RecNodeProxy.newFrom(this, numChannels, {
					//rec.record(path, headerFormat, sampleFormat);
		});
		
		Routine({
			1.0.wait;//seemingly must wait.
			rec.record(path, headerFormat, sampleFormat);
		}).play;
		
		^rec
	}
	
	refresh {
		var msg;
		msg = List.new;
		this.sendSynthMsg(msg, true);
		this.schedSendOSC(msg)
	}
	
	rate { ^outbus.tryPerform(\rate) }
	
	numChannels {
		^outbus.tryPerform(\numChannels);
	}
	
	isPlaying { 
		^(group.notNil and: {group.isPlaying})
	}
	
	printOn { arg stream;
		stream << "NodeProxy." << this.rate << "(" << server << ", " << this.numChannels <<")";
	}
	
	
	// setting the source to anything that returns a valid ugen input
	
	add { arg obj, send=true;
		this.setObj(obj, send, false)
	}
	
	source_ { arg obj; 
		this.setObj(obj, true) 
	}
	
	setObj { arg obj, send=false, freeAll=true, onCompletion, latency=0.3; 		var def, ok, writeOK;
			
			ok = this.initFor(obj);
			
			if(ok, {
				if(freeAll, { 
					synthDefs = Array.new; 
					this.initParents 
				});
				
				def = obj.asProxySynthDef(this);
				synthDefs = synthDefs.add(def);
				def.writeDefFile;
				loaded = false;
				
				if(server.serverRunning, { 
					this.sendDef(def);//for now.
					if(send, { this.sendToServer(freeAll, latency, nil, onCompletion) });
				 });
				
			}, { "rate/numChannels must match".inform })
			
	}
	
	
	
	
	////////////behave like my group////////////
	
	free {
		if(this.isPlaying, { group.free });
	}
	
	
	set { arg ... args;
		nodeMap.performList(\set, args);
		if(this.isPlaying, { group.performList(\set, args) }, {"not playing".postln });
	}
	
	setn { arg ... args;
		nodeMap.performList(\setn, args);
		if(this.isPlaying, { group.performList(\setn, args) });
	}
	
	//map to a control proxy
	map { arg key, proxy; // ... args; //...args maybe doesn't work. test.
		var args;
		if(proxy.rate === 'control', { 
			args = [key, proxy]; //++args;
			nodeMap.performList(\map, args);
			parents.add(proxy);
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
		var msg;
		msg = List.new;
		if(this.isPlaying, { 
			this.freeAllMsg(msg);
			server.listSendBundle(nil, msg)
		});
	}
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	/////////////////////////////////////////////
	
	
	// server communications, updating
	
	sendToServer { arg freeAll=true, latency=0.3, extraArgs, onCompletion;
		var msg, resp;
		if( synthDefs.isEmpty.not and: { server.serverRunning }, {
				msg = List.new;
				this.prepareForPlayMsg(msg, freeAll);
				this.sendSynthMsg(msg, freeAll, extraArgs);
				if(latency.notNil, {
					SystemClock.sched(latency, {
								this.schedSendOSC(msg, onCompletion); 
					})
				}, {
					resp = OSCresponder(server.addr, '/done', { 
						this.schedSendOSC(msg, onCompletion);
						resp.remove;
					}).add;
				
				});
		});
	}
			
	schedSendOSC { arg msg, onCompletion;
					//msg.asCompileString.postln;
					if(clock.notNil, {
						clock.sched(0, { 
							server.listSendBundle(nil, msg); 
							onCompletion.value(this) 
						})
					}, {
						server.listSendBundle(nil, msg); 
						onCompletion.value(this)
					})
	}
	
	prepareForPlayMsg { arg msg, freeAll=true;
					if(this.isPlaying.not, {
						group = Group.newMsg(msg, server, \addToHead);
						group.prIsPlaying(true);
					}, {
						if(freeAll, {
							this.freeAllMsg(msg);
						});
					});
	}
	
	sendSynthMsg { arg msg, freeAll=true, extraArgs;
				var synth;
				
				if(freeAll, {
					synthDefs.do({ arg synthDef;
						Synth.newMsg(msg, synthDef.name, extraArgs, group); 
					});
					nodeMap.updateMsg(msg, group);
					
				}, {
					synth = Synth.newMsg(msg, synthDefs.last.name, extraArgs, group); 
					nodeMap.updateMsg(msg, synth);
				});	
	}
	
	

		
	freeAllMsg { arg msg;
					if(autoRelease, {
							group.addMsg(msg, 15, [\synthGate, 0.0]); //n_set
							group.freeAll(false);
					});
					//group.addMsg(msg, 24); //g_freeAll
					//	}, {
	}
	
	
		
	////// private /////
	
	initFor { arg obj;
		var rate, numChannels;
			if(obj.notNil, {
				#rate, numChannels = obj.busData;
				if(rate !== 'audio', { rate = 'control' });//see later for scalars
				numChannels = numChannels ? 1;
				
				if(outbus.isNil, {
					outbus = Bus.perform(rate, server, numChannels);
					nodeMap = ProxyNodeMap.new;
					^true
				}, {
					^(outbus.rate === rate) && (numChannels <= outbus.numChannels)
				});
			});
			^false
		
	}

	getOutput { arg numChannels;
		var out, n;
		
		this.addToBuildSynthDef;
		this.wakeUpParents;
		n = outbus.numChannels;
		out = if(this.rate === 'audio', 
				{ InFeedback.ar( outbus.index, n) },
				{ In.kr( outbus.index, n) }
			);
	
		//test that
		/*
		if(numChannels != n, {
			out = NumChannels.ar(out, numChannels, false)
		}); 
		*/
		^out
		
		
	}
	
	addToBuildSynthDef {
		var parentProxy;
		parentProxy = UGen.buildSynthDef.tryPerform(\proxy);
		if(parentProxy.notNil && (parentProxy !== this), { parentProxy.parents.add(this) });
	}
	
	wakeUpToBundle { arg bundle; //no need to wait, def is on server
		if(this.isPlaying.not, { 
			this.prepareForPlayMsg(bundle, true);
			this.sendSynthMsg(bundle, true, 0);
		});
		
	}
	
	wakeUpParentsToBundle { arg bundle; //see for synthDefs
		
		
		if(this.isPlaying.not, {
			if(loaded.not, { this.loadAll }); //change later maybe to defsToBundle in sendSynthMsg
			parents.do({ arg item; item.wakeUpParentsToBundle(bundle) });
			this.wakeUpToBundle(bundle);
		});
	}
	
	wakeUpParents { 
		var bundle;
		bundle = List.new;
		this.wakeUpParentsToBundle(bundle);
		this.schedSendOSC(bundle);
	}
	
	load {
		var msg;
		msg = List.new;
		this.loadMsg(msg);
		if(server.serverRunning, { 
			server.listSendBundle(nil, msg);
		});
	
	}
		
	loadAll {
		var msg;
		msg = List.new;
		if(server.serverRunning, {
			parents.do({ arg proxy; 
				proxy.sendAllDefsMsg(msg); 
			});
			this.sendAllDefsMsg(msg);
			server.listSendBundle(nil, msg);
		}, { "server not running".inform });
	}
	sendDef { arg def;
		//var msg;
		//msg = List.new;
		//this.sendDefMsg(msg, def);
		server.sendMsg("/d_recv", def.asBytes);
	}
	
	sendDefMsg { arg msg, def;
			msg.add(["/d_recv", def.asBytes]);
	}
	
	sendAllDefsMsg { arg msg;
		synthDefs.do({ arg def;
			msg.add(["/d_recv", def.asBytes]);
		});
		//loaded = true; //remember to send;
	}	
	
	
}