

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
	
	wrapForNodeProxy { arg proxy, channelOffset=0, index=0;
		^ProxySynthDef(
			proxy.generateUniqueName ++ index, 
			this.prepareForProxySynthDef(proxy),
			proxy.lags,
			proxy.prepend, 
			true, 
			channelOffset
		); 
	}
	
	numChannels { ^nil } 
	
	//move elsewhere later
	findKey { 
		^currentEnvironment.findKeyForValue(this)
	}
	
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
	wrapForNodeProxy {}
	
	hasGate {
		^if(controlNames.isNil, { 
			false 
		}, {
			controlNames.any({ arg name; name.name == "gate" })
		})
	}
	
}

+SoundDef {
	wrapForNodeProxy {
		^synthDef
	}
	proxyControlClass {
		^SoundDefControl
	}

}

+Symbol {
	wrapForNodeProxy {}
	proxyControlClass {
		^SynthControl
	}
}


+AbstractPlayer {
	
	wrapForNodeProxy {}
	proxyControlClass {
		^CXPlayerControl
	}

}

//use patch later
+Instr {
	prepareForProxySynthDef {
		^func.prepareForProxySynthDef;
	}
	
	//makeProxyControl { arg channelOffset=0;
	//	^Patch(this.name).makeProxyControl(channelOffset)
	//}
	
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

+BusPlug {
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
}



+Pattern {
	proxyControlClass {
		^StreamControl
	}
	wrapForNodeProxy { arg proxy, channelOffset=0;
		^this.asEventStream.wrapForNodeProxy(proxy, channelOffset)
	}
}


+Stream {

	//wrapForNodeProxy {}
	wrapForNodeProxy { arg proxy, channelOffset=0;
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
	wrapForNodeProxy { arg proxy, channelOffset=0;
		^EventStream(this.asStream).wrapForNodeProxy(proxy, channelOffset);
	}

}
+Task {
	wrapForNodeProxy {}

}


+PauseStream {
	collect { arg func;
		^this.class.new(originalStream.collect(func), clock)
	}
}

+EventStream {
	collect { arg func;
		^this.class.new(originalStream.collect(func), event)
	}
}


