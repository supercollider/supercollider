// lightweight objects that insulate different ways of playing/stopping.
// the bundle that is passed in is a MixedBundle


AbstractPlayControl {

	var <source, <>channelOffset;
	var <paused=false;

	classvar <>buildMethods, <>proxyControlClasses; // see wrapForNodeProxy for methods

	*new { | source, channelOffset = 0 |
		^super.newCopyArgs(source, channelOffset);
	}

	build { ^true }
	pause { this.stop }
	resume { this.start }
	nodeID { ^nil }

	readyForPlay { ^true }
	distributable { ^false } // shared proxy support

	loadToBundle {}
	spawnToBundle {} // only active in synthcontrols


	playToBundle { | bundle, args |
		bundle.addOnSendMessage(this, \play); //no latency (latency is in stream already)
		^nil //return a nil object instead of a synth
	}

	stopToBundle { | bundle |
		bundle.addOnSendMessage(this, \stop);
	}

	freeToBundle {}

	set {}
	controlNames { ^nil }

	play { this.subclassResponsibility(thisMethod) }
	stop { this.subclassResponsibility(thisMethod) }


	wakeUpParentsToBundle {}
	addParent { "wrong object in NodeProxy.buildControl".error } // for now.
	parents { ^nil }
	store {}


}


// The stream is a pause stream. This is meant for running in the proxy to control inner properties so it is stopped when removed.


StreamControl : AbstractPlayControl {

	var stream, clock;

	playToBundle { | bundle |
		// no latency (latency is in stream already)
		if(paused.not) { bundle.addOnSendMessage(this, \play) }
		^nil // return a nil object instead of a synth
	}

	build { | proxy, orderIndex = 0 |
		clock = proxy.clock;
		paused = proxy.paused;
		stream = source.buildForProxy(proxy, channelOffset, orderIndex);
		^true;
	}

	pause { stream.pause; paused=true }
	resume { | clock, quant = 1.0 |
		stream.resume(clock, quant);
		paused = false;
	}

	readyForPlay { ^stream.notNil }

	play {
		if(stream.isPlaying.not) {
			stream.play(clock, false, 0.0)
		}
	}
	stop { stream.stop }

}


PatternControl : StreamControl {

	var fadeTime, <array;

	playStream { | str |
		var dt = fadeTime.value;
		if(dt <= 0.02) {
			str.play(clock, false, 0.0)
		} {
			str.xplay(dt / clock.beatDur, clock, false, 0.0)
		}
	}

	stopStreams { | streams, dt |

		dt = (dt ? fadeTime).value;
		if(dt <= 0.02) {
			streams.do { arg item; item.stop  }
		} {
			dt = dt / clock.beatDur;
			streams.do { arg item; item.xstop(dt) };
			// make sure it is stopped, in case next is never called
			SystemClock.sched(dt, { streams.do(_.stop) });
		}
	}

	build { | proxy, orderIndex = 0 |
		fadeTime = { proxy.fadeTime }; // needed for pattern xfade
		stream = source.buildForProxy(proxy, channelOffset, orderIndex);
		clock = proxy.clock ? TempoClock.default;
		paused = proxy.paused;
		^stream.notNil
	}

	stopToBundle { | bundle, dt |
		var streams;
		streams = array.copy;
		array = nil;
		bundle.onSend({ this.stopStreams(streams, dt) });
	}

	pause {
		array.do(_.pause);
		paused = true;
	}

	resume { | clock, quant = 1.0 |
		array.do(_.resume(clock, quant));
		paused = false;
	}

	playToBundle { | bundle, args, proxy, addAction = 1 |
		var str, event;
		if(paused.not and: { stream.isPlaying.not })
		{
			str = source.buildForProxy(proxy, channelOffset);
			if(args.notNil) {
					event = str.event;
					args.pairsDo { arg key, val; event[key] = val }
			};
			array = array.add(str);
			// no latency (latency is in stream already)
			bundle.onSend {
				this.playStream(str)
			}
		}
		^nil // return a nil object instead of a synth
	}

	stop {
		this.stopStreams(array.copy);
		array = nil;
	}

}



