

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

+ProxySynthDef {
	hasGate {
		^true
	}
}


+SynthDef {
	buildForProxy {}
	
	hasGate {
		^if(controlNames.isNil, { 
			false 
		}, {
			controlNames.any({ arg name; name.name == "gate" })
		})
	}
	
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
			{ Array.fill(proxy.numChannels, { Line.kr(this,this,0.001) }) }
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
		^this.asEventStreamPlayer.buildForProxy(proxy, channelOffset)
	}
}


+Stream {

	buildForProxy { arg proxy, channelOffset=0;
		//assume audio rate event stream for now.
		var str, ok, argNames, msgFunc;
		ok = proxy.initBus('audio', 2);
		
		argNames = [\freq,\amp,\sustain,\pan];//more later
		msgFunc = { arg id, freq;
				var args, bundle;
				if(~argNames.notNil, {
					args = Array.newClear(~argNames.size * 2);
					~argNames.do({ arg name, i; 
						args.put(i*2, name); 
						args.put(i*2+1, currentEnvironment.at(name))
					});
				});
				bundle = List[[9, ~instrument, id, 1, ~group] ++ args];
				proxy.nodeMap.addToBundle(bundle, id);
				bundle
		};

		
		^if(ok, {
			//bus = proxy.index; //done in bundle already
			
			this.collect({ arg event;
				event.use({ 
					~nodeID = proxy.group.asNodeID; //in case of Pmono. see.
					~argNames = argNames;
					~msgFunc = msgFunc;
				});
				event;
			}); 
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

+EventStreamPlayer {
	collect { arg func;
		^this.class.new(originalStream.collect(func), event)
	}
}

///////////////// cx players ////////////

+AbstractPlayer {
	
	proxyControlClass {
		^CXPlayerControl
	}
	
}

+Patch {
	
	makeProxyControl { arg channelOffset=0, proxy;
			
			^this.proxyControlClass.new(this.wrapInFader(proxy), channelOffset); 
	}
	
	wrapInFader { arg proxy;
			^Patch({ arg input;
						var synthGate, synthFadeTime;
						synthGate = Control.names('#gate').kr(1.0);
						synthFadeTime = Control.names('#fadeTime').kr(0.02);
						input 
						 *
						 Array.fill((proxy.numChannels ? 1), 1)
						 *
						 EnvGen.kr(
							Env.new(#[0,1,0],[1,1.25],'sin',1),
							synthGate,1,0,synthFadeTime,2
						)
			}, [this]);
			
	}
	
	
}



