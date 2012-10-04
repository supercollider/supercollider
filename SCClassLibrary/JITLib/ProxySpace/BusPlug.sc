BusPlug : AbstractFunction {

	var <server, <bus;
	var <>monitor, <>parentGroup; // if nil, uses default group
	var busArg; // cache for "/s_new" bus arg
	var busLoaded = false;

	classvar <>defaultNumAudio=2, <>defaultNumControl=1;


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

	ar { | numChannels, offset = 0 |
		if(this.isNeutral) {
			this.defineBus(\audio, numChannels)
		};
		this.prepareOutput;
		^InBus.ar(bus, numChannels ? bus.numChannels, offset)
	}

	kr { | numChannels, offset = 0 |
		if(this.isNeutral) {
			this.defineBus(\control, numChannels)
		};
		this.prepareOutput;
		^InBus.kr(bus, numChannels ? bus.numChannels, offset)
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
		^this.value;
	}




	//  math support

	value { | something |
		var n;
		if(UGen.buildSynthDef.isNil) { ^this }; // only return when in ugen graph.
		something !? {  n = something.numChannels };
		^if(something.respondsTo(\rate) and: { something.rate == 'audio'}) { this.ar(n) } { this.kr(n) }
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
		this.freeBus;
		bus = inBus;
		this.makeBusArg;
		busLoaded = bus.server.serverRunning;
	}

	// returns false if failed
	initBus { | rate, numChannels |
		if(rate.isNil or: { rate === 'scalar' }) { ^true }; // this is no problem
		if(this.isNeutral) {
			this.defineBus(rate, numChannels);
			^true
		} {
			numChannels = numChannels ? this.numChannels;
			^(bus.rate === rate) and: { numChannels <= bus.numChannels }
		}
	}

	defineBus { | rate = \audio, numChannels |
		if(numChannels.isNil) {
			numChannels = if(rate === \audio) {
								this.class.defaultNumAudio
							} {
								this.class.defaultNumControl							}
		};
		this.bus = Bus.alloc(rate, server, numChannels);
	}

	freeBus {
		if(bus.notNil) {
			if(bus.rate === \control) { bus.setAll(0) }; // clean up
			bus.free;
			monitor.stop;
		};
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
		this.playToBundle(bundle, out.asControlInput, numChannels, group, multi, vol, fadeTime, addAction);
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
		this.playNToBundle(bundle, outs.asControlInput, amps, ins, vol, fadeTime, group, addAction);
		bundle.schedSend(this.homeServer, this.clock ? TempoClock.default, this.quant);
		this.changed(\playN, [outs, amps, ins, vol, fadeTime, group, addAction]);
	}

	fadeTime { ^0.02 }
	quant { ^nil }

	vol { ^if(monitor.isNil) { 1.0 } { monitor.vol } }
	vol_ { arg val; this.initMonitor(val) }

	monitorIndex { ^if(monitor.isNil) { nil } { monitor.out } }
	monitorGroup { ^if(monitor.isNil) { nil } { monitor.group } }

	initMonitor { | vol |
		if(this.rate !== 'audio') { Error("can only monitor audio proxy").throw };
		if(monitor.isNil) { monitor = Monitor.new };
		if (vol.notNil) { monitor.vol_(vol) };
		^monitor
	}

	stop { | fadeTime = 0.1, reset = false |
		monitor.stop(fadeTime);
		if(reset) { monitor = nil };
		this.changed(\stop, [fadeTime, reset]);
	}

	scope { | bufsize = 4096, zoom |
		if(this.isNeutral.not) { ^bus.scope(bufsize, zoom) }
	}



	// bundling messages

	playToBundle { | bundle, out, numChannels,
				group, multi=false, vol, fadeTime, addAction |
		this.newMonitorToBundle(bundle, numChannels);
		group = group ?? { if(parentGroup.isPlaying) { parentGroup } { this.homeServer.asGroup } };
		monitor.usedPlayN = false;
		monitor.playToBundle(bundle, bus.index, bus.numChannels, out, numChannels, group,
			multi, vol, fadeTime, addAction);
	}

	playNToBundle { | bundle, outs, amps, ins, vol, fadeTime, group, addAction |
		this.newMonitorToBundle(bundle); // todo: numChannels
		monitor.usedPlayN = true;
		group = group ?? { if(parentGroup.isPlaying) { parentGroup } { this.homeServer.asGroup } };
		monitor.playNBusToBundle(bundle, outs, amps, ins, bus, vol, fadeTime, group, addAction);

	}

	newMonitorToBundle { | bundle, numChannels |
		this.initBus(\audio, numChannels);
		this.initMonitor;

		if(this.isPlaying.not) { this.wakeUpToBundle(bundle) };
	}




	// netwrk node proxy support

	shared { ^false }
	homeServer { ^server }

	printOn { | stream |
		stream 	<< this.class.name << "." << bus.rate << "("
				<< server << ", " << bus.numChannels <<")";
	}
}
