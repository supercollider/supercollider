// these extensions provide means to wrap Objects so that they
// make sense within the server bus system according to a node proxy.

////////////////////// graphs, functions, numericals and arrays //////////////////

+ Object {

	// objects can define their own wrapper classes dependent on
	// how they play, stop, build, etc. see SynthDefControl for example
	// the original (wrapped) object can be reached by the .source message
	// for objects that only create ugen graphs, define prepareForProxySynthDef(proxy)

	proxyControlClass {
		^SynthDefControl
	}

	makeProxyControl { | channelOffset = 0 |
		^this.proxyControlClass.new(this, channelOffset);
	}


	// any preparations that have to be done to prepare the object
	// implement 'prepareForProxySynthDef' to return a ugen func


	// this method is called from within the Control
	buildForProxy { | proxy, channelOffset = 0 |
		var channelConstraint, rateConstraint;
		var argNames = this.argNames;
		if(proxy.fixedBus) {
			channelConstraint = proxy.numChannels;
			rateConstraint = proxy.rate;
		};
		^ProxySynthDef(
			SystemSynthDefs.tempNamePrefix ++ proxy.generateUniqueName ++ UniqueID.next,
			this.prepareForProxySynthDef(proxy, channelOffset),
			proxy.nodeMap.ratesFor(argNames),
			nil,
			true,
			channelOffset,
			channelConstraint,
			rateConstraint
		);
	}
	prepareForProxySynthDef { ^this.subclassResponsibility(thisMethod) }

	defaultArgs { ^nil }
	argNames { ^nil }

	nodeMapMapsToControl { ^false }

	// support for unop / binop proxy
	isNeutral { ^true }
	initBus { ^true }
	wakeUp {}

}


+ Function {
	prepareForProxySynthDef { ^this }
	argNames { ^def.argNames }
	defaultArgs { ^def.prototypeFrame }
}

+ AbstractFunction {
	prepareForProxySynthDef { ^{ this.value } }
}

+ SimpleNumber {
	proxyControlClass { ^StreamControl }

	buildForProxy { | proxy, channelOffset = 0 |
		^[this].buildForProxy(proxy, channelOffset)
	}
}

+ Synth { // better information about common error
	prepareForProxySynthDef { | proxy |
		Error(
			"A synth is no valid source for a proxy.\n"
			"For instance, ~out = { ... }.play would cause this and should be:\n"
			"~out = { ... }; ~out.play; or (~out = { ... }).play;"
		).throw;
	}
}

+ Array {

	proxyControlClass { ^StreamControl }

	buildForProxy { | proxy, channelOffset = 0 |
		proxy.initBus(\control, this.size);
		^(
			type: \fadeBus,
			array: this,
			proxy: proxy,
			channelOffset: channelOffset,
			server: proxy.server,
			finish: #{
				var proxy = ~proxy;
				~array = ~array.wrapExtend(proxy.numChannels);
				~out = proxy.index + ~channelOffset;
				~group = proxy.group;
				~rate = proxy.rate;
				~numChannels = proxy.numChannels;
				~fadeTime = proxy.fadeTime;
				~curve = proxy.nodeMap.at(\curve) ? 1.0;
			}
		)
	}
}

+ SequenceableCollection {
	prepareForProxySynthDef { | proxy |
		^{ this.collect({ |el| el.prepareForProxySynthDef(proxy).value }) }
		// could use SynthDef.wrap, but needs type check for function.
	}
}

+ BusPlug {
	prepareForProxySynthDef { | proxy |
		proxy.initBus(this.rate, this.numChannels);
		^{ this.value(proxy) }
	}

	nodeMapMapsToControl { ^true }

}

+ AbstractOpPlug {
	prepareForProxySynthDef { | proxy |
		proxy.initBus(this.rate, this.numChannels);
		^{ this.value(proxy) }
	}
}

// needs a visit: lazy init + channelOffset

+ Bus {
	prepareForProxySynthDef { | proxy |
		^BusPlug.for(this).prepareForProxySynthDef(proxy);
	}
}




///////////////////////////// SynthDefs and alike ////////////////////////////////////


