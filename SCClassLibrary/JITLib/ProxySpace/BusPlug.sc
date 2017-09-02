BusPlug : AbstractFunction {

	var <server, <bus;
	var <>monitor, <>parentGroup; // if nil, uses default group
	var busArg; // cache for "/s_new" bus arg
	var busLoaded = false;
	var <>reshaping; // \elastic, \expanding

	classvar <>defaultNumAudio=2, <>defaultNumControl=1, <>defaultReshaping;

	*new { | server |
		^super.newCopyArgs(server ? Server.default);
	}

	*for { | bus |
		bus = bus.asBus;
		^this.new(bus.server).bus_(bus)
	}

	*audio { | server, numChannels |
		^this.new(server).defineBus(\audio, numChannels)
	}

	*control { | server, numChannels |
		^this.new(server).defineBus(\control, numChannels)
	}

	clear {
		this.free;
		this.stop;
		this.freeBus;
		monitor = nil;
		this.changed(\clear);
	}


	//  playing and access

	rate {  ^if(bus.isNil) { \scalar } { bus.rate } }
	numChannels {  ^if(bus.isNil) { nil } { bus.numChannels } }
	index { ^if(bus.isNil) { nil } { bus.index } }

	fixedBus {
		^reshaping.isNil
	}

	isNeutral {
		^bus.isNil or: { bus.index.isNil and: { bus.numChannels.isNil } }
	}

	isMonitoring {
		^monitor.isPlaying
	}

	isPlaying {
		^this.index.notNil and: { server.serverRunning }
	}

	prepareOutput { } // see subclass
	clock { ^nil  }

	ar { | numChannels, offset = 0, clip = \wrap |
		var output;
		if(this.isNeutral) {
			this.defineBus(\audio, numChannels)
		};
		this.prepareOutput;
		output = InBus.ar(bus, numChannels, offset, clip);
		// always return an array if no channel size is specified
		^if(numChannels.isNil) { output.asArray } { output }
	}

	kr { | numChannels, offset = 0, clip = \wrap |
		var output;
		if(this.isNeutral) {
			this.defineBus(\control, numChannels)
		};
		this.prepareOutput;
		output = InBus.kr(bus, numChannels, offset, clip);
		// always return an array if no channel size is specified
		^if(numChannels.isNil) { output.asArray } { output }
	}

	asStream {
		^Routine { loop { this.asControlInput.yield } }
	}

	embedInStream { | inval | // for now, force multichannel expansion in streams early.
		^this.asControlInput.embedInStream(inval);
	}

	asControlInput {
		if(this.isPlaying.not) {
			if(this.isNeutral) { this.defineBus(\control, 1) };
			this.wakeUp
		};
		^this.busArg;
	}

	asUGenInput {
		^this.value(nil)
	}


	//  math support

	value { | something |
		var n;
		if(UGen.buildSynthDef.isNil) { ^this }; // only return when in ugen graph.
		something !? { n = something.numChannels };
		^if(something.respondsTo(\rate) and: { something.rate == 'audio'} or: { this.rate == \audio }) {
			this.ar(n)
		} {
			this.kr(n)
		}

	}

	composeUnaryOp { | aSelector |
		^UnaryOpPlug.new(aSelector, this)
	}
	composeBinaryOp { | aSelector, something |
		^BinaryOpPlug.new(aSelector, this, something)
	}
	reverseComposeBinaryOp { | aSelector, something |
		^BinaryOpPlug.new(aSelector, something, this)
	}
	composeNAryOp { |aSelector, anArgList|
		^thisMethod.notYetImplemented
		//^NAryOpPlug.new(aSelector, [this]++anArgList) // nary op ugens are not yet implemented
	}


	// bus initialization


	bus_ { | inBus |
		var bundle;
		if(bus != inBus) {
			this.setBus(inBus);
			if(monitor.isPlaying) {
				bundle = OSCBundle.new;
				monitor.stopToBundle(bundle);
				monitor.playNBusToBundle(bundle, bus: inBus);
				bundle.schedSend(server);
			}
		};
	}


	// you have to stop and play explicitly
	setBus { | inBus |
		if(inBus.isNil) { Error("BusPlug:setBus: bus can't be set to nil.").throw };
		if(bus != inBus) {
			//postf("% has new bus: % \nold bus was: %\n", this, inBus, bus);
			this.freeBus;
			bus = inBus;
			this.makeBusArg;
			busLoaded = bus.server.serverRunning;
			this.changed(\bus, bus);
		}
	}

	// returns false if failed

	initBus { | rate, numChannels |
		if(rate == \scalar) { ^true }; // no bus output
		if(this.isNeutral) {
			this.defineBus(rate, numChannels);
			^true
		};

		numChannels = numChannels ? this.numChannels;
		rate = rate ? this.rate;

		if(numChannels == bus.numChannels and: { rate == bus.rate }) {
			^true // already there
		};

		if(reshaping.isNil or: { reshaping == \static }) {
			^(this.rate === rate) and: { numChannels <= bus.numChannels }
		};
		// for now: always reshape on rate change, because rate adaption happens earlier.
		if(this.rate != rate) {
			this.defineBus(rate, numChannels);
			^true
		};
		if(reshaping == \elastic and: { numChannels != bus.numChannels  }) {
			this.defineBus(rate, numChannels);
			^true
		};
		if(reshaping == \expanding) {
			if(numChannels > bus.numChannels) {
				this.defineBus(rate, numChannels);
			};
			^true
		};
		Error("reshaping '%' not implemented".format(reshaping)).throw;
	}

	defineBus { | rate = \audio, numChannels |
		numChannels = numChannels ? this.numChannels;
		if(rate != \audio) { rate = \control };
		if(numChannels.isNil) {
			numChannels = if(rate === \audio) {
				this.class.defaultNumAudio
			} {
				this.class.defaultNumControl
			}
		};
		this.setBus(Bus.alloc(rate, server, numChannels));
	}

	freeBus {
		var oldBus = bus;
		oldBus.free(true);
		busArg = bus = nil;
		busLoaded = false;
	}

	busArg { ^busArg ?? { this.makeBusArg } }

	makeBusArg {
		var index, numChannels, prefix;
		if(bus.isNil) { ^busArg = "" }; // still neutral
		prefix = if(this.rate == \audio) { "\a" } { "\c" };
		index = this.index;
		numChannels = this.numChannels;
		^busArg = if(numChannels == 1) {
			prefix ++ index
		} {
			{ |i| prefix ++ (index + i) }.dup(numChannels)
		}
	}

	asMap {
		^this.busArg
	}

	wakeUpToBundle {}
	wakeUp {}
	asBus { ^if(this.isNeutral) { nil } { bus } }
	asNodeArg { ^if(this.isNeutral) { nil } { this.busArg } }





	// monitoring


	play { | out, numChannels, group, multi=false, vol, fadeTime, addAction |
		var bundle = MixedBundle.new;
		if(this.homeServer.serverRunning.not) {
			("server not running:" + this.homeServer).warn;
			^this
		};
		if(bus.rate == \control) { "Can't monitor a control rate bus.".warn; monitor.stop; ^this };
		group = group ?? {this.homeServer.defaultGroup};
		this.playToBundle(bundle, out, numChannels, group, multi, vol, fadeTime, addAction);
		// homeServer: multi client support: monitor only locally
		bundle.schedSend(this.homeServer, this.clock ? TempoClock.default, this.quant);
		this.changed(\play, [out, numChannels, group, multi, vol, fadeTime, addAction]);
	}

	playN { | outs, amps, ins, vol, fadeTime, group, addAction |
		var bundle = MixedBundle.new;
		if(this.homeServer.serverRunning.not) {
			("server not running:" + this.homeServer).warn;
			^this
		};
		if(bus.rate == \control) { "Can't monitor a control rate bus.".warn; monitor.stop; ^this };
		group = group ?? {this.homeServer.defaultGroup};
		this.playNToBundle(bundle, outs, amps, ins, vol, fadeTime, group, addAction);
		bundle.schedSend(this.homeServer, this.clock ? TempoClock.default, this.quant);
		this.changed(\playN, [outs, amps, ins, vol, fadeTime, group, addAction]);
	}

	fadeTime { ^0.02 }
	quant { ^nil }

	vol { ^if(monitor.isNil) { 1.0 } { monitor.vol } }
	vol_ { |val| this.initMonitor(val) }

	monitorIndex { ^if(monitor.isNil) { nil } { monitor.out } }
	monitorGroup { ^if(monitor.isNil) { nil } { monitor.group } }

	initMonitor { | vol |
		if(monitor.isNil) { monitor = Monitor.new };
		if (vol.notNil) { monitor.vol_(vol) };
		^monitor
	}

	stop { | fadeTime, reset = false |
		monitor.stop(fadeTime);
		if(reset) { monitor = nil };
		this.changed(\stop, [fadeTime, reset]);
	}

	scope { | bufsize = 4096, zoom |
		if(this.isNeutral.not) { ^bus.scope(bufsize, zoom) } { "Can't scope unintitialized bus".warn }
	}

	plot { | duration = 0.01, bounds, minval, maxval, separately = false |
		^if(this.isNeutral.not) {
			this.wakeUp;
			bus.plot(duration, bounds, minval, maxval, separately);
		} { "Can't plot unintitialized bus".warn; nil }
	}


	// bundling messages

	playToBundle { | bundle, out, numChannels,
		group, multi=false, vol, fadeTime, addAction |
		if(bus.rate == \control) {
			"Can't monitor a control rate bus.".warn;
			monitor !? { monitor.stopToBundle(bundle) };
			^this
		};
		this.newMonitorToBundle(bundle, numChannels);
		group = group ?? { if(parentGroup.isPlaying) { parentGroup } };
		out = out.asControlInput;
		if(numChannels.notNil) { out = (0..numChannels-1) + (out ? 0) };
		monitor.playNBusToBundle(bundle, out, nil, nil, bus, vol, fadeTime, group, addAction, multi);
	}

	playNToBundle { | bundle, outs, amps, ins, vol, fadeTime, group, addAction |
		if(bus.rate == \control) {
			"Can't monitor a control rate bus.".warn;
			monitor !? { monitor.stopToBundle(bundle) };
			^this
		};
		this.newMonitorToBundle(bundle, ins !? { ins.asArray.maxItem + 1 });
		group = group ?? { if(parentGroup.isPlaying) { parentGroup } };
		monitor.playNBusToBundle(bundle, outs, amps, ins, bus, vol, fadeTime, group, addAction);
	}

	newMonitorToBundle { | bundle, numChannels |
		if(this.isNeutral) { this.initBus(\audio, numChannels) };
		this.initMonitor;
		if(this.isPlaying.not) { this.wakeUpToBundle(bundle) };
	}

	// making copies

	copy {
		^this.class.new(server).copyState(this)
	}

	copyState { | proxy |
		var proxyBus = proxy.bus;
		if(this.isNeutral.not) { this.clear };
		parentGroup = proxy.parentGroup;
		reshaping = proxy.reshaping;
		monitor = proxy.monitor.copy;
		proxyBus !? {
			bus = Bus.alloc(proxyBus.rate, proxyBus.server, proxyBus.numChannels);
			this.makeBusArg;
		}
	}

	// server state

	serverQuit {
		busLoaded = false;
	}


	// network node proxy support

	shared { ^false }
	homeServer { ^server }

	printOn { | stream |
		stream 	<< this.class.name << "." << bus.rate << "("
		<< server << ", " << bus.numChannels <<")";
	}
}
