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

	copy {
		^this.class.new.copyState(this)
	}

	copyState { |control|
		source = control.source; // source is not copied, assumed to be stateless
		channelOffset = control.channelOffset;
		paused = control.paused;
	}


}


// The stream is a pause stream. This is meant for running in the proxy to control inner properties so it is stopped when removed.


StreamControl : AbstractPlayControl {

	var stream, <clock;

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

	copyState { |control|
		// stream can't be copied.
		// control has to be rebuilt (see: NodeProxy:copyState)
		super.copyState(control);
		clock = control.clock;
	}

	copyRequiresRebuild {
		^true
	}

	controlNames {
		^if(source.isNumber or: { source.isArray }) {
			// this special key (#) allows to set the source from a slider
			ControlName('#', defaultValue:source)
		}
	}

}


PatternControl : StreamControl {

	var fadeTime, <array;

	playStream { | str |
		var dt = fadeTime.value ? 0.0;
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
				args.value.pairsDo { arg key, val; event[key] = val }
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
	var <canReleaseSynth=false, <canFreeSynth=false, <hasFadeTimeControl=false;
	var prevBundle;


	loadToBundle {} // assumes that SynthDef is loaded in the server

	asDefName { ^source }

	distributable { ^canReleaseSynth } // n_free not implemented in shared node proxy

	build { | proxy, orderIndex | 	// assumes audio rate proxy if not initialized
		var rate, desc;
		desc = this.synthDesc;
		if(desc.notNil) {
			canFreeSynth = desc.canFreeSynth;
			canReleaseSynth = desc.hasGate && canFreeSynth;
			hasFadeTimeControl = desc.controls.any { |x| x.name === \fadeTime };
		};
		if(proxy.isNeutral) { rate = \audio };
		^proxy.initBus(rate)
	}

	spawnToBundle { | bundle, extraArgs, target, addAction = 0 | // assumes self freeing
		var targetID = target.asTarget.nodeID;
		bundle.addCancel({ [9, this.asDefName, -1, addAction, targetID] ++ extraArgs.value });
	}

	playToBundle { | bundle, extraArgs, target, addAction = 1 |
		var group;
		server = target.server;
		group = target.asTarget;
		nodeID = server.nextNodeID;
		bundle.addCancel({ [9, this.asDefName, nodeID, addAction, group.nodeID] ++ extraArgs.value });
		if(paused) { bundle.addCancel(["/n_run", nodeID, 0]) };
		prevBundle = bundle;
		^nodeID
	}

	stopToBundle { | bundle, fadeTime |
		if(nodeID.notNil) {

			if(canReleaseSynth) {
				if(hasFadeTimeControl) {
					bundle.addAll([['/error', -1], [15, nodeID, \fadeTime, fadeTime, \gate, 0], ['/error', -2]])
				} {
					bundle.addAll([['/error', -1], [15, nodeID, \gate, -1.0 - fadeTime.abs], ['/error', -2]])
				}
			} {
				if(canFreeSynth.not) { //"/n_free"
					bundle.addAll([['/error', -1], [11, nodeID], ['/error', -2]]);
				}
				// otherwise it is self freeing by some inner mechanism.
			};
			nodeID = nil;
		}
	}

	freeToBundle {
		prevBundle !? { prevBundle.cancel };
	}

	set { | ... args |
		server.sendBundle(server.latency, ["/n_set", nodeID] ++ args.asOSCArgArray);
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

	copyState { |control|
		super.copyState(control);
		server = control.server;
		canReleaseSynth = control.canReleaseSynth;
		canFreeSynth = control.canFreeSynth;
	}

}


SynthDefControl : SynthControl {

	var <synthDef, <parents;
	var <bytes;

	readyForPlay { ^synthDef.notNil }

	build { | proxy, orderIndex = 0 |
		var ok, rate, numChannels, outerDefControl, outerBuildProxy, controlNames;

		outerDefControl = NodeProxy.buildProxyControl;
		outerBuildProxy = NodeProxy.buildProxy;
		NodeProxy.buildProxyControl = this;
		NodeProxy.buildProxy = proxy;
		synthDef = source.buildForProxy(proxy, channelOffset, orderIndex);
		NodeProxy.buildProxyControl = outerDefControl;
		outerBuildProxy = outerBuildProxy;

		rate = synthDef.rate;
		numChannels = synthDef.numChannels;
		ok = proxy.initBus(rate, numChannels);

		if(ok) {
			paused = proxy.paused;
			canReleaseSynth = synthDef.canReleaseSynth;
			canFreeSynth = synthDef.canFreeSynth;
			controlNames = synthDef.allControlNames;
			hasFadeTimeControl = controlNames.notNil and: {
				controlNames.any { |x| x.name === \fadeTime }
			};
		} {
			synthDef = nil;
			"synth def couldn't be built".warn;
		}
	}

	loadToBundle { | bundle, server |
		var size, path;

		// cache rendered synth def, so it can be copied if necessary (see: copyData)
		// We need to keep the bytes here, because some other instance may have deleted it from the server (see: freeToBundle)
		// the resulting synthDef will have the same name, because the name is encoded in the data (bytes).
		bytes = bytes ?? { synthDef.asBytes }; // here the work for sclang is done.
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
		};
		prevBundle = bundle;
	}

	freeToBundle { | bundle, proxy |
		if(synthDef.notNil) { bundle.addPrepare([53, synthDef.name]) }; // "/d_free"
		parents.do { |x| x.removeChild(proxy) };
		bytes = parents = nil;
		prevBundle !? { prevBundle.cancel };
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
		if(parents.includes(proxy).not) {
			parents.add(proxy);
			proxy.addChild(NodeProxy.buildProxy);
		}
	}

	controlNames { ^synthDef.allControlNames }

	copyState { |control|
		super.copyState(control);
		synthDef = control.synthDef;
		parents = control.parents.copy;
		bytes = control.bytes; // copy cached data
	}


}
