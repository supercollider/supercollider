

+Object { 
	
	//objects can define their own wrapper classes dependant on 
	//how they play, stop, build, etc. see SynthDefControl for example
	
	proxyControlClass {
		^SynthDefControl
	}
	
	makeProxyControl { arg channelOffset=0;
		^this.proxyControlClass.new(this, channelOffset); 
	}
	
	
	//any preparations that have to be done to make the object
	//a synthdef, if it is possible
	//class Object assumes to be some graph builder
	
	prepareForProxySynthDef { }
	
	buildForProxy { arg proxy, channelOffset=0, index=0;
		var argNames;
		argNames = this.argNames;
		^ProxySynthDef(
			proxy.generateUniqueName ++ index,
			this.prepareForProxySynthDef(proxy),
			proxy.nodeMap.lagsFor(argNames),
			nil, 
			true, 
			channelOffset
		); 
	}
	
	argNames { ^nil }
	numChannels { ^nil } 
	
	
}

+SequenceableCollection {
	prepareForProxySynthDef { arg proxy;
		^{ this.collect({ arg item; item.value(proxy) }) }
	}
}


+SynthDef {
	buildForProxy {}
	
	hasGate {
		^if(controlNames.isNil, { 
			false 
		}, {
			controlNames.any({ arg name; name.name == "gate" })
			and:
			this.canFreeSynth
		})
	}
	canFreeSynth { ^children.canFreeSynth }
}

+SoundDef {
	buildForProxy {
		^synthDef
	}
	proxyControlClass {
		^SoundDefControl
	}

}

+Symbol {
	buildForProxy {}
	proxyControlClass {
		^SynthControl
	}
}


+AbstractPlayControl {
	makeProxyControl {} //already wrapped
}



+SimpleNumber { //some more efficient way needed to put a value here
	prepareForProxySynthDef { arg proxy;
		^if(proxy.rate === 'audio', {
			{ Array.fill(proxy.numChannels, { K2A.ar(this) }) }
		}, { 
			{  Control.kr(Array.fill(proxy.numChannels, {this})) }
		})
	}
}



+Function {
	
	defArgs {
		^def.prototypeFrame
	}
	argNames {
		^def.argNames
	}
}

//use patch later
+Instr {
	prepareForProxySynthDef {
		^func;
	}
	//see later for a player inherent scheme.
	//makeProxyControl { arg channelOffset=0;
	//	^Patch(this.name).makeProxyControl(channelOffset)
	//}
	
}

+BusPlug {
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
}

///////////////// Pattern - Streams //////////////


+Pattern {
	proxyControlClass {
		^StreamControl
	}
	buildForProxy { arg proxy, channelOffset=0;
		^this.asStream.buildForProxy(proxy, channelOffset)
	}
}


+Stream {

	buildForProxy { arg proxy, channelOffset=0;
		//assume audio rate event stream for now.
		var str, ok, argNames, msgFunc, index;
		ok = proxy.initBus('audio', 2);
		index = proxy.index;
		argNames = [\amp,\sustain,\pan];//change later
		msgFunc = { arg id, freq;
				var args, bundle, names;
				names = ~argNames ? argNames;
				args = Array.newClear(names.size * 2);
				names.do({ arg name, i; 
						args.put(i*2, name); 
						args.put(i*2+1, currentEnvironment.at(name))
					});
				
				args = args ++ [\out, index, \i_out, index, \freq, freq];
				bundle = List[[9, ~instrument, id, 1, ~group] ++ args];
				proxy.nodeMap.mapToBundle(bundle, id);
				bundle
		};

		^if(ok, {
			this.collect({ arg event;
				event.use({ 
					~nodeID = proxy.group.asNodeID; //in case of Pmono
					~argNames = ~argNames ? argNames;
					~msgFunc = msgFunc;
					~server = proxy.server;
				});
				event;
			}).asEventStreamPlayer
		}, nil);

	}
	
	proxyControlClass {
		^StreamControl
	}
}

+Pdef {
	buildForProxy { arg proxy, channelOffset=0;
		^EventStreamPlayer(this.asStream).buildForProxy(proxy, channelOffset);
	}

}
+Task {
	buildForProxy {}

}


+PauseStream {
	collect { arg func;
		^this.class.new(originalStream.collect(func), clock)
	}
}


///////////////// cx players ////////////

+AbstractPlayer {
	
	proxyControlClass {
		^CXPlayerControl
	}
	
	prepareToPlayWithProxy { arg proxy;
		var tempBus, ok;
		tempBus = proxy.asBus;
		this.prepareForPlay(bus: tempBus); //to get the rate
		ok = proxy.initBus(this.rate ? 'audio', this.numChannels ? 2);
		//source.free; 
		tempBus.free;
		^ok
	}
}

+Patch {
	
	proxyControlClass {
			^CXSynthPlayerControl
	}
	
	makeProxyControl { arg channelOffset=0, proxy;
			
			^this.proxyControlClass.new(this.wrapInFader(proxy), channelOffset); 
	}
	
	wrapInFader { arg proxy;
			this.prepareToPlayWithProxy(proxy); //do it here for now.
			^EnvelopedPlayer(this, Env.asr(0.5, 1, 0.5));
			
	}
	
}

/////////// test : query freeing


+ Object {
	canFreeSynth {
		^false
	}
}

+SequenceableCollection {
	canFreeSynth {
		^this.any({ arg item; item.canFreeSynth })
	}
}

+ EnvGen {
	canFreeSynth {
		^if(inputs.at(4) > 1, { //doneAction
			if(inputs.at(7) != -99 and: { inputs.at(0).isNumber }, { //releaseNode, gate
				false
			}, {
				true
			})
		}, { 
			false 
		})
	}
}

+ Free {
	canFreeSynth { ^true }
}

+ FreeSelf {
	canFreeSynth { ^true }
}

+ DetectSilence {
	canFreeSynth { ^true }
}




