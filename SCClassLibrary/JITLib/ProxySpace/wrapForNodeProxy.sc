//these extensions provide means to wrap Objects so that they
//make sense within the server bus system according to a node proxy.

////////////////////// graphs, functions, numericals and arrays //////////////////

+Object {

	//objects can define their own wrapper classes dependant on
	//how they play, stop, build, etc. see SynthDefControl for example
	//the original (wrapped) object can be reached by the .source message
	//for objects that only create ugen graphs, define prepareForProxySynthDef(proxy)

	proxyControlClass {
		^SynthDefControl
	}

	makeProxyControl { arg channelOffset=0;
		^this.proxyControlClass.new(this, channelOffset);
	}


	//any preparations that have to be done to prepare the object
	//implement 'prepareForProxySynthDef' to return a ugen func


	//this method is called from within the Control
	buildForProxy { arg proxy, channelOffset=0, index;
		var argNames;
		argNames = this.argNames;
		^ProxySynthDef(
			SystemSynthDefs.tempNamePrefix ++ proxy.generateUniqueName ++ index,
			this.prepareForProxySynthDef(proxy),
			proxy.nodeMap.ratesFor(argNames),
			nil,
			true,
			channelOffset,
			proxy.numChannels,
			proxy.rate
		);
	}
	prepareForProxySynthDef { ^this.subclassResponsibility(thisMethod) }

	defaultArgs { ^nil }
	argNames { ^nil }

	//support for unop / binop proxy
	isNeutral { ^true }
	initBus { ^true }
	wakeUp {}

}


+Function {
	prepareForProxySynthDef { ^this }
	argNames { ^def.argNames }
	defaultArgs { ^def.prototypeFrame }
}

+AbstractFunction {
	prepareForProxySynthDef { ^{ this.value } }
}

+SimpleNumber {

	prepareForProxySynthDef { arg proxy;
		proxy.initBus(\control, 1);
		^{DC.multiNewList([proxy.rate] ++ this) };
	}
}

+Synth { // better information about common error
	prepareForProxySynthDef { arg proxy;
		Error(
			"A synth is no valid source for a proxy.\n"
			"For instance, ~out = { ... }.play would cause this and should be:\n"
			"~out = { ... }; ~out.play; or (~out = { ... }).play;"
		).throw;
	}
}

+RawArray {
	prepareForProxySynthDef { arg proxy;
		proxy.initBus(\control, this.size);
		^{DC.multiNewList([proxy.rate] ++ this) };
	}
}

+SequenceableCollection {
	prepareForProxySynthDef { arg proxy;
		proxy.initBus(\control, this.size);
		^{ this.collect({ |el| el.prepareForProxySynthDef(proxy).value }) }
		// could use SynthDef.wrap, but needs type check for function.
	}
}

+BusPlug {
	prepareForProxySynthDef { arg proxy;
		proxy.initBus(this.rate, this.numChannels);
		^{ this.value(proxy) }
	}

}

+AbstractOpPlug {
	prepareForProxySynthDef { arg proxy;
		proxy.initBus(this.rate, this.numChannels);
		^{ this.value(proxy) }
	}
}

//needs a visit: lazy init + channelOffset

+Bus {
	prepareForProxySynthDef { arg proxy;
		^BusPlug.for(this).prepareForProxySynthDef(proxy);
	}
}




///////////////////////////// SynthDefs and alike ////////////////////////////////////


+SynthDef {
	buildForProxy {}
	numChannels { ^nil } //don't know
	rate { ^nil }
}



+Symbol {
	buildForProxy {}
	proxyControlClass {
		^SynthControl
	}
}

///////////////////////////// Pattern - Streams ///////////////////////////////////


+Stream {
	proxyControlClass { ^StreamControl }
	buildForProxy { ^PauseStream.new(this) }
}

+PauseStream {
	buildForProxy { ^this }
	proxyControlClass { ^StreamControl }
}

+PatternProxy {
	buildForProxy { "a numerical pattern does not make sense here.".error; ^nil }
}

+TaskProxy {
	proxyControlClass { ^StreamControl }

	buildForProxy {  arg proxy, channelOffset=0;
		^PauseStream(this.endless.asStream
			<> (
				nodeProxy: proxy,
				channelOffset: channelOffset,
				server: { proxy.server },
				out: { proxy.index },
				group: { proxy.group }
			)
		)
	}
}

+EventPatternProxy {
	proxyControlClass { ^PatternControl }

	buildForProxy {  arg proxy, channelOffset=0;
		^this.endless.buildForProxy(proxy, channelOffset)
	}
}


+Pattern {
	proxyControlClass { ^PatternControl }

	buildForProxy { arg proxy, channelOffset=0;
		var player = this.asEventStreamPlayer;
		var event = player.event.buildForProxy(proxy, channelOffset);
		^event !? { player };
	}
}