SynthControl : AbstractPlayControl {

	var <server, <>nodeID;
	var <canReleaseSynth=false, <canFreeSynth=false;

	loadToBundle {} // assumes that SynthDef is loaded in the server

	asDefName { ^source }

	distributable { ^canReleaseSynth } // n_free not implemented in shared node proxy

	build { | proxy | 	// assumes audio rate proxy if not initialized
		var rate, desc;
		desc = this.synthDesc;
		if(desc.notNil) {
			canFreeSynth = desc.canFreeSynth;
			canReleaseSynth = desc.hasGate && canFreeSynth;
		};
		if(proxy.isNeutral) { rate = \audio };
		^proxy.initBus(rate, proxy.numChannels ? 2)
	}

	spawnToBundle { | bundle, extraArgs, target, addAction = 0 | // assumes self freeing
		var targetID = target.asTarget.nodeID;
		bundle.add([9, this.asDefName, -1, addAction, targetID]++extraArgs.asOSCArgArray);
	}

	playToBundle { | bundle, extraArgs, target, addAction = 1 |
		var group;
		server = target.server;
		group = target.asTarget;
		nodeID = server.nextNodeID;
		bundle.add([9, this.asDefName, nodeID, addAction, group.nodeID]++extraArgs.asOSCArgArray);
		if(paused) { bundle.add(["/n_run", nodeID, 0]) };
		^nodeID
	}

	stopToBundle { | bundle, fadeTime |
		if(nodeID.notNil) {
			if(canReleaseSynth) {
					bundle.addAll([['/error', -1], [15, nodeID, \gate, 0.0, \fadeTime, fadeTime], ['/error', -2]]);
			} {
					if(canFreeSynth.not) { //"/n_free"
						bundle.addAll([['/error', -1], [11, nodeID], ['/error', -2]]);
					};
					// otherwise it is self freeing by some inner mechanism.
			};
			nodeID = nil;
		}
	}

	set { | ... args |
		server.sendBundle(server.latency, ["/n_set", nodeID] ++ args);
	}

	pause { | clock, quant = 1 |
		this.run(clock, quant, false);
	}

	resume { | clock, quant = 1 |
		this.run(clock, quant, true);
	}

	run { | clock, quant, flag = true |
		if(nodeID.notNil) {
			(clock ? SystemClock).play({
				server.sendMsg("/n_run", nodeID, flag.binaryValue);
				paused = flag.not;
				nil;
			}, quant)
		} { paused = flag.not; }
	}

	synthDesc {
		var dict = SynthDescLib.global.synthDescs;
		^if(dict.notNil) { dict.at(this.asDefName.asSymbol) } { nil };
	}

	controlNames {
		var desc = this.synthDesc;
		^if(desc.notNil) { desc.controls } { nil }
	}

	synthDefPath { ^SynthDef.synthDefDir ++ this.asDefName ++ ".scsyndef" }

	store { SynthDescLib.global.read(this.synthDefPath) }

}


SynthDefControl : SynthControl {

	var <synthDef, <parents;

	readyForPlay { ^synthDef.notNil }

	build { | proxy, orderIndex = 0 |
		var ok, rate, numChannels;

		NodeProxy.buildProxyControl = this;
		synthDef = source.buildForProxy(proxy, channelOffset, orderIndex);
		NodeProxy.buildProxyControl = nil;

		rate = synthDef.rate ?? { if(proxy.rate !== \control) { \audio } { \control } };
		numChannels = synthDef.numChannels ? proxy.numChannels ? 2;
		ok = proxy.initBus(rate, numChannels);

		if(ok and: { synthDef.notNil}) {
			paused = proxy.paused;
			canReleaseSynth = synthDef.canReleaseSynth;
			canFreeSynth = synthDef.canFreeSynth;
		} {
			synthDef = nil;
		}
	}

	loadToBundle { | bundle, server |
		var bytes, size, path;

		bytes = synthDef.asBytes;
		size = bytes.size;
		size = size - (size bitAnd: 3) + 84; // 4 + 4 + 16 + 16 // appx path length size + overhead
		if(server.options.protocol === \tcp or: { size < 16383}) {
			// full size: 65535, but does not work.
			bundle.addPrepare([5, bytes]); // "/d_recv"
		} {
			// bridge exceeding bundle size by writing to disk
			if(server.isLocal.not) {
				Error("SynthDef too large (" ++ size
				++ " bytes) to be sent to remote server via udp").throw;
			};
			path = this.synthDefPath;
			this.writeSynthDefFile(path, bytes);
			bundle.addPrepare([6, path]); // "/d_load"
		}
	}

	freeToBundle { | bundle |
		if(synthDef.notNil) { bundle.addPrepare([53, synthDef.name]) } // "/d_free"
	}

	writeSynthDefFile { | path, bytes |
		var file = File(path, "w");
		protect { file.putAll(bytes) } { file.close }
	}

	asDefName { ^synthDef.name }

	wakeUpParentsToBundle { | bundle, checkedAlready |
		parents.do { | proxy |
			proxy.wakeUpToBundle(bundle, checkedAlready)
		}
	}

	addParent { | proxy |
		if(parents.isNil) { parents = IdentitySet.new };
		parents.add(proxy);
	}

	controlNames { ^synthDef.allControlNames }

}