+ SynthDef {
	buildForProxy {}
	numChannels { ^nil } // cannot know this
	rate { ^nil }
}



+ Symbol {
	buildForProxy {}
	proxyControlClass {
		^SynthControl
	}
}

///////////////////////////// Pattern - Streams ///////////////////////////////////


+ Stream {
	proxyControlClass { ^StreamControl }
	buildForProxy { ^PauseStream.new(this) }
}

+ PauseStream {
	buildForProxy { ^this }
	proxyControlClass { ^StreamControl }
}

+ PatternProxy {
	buildForProxy { "a numerical pattern does not make sense here.".error; ^nil }
}

+ TaskProxy {
	proxyControlClass { ^StreamControl }

	buildForProxy { | proxy, channelOffset = 0 |
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

+ EventPatternProxy {
	proxyControlClass { ^PatternControl }

	buildForProxy {  | proxy, channelOffset = 0 |
		proxy.initBus(\audio);
		^this.endless.buildForProxy(proxy, channelOffset)
	}
}


+ Pattern {
	proxyControlClass { ^PatternControl }

	buildForProxy { | proxy, channelOffset = 0 |
		var player = this.asEventStreamPlayer;
		var event = player.event.buildForProxy(proxy, channelOffset);
		proxy.initBus(\audio);
		^event !? { player };
	}
}

+ Event {
	proxyControlClass { ^StreamControl }
	buildForProxy { | proxy, channelOffset = 0 |
		var ok, index, server, numChannels, rate, finish, out, group;
		ok = if(proxy.isNeutral) {
			rate = this.at(\rate) ? 'audio';
			numChannels = this.at(\numChannels);
			numChannels !? { numChannels = numChannels + channelOffset };
			proxy.initBus(rate, numChannels);
		} {
			rate = proxy.rate; // if proxy is initialized, it is user's responsibility
			numChannels = proxy.numChannels;
			true
		};
		^if(ok) {
			index = proxy.index;
			server = proxy.server;
			out = {
				var n = proxy.numChannels;
				if(n.isNil) { -1 } {
					(~channelOffset ? channelOffset) % n + index
				}
			};
			group = { proxy.group.asNodeID };
			this.use({
				~proxy = proxy;
				~server = server;
				~group = group;
				~out = out.value;
				finish = ~finish;
				~finish = {
					finish.value;
					proxy.nodeMap.addToEvent(currentEnvironment);
					~out = out.value;
				}
			});
			this
		} { nil }
	}
}



/////////// pluggable associations //////////////


+ Association {
	buildForProxy { | proxy, channelOffset = 0, index |
		^AbstractPlayControl.buildMethods[key].value(value, proxy, channelOffset, index)
	}
	proxyControlClass {
		^AbstractPlayControl.proxyControlClasses[key] ? SynthDefControl
	}
}

+ AbstractPlayControl {
	makeProxyControl { ^this.deepCopy } //already wrapped, but needs to be copied

	/* these adverbial extendible interfaces are for supporting different role schemes.
	it is called by Association, so ~out = \filter -> ... will call this. The first arg passed is 	the value of the association */

	*initClass {
		proxyControlClasses = (
			filter: SynthDefControl,
			xset: StreamControl,
			pset: StreamControl,
			set: StreamControl,
			seti: StreamControl,
			stream: PatternControl,
			setbus: StreamControl,
			setsrc: StreamControl
		);

		buildMethods = (

			filter: #{ | func, proxy, channelOffset = 0, index |
				var ok, ugen;
				if(proxy.isNeutral) {
					ugen = func.value(Silent.ar);
					ok = proxy.initBus(ugen.rate, ugen.numChannels + channelOffset);
					if(ok.not) { Error("NodeProxy input: wrong rate/numChannels").throw }
				};

				{ | out |
					var env, ctl = NamedControl.kr("wet" ++ (index ? 0), 1.0);
					if(proxy.rate === 'audio') {
						env = ctl * EnvGate(i_level: 0, doneAction:2, curve:\sin);
						XOut.ar(out, env, SynthDef.wrap(func, nil, [In.ar(out, proxy.numChannels)]))
					} {
						env = ctl * EnvGate(i_level: 0, doneAction:2, curve:\lin);
						XOut.kr(out, env, SynthDef.wrap(func, nil, [In.kr(out, proxy.numChannels)]))
                    };
				}.buildForProxy( proxy, channelOffset, index )

			},
			set: #{ | pattern, proxy, channelOffset = 0, index |
				var args = proxy.controlNames.collect(_.name);
				Pchain(
					(type: \set, \id: { proxy.group.nodeID }),
					pattern,
					(args: args)
				).buildForProxy( proxy, channelOffset, index )
			},
			pset: #{ | pattern, proxy, channelOffset = 0, index |
				Pbindf(
					pattern,
					\play, { proxy.set(*proxy.controlNames.collect(_.name).envirPairs.asOSCArgArray) }
				).buildForProxy( proxy, channelOffset, index )
			},
			xset: #{ | pattern, proxy, channelOffset = 0, index |
				Pbindf(
					pattern,
					\play, { proxy.xset(*proxy.controlNames.collect(_.name).envirPairs.asOSCArgArray) }
				).buildForProxy( proxy, channelOffset, index )
			},
			setbus: #{ | pattern, proxy, channelOffset = 0, index |
				var ok = proxy.initBus(\control);
				if(ok.not) { Error("NodeProxy input: wrong rate").throw };
				Pbindf(
					pattern,
					\type, \bus,
					\id, Pfunc { proxy.group.nodeID },
					\array, Pkey(\value).collect { |x| x.asArray.keep(proxy.numChannels) },
					\out, Pfunc { proxy.index }
				).buildForProxy( proxy, channelOffset, index )
			},
			setsrc: #{ | pattern, proxy, channelOffset = 0, index = 0 |
				pattern.collect { |event|
					event[\type] = \rest;
					proxy.put(index + 1, event[\source]);
					event
				}.buildForProxy( proxy, channelOffset, index );
			},
			control: #{ | values, proxy, channelOffset = 0, index |
				{ Control.kr(values) }.buildForProxy( proxy, channelOffset, index );
			},
			filterIn: #{ | func, proxy, channelOffset = 0, index |
				var ok, ugen;
				if(proxy.isNeutral) {
					ugen = func.value(Silent.ar);
					ok = proxy.initBus(ugen.rate, ugen.numChannels + channelOffset);
					if(ok.not) { Error("NodeProxy input: wrong rate/numChannels").throw }
				};

				{ | out |
					var in, env;
					var wetamp = NamedControl.kr("wet" ++ (index ? 0), 1.0);
					var dryamp = 1 - wetamp;
					var sig = { |in| SynthDef.wrap(func, nil, [in * wetamp]) + (dryamp * in) };

					if(proxy.rate === 'audio') {
						in = In.ar(out, proxy.numChannels);
						env = EnvGate(i_level: 0, doneAction:2, curve:\sin);
						XOut.ar(out, env, sig.(in))
					} {
						in = In.kr(out, proxy.numChannels);
						env = EnvGate(i_level: 0, doneAction:2, curve:\lin);
						XOut.kr(out, env, sig.(in))
					};
				}.buildForProxy( proxy, channelOffset, index )
			},

			mix: #{ | func, proxy, channelOffset = 0, index |

				{
					var ctl = NamedControl.kr("mix" ++ (index ? 0), 1.0);
					var sig = SynthDef.wrap(func);
					var curve = if(sig.rate === \audio) { \sin } { \lin };
					var env = EnvGate(i_level: 0, doneAction:2, curve:curve);

					ctl * sig * env

				}.buildForProxy( proxy, channelOffset, index )
			},

			// seti role: set controls for a specific channel in a multi channel NodeProxy
			seti: #{ |pattern, proxy, channelOffset = 0, index|
				var args = proxy.controlNames.collect(_.name);
				Pchain(
					(type: \set, id: { proxy.group.nodeID }, args: args),
					Pbindf(
						pattern,
						\play, Pfunc { |e| args.do { |n|
							e[n] !? {
								proxy.seti(n, e.channelOffset, e[n])
							}
						}}
					)
				).buildForProxy(proxy, channelOffset, index)
			}
		)
	}
}
