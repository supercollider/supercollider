
NodeProxy : AbstractFunction {

	var <server, <group, <outbus;
	var <nodeMap, <>clock;
	var <synthDefs, <parents, <loaded=false;
	
	
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
		synthDefs = Array(1);
		this.initParents;
		outbus = nil;
		//not yet implemented: if(outbus.notNil, { outbus.free });
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
	
	
	play { arg outBus=0, nChan;
		var playGroup, msg;
		if(outbus.isNil, {this.allocBus(\audio, nChan ? 2) });
		
		msg = List.new;
		
		playGroup = Group.newMsg(msg, server);
		nChan = nChan ? this.numChannels;
		nChan = nChan.min(this.numChannels);
		this.wakeUpParentsToBundle(msg);
		
		nChan.do({ arg i;
			Synth.newMsg(msg, "proxyOut-linkDefAr", [\i_busOut, outBus+i, \i_busIn, outbus.index+i], playGroup, \addToTail);
		});
		server.listSendBundle(nil, msg);
		^playGroup
	}
	
	send { arg extraArgs;
			this.sendToServer(false, 0.0, extraArgs);
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
	
	add { arg obj;
		this.setObj(obj, true, false, true)
	}
	source_ { arg obj; 
		this.setObj(obj, true) 
	}
	
	setObj { arg obj, send=false, freeLast=true, add=false, onCompletion, latency=0.5; 		var def, ok, writeOK;
			
			ok = this.initFor(obj);
			
			if(ok, {
				if(add.not, { 
					synthDefs = Array.new; 
					this.initParents 
				});
				
				def = obj.asProxySynthDef(this);
				def.postln;
				synthDefs = synthDefs.add(def);
				def.writeDefFile;
				
				if(server.serverRunning, { 
					this.defToServer(def);
					loaded = true;
					if(send, { this.sendToServer(freeLast, latency, onCompletion, add) });
				 }, { loaded = false });
				
			}, { "rate/numChannels must match".inform })
			
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
		if(this.isPlaying, { group.set(\synthGate, 0.0) });
	}
	run { arg flag=true;
		if(this.isPlaying, { group.run(flag) });
	}
	
	/////////////////////////////////////////////
	
	
			
	// server communications, updating
	
	sendToServer { arg freeLast=true, latency=0.3, extraArgs, onCompletion, add=false;
		var msg, resp;
		if( synthDefs.isEmpty.not and: { server.serverRunning }, {
				msg = List.new;
				this.sendSynthMsg(msg, freeLast, extraArgs, add);
				
				if(latency.notNil, {
						SystemClock.sched(latency, {
							this.schedSendOSC(msg, onCompletion); 
						});
					
					}, {
						//doesn't work when several defs are sent at once.
						resp = OSCresponder(server.addr, '/done', {
							this.schedSendOSC(msg, onCompletion);
							resp.remove;
						}).add
				});
				
		});
	}
	
	schedSendOSC { arg msg, onCompletion;
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
	
	defToServer { arg def;
	
		server.sendMsg("/d_recv", def.asBytes);
		
		/*
		if(server.isLocal, {
				server.loadSynthDef(def.name)
			}, {
				server.sendSynthDef(def.name)
		}); 
		*/
	}
		
	
	
	sendSynthMsg { arg msg, freeLast=true, extraArgs, add=false;
				var synth;
				if(this.isPlaying.not, {
					group = Group.newMsg(msg, server, \addToHead);
					group.prIsPlaying(true);
				
				}, {
					//release current synth
					if(freeLast, {
						group.addMsg(msg, "/n_set", [\synthGate, 0.0])
					});
				});
			if(add, {
				synth = Synth.newMsg(msg, synthDefs.last.name, extraArgs, group); 
				nodeMap.updateMsg(msg, synth);
			}, {
				synthDefs.do({ arg synthDef;
					var synth;
					synth = Synth.newMsg(msg, synthDef.name, extraArgs, group); 
					nodeMap.updateMsg(msg, synth);
				});
			});
		
	
	}
	
		
	////// private /////
	
	initFor { arg obj;
		var rate, numChannels;
			if(obj.notNil, {
				//for speed
				
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
	
	wakeUpToBundle { arg bundle;
		
		if(this.isPlaying.not, { this.sendSynthMsg(bundle, true, 0) });
		//no need to wait, def is on server
	
	}
	
	wakeUpParentsToBundle { arg bundle; //see for synthDefs
		if(this.isPlaying.not, {
			if(loaded.not, { this.loadAll }); //stick in bundle later
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
		if(loaded.not && synthDefs.isEmpty.not, { 
					synthDefs.do({ arg def;
						this.defToServer(def);
					});
					loaded = true;
					
		});
	}
		
	loadAll {
		if(server.serverRunning, {
			parents.do({ arg item; item.load });
			this.load;
		}, { "server not running".inform });
	}	
	
	
}