+ Event {
	proxyControlClass { ^StreamControl }
	buildForProxy { arg proxy, channelOffset=0;
		var ok, index, server, numChannels, rate, finish;
		ok = if(proxy.isNeutral) {
			rate = this.at(\rate) ? 'audio';
			numChannels = this.at(\numChannels) ? NodeProxy.defaultNumAudio;
			proxy.initBus(rate, numChannels);
		} {
			rate = proxy.rate; // if proxy is initialized, it is user's responsibility
			numChannels = proxy.numChannels;
			true
		};
		^if(ok) {
				index = proxy.index;
				server = proxy.server;
				this.use({
					~channelOffset = channelOffset; // default value
					~out = { ~channelOffset % numChannels + index };
					~server = server; // not safe for server changes yet
					finish = ~finish;
					~group = { proxy.group.asNodeID };
					~finish = {
						finish.value;
						proxy.nodeMap.addToEvent(currentEnvironment);
						~group = ~group.value;
						~out = ~out.value;
					}
				});
				this
		} { nil }
	}
}



/////////// pluggable associations //////////////


+Association {
	buildForProxy { arg proxy, channelOffset=0, index;
		^AbstractPlayControl.buildMethods[key].value(value, proxy, channelOffset, index)
	}
	proxyControlClass {
		^AbstractPlayControl.proxyControlClasses[key] ? SynthDefControl
	}
}

+AbstractPlayControl {
	makeProxyControl { ^this.deepCopy } //already wrapped, but needs to be copied

	/* these adverbial extendible interfaces are for supporting different role schemes.
	it is called by Association, so ~out = \filter -> ... will call this. The first arg passed is 	the value of the association */

	*initClass {
		proxyControlClasses = (
			filter: SynthDefControl,
			xset: StreamControl,
			set: StreamControl,
			stream: PatternControl,
			setbus: StreamControl,
			setsrc: StreamControl
		);

		buildMethods = (

		filter: #{ arg func, proxy, channelOffset=0, index;
			var ok, ugen;
			if(proxy.isNeutral) {
				ugen = func.value(Silent.ar);
				ok = proxy.initBus(ugen.rate, ugen.numChannels);
				if(ok.not) { Error("NodeProxy input: wrong rate/numChannels").throw }
			};

			{ arg out;
				var e;
				e = EnvGate.new * Control.names(["wet"++(index ? 0)]).kr(1.0);
				if(proxy.rate === 'audio') {
					XOut.ar(out, e, SynthDef.wrap(func, nil, [In.ar(out, proxy.numChannels)]))
				} {
					XOut.kr(out, e, SynthDef.wrap(func, nil, [In.kr(out, proxy.numChannels)]))				};
			}.buildForProxy( proxy, channelOffset, index )

		},
		set: #{ arg pattern, proxy, channelOffset=0, index;
			var args;
			args = proxy.controlNames.collect(_.name);
			Pbindf(
				pattern,
				\type, \set,
				\id, Pfunc { proxy.group.nodeID },
				\args, args
			).buildForProxy( proxy, channelOffset, index )
		},
		xset: #{ arg pattern, proxy, channelOffset=0, index;
			Pbindf(
				pattern,
				\play, { proxy.xset(*proxy.controlNames.collect(_.name).envirPairs) }
			).buildForProxy( proxy, channelOffset, index )
		},
		setbus: #{ arg pattern, proxy, channelOffset=0, index;
			var ok = proxy.initBus(\control);
			if(ok.not) { Error("NodeProxy input: wrong rate").throw };
			Pbindf(
				pattern,
				\type, \bus,
				\id, Pfunc { proxy.group.nodeID },
				\array, Pkey(\value).collect { |x| x.keep(proxy.numChannels) },
				\out, Pfunc { proxy.index }
			).buildForProxy( proxy, channelOffset, index )
		},
		setsrc: #{ arg pattern, proxy, channelOffset=0, index=0;
			pattern.collect { |event|
				event[\type] = \rest;
				proxy.put(index + 1, event[\source]);
				event
			}.buildForProxy( proxy, channelOffset, index );
		},
		control: #{ arg values, proxy, channelOffset=0, index;
			{ Control.kr(values) }.buildForProxy( proxy, channelOffset, index );
		},
		filterIn: #{ arg func, proxy, channelOffset=0, index;
			var ok, ugen;
			if(proxy.isNeutral) {
				ugen = func.value(Silent.ar);
				ok = proxy.initBus(ugen.rate, ugen.numChannels);
				if(ok.not) { Error("NodeProxy input: wrong rate/numChannels").throw }
			};

			{ arg out;
				var in;
				var egate = EnvGate.new;
				var wetamp = Control.names(["wet"++(index ? 0)]).kr(1.0);
				var dryamp = 1 - wetamp;
				if(proxy.rate === 'audio') {
					in = In.ar(out, proxy.numChannels);
					XOut.ar(out, egate, SynthDef.wrap(func, nil,
						[in * wetamp]) + (dryamp * in))
				} {
					in = In.kr(out, proxy.numChannels);
					XOut.kr(out, egate, SynthDef.wrap(func, nil,
						[in * wetamp]) + (dryamp * in))
				};
			}.buildForProxy( proxy, channelOffset, index )
		},

		mix: #{ arg func, proxy, channelOffset=0, index;

			{ var e = EnvGate.new * Control.names(["mix"++(index ? 0)]).kr(1.0);
				e * SynthDef.wrap(func);
			}.buildForProxy( proxy, channelOffset, index )
		};

		)
	}
}
