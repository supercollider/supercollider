
//these extensions provide means to wrap Objects so that they 
//make sense within the server bus system according to a node proxy.

////////////////////// graphs, functions, numericals and arrays //////////////////

+Object { 
	
	//objects can define their own wrapper classes dependant on 
	//how they play, stop, build, etc. see SynthDefControl for example
	//the original (wrapped) object can be reached by the .source message 
	
	proxyControlClass {
		^SynthDefControl
	}
	
	makeProxyControl { arg channelOffset=0;
		^this.proxyControlClass.new(this, channelOffset); 
	}
	
	
	//any preparations that have to be done to prepare the object
	//implement 'prepareForProxySynthDef' to return a ugen func
	
	
	//this method is called from within the Control
	buildForProxy { arg proxy, channelOffset=0, nameEnd;
		var argNames;
		argNames = this.argNames;
		^ProxySynthDef(
			proxy.generateUniqueName ++ nameEnd,
			this.prepareForProxySynthDef(proxy),
			proxy.nodeMap.lagsFor(argNames),
			nil, 
			true, 
			channelOffset,
			proxy.numChannels
		); 
	}
	prepareForProxySynthDef { ^this.subclassResponsibility(thisMethod) }
	
	argNames { ^nil }
	numChannels { ^nil } 
	clear { }
	//support for unop / binop proxy
	isNeutral { ^true }
	initBus { ^true }
	wakeUp {}
	
}

+Function {
	prepareForProxySynthDef { ^this }
}

+SequenceableCollection {
	prepareForProxySynthDef { arg proxy;
		^{ this.collect({ arg item; item.value(proxy) }) }
	}
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

+BusPlug {
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
}

//needs a visit: lazy init + channleOffset

+Bus {
	makeProxyControl { arg channelOffset=0;
		^BusPlug.for(this).makeProxyControl(channelOffset);
	}
}




///////////////////////////// SynthDefs and alike ////////////////////////////////////


+SynthDef {
	buildForProxy {}
	
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







///////////////////////////// Pattern - Streams ///////////////////////////////////


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

+SynthlessPlayer {
	
	proxyControlClass {
		^CXPlayerControl
	}
	
	wrapInFader {}
	
	
}

+AbstractPlayer {
	
	proxyControlClass {
			^CXSynthPlayerControl
	}
	
	prepareToPlayWithProxy { arg proxy;
		var tempBus, ok;
		tempBus = proxy.asBus;
		this.prepareForPlay(bus: tempBus); 
		//get the rate. there seems a bug in prepareForPlay: doesn't set the rate
		//[this.numChannels, tempBus].debug;
		ok = proxy.initBus(this.rate ? 'audio', this.numChannels ? 2); 
		tempBus.free;
		this.prepareForPlay(bus: proxy.asBus);
		
		^ok
	}
	
	makeProxyControl { arg channelOffset=0, proxy;
			this.prepareToPlayWithProxy(proxy); //do it here for now.
			^this.proxyControlClass.new(this.wrapInFader(proxy), channelOffset); 
	}
	
	wrapInFader { arg bus;
			var n;
			n = bus.numChannels;
			^EnvelopedPlayer(
				Patch({ arg input; NumChannels.ar(input, n, true) },[this]),
				Env.asr(0.5, 1, 0.5), 
				n
			);
			
	}
	
}

+Instr {
	
	makeProxyControl { arg channelOffset=0, proxy;
		^Patch(this).makeProxyControl(channelOffset, proxy)
	}
	
}


///////////  query whether some UGen can possibly free its synth from within


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
	canFreeSynth { ^inputs.at(3) > 1 }
}

+ SynthDef {
	canFreeSynth { ^children.canFreeSynth }
	
	hasGate {
		^if(controlNames.isNil, { 
			false 
		}, {
			controlNames.any({ arg name; name.name == "gate" })
			and:
			this.canFreeSynth
		})
	}
}



