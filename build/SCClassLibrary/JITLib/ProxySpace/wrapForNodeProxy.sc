

+Object { 
	
	proxyControlClass {
		^SynthDefControl
	}
	
	makeProxyControl { arg channelOffset=0;
		^this.proxyControlClass.new(this, channelOffset); 
	}
	
	//assumes some GraphBuilder
	prepareForProxySynthDef {
		^this
	}
	
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
	putExtend { arg index, item;
		var res;
		res = this;
		^if(index.isNil, {
			this.add(item)
		}, {
			if(index >= this.size, {
				res = this.extend(index + 1);
			});
			res.put(index, item)
		});
	
	}
}

+ProxySynthDef {
	hasGate {
		^true
	}
}

+SynthDef {
	wrapForNodeProxy {
		^this
	}
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


+AbstractPlayer {
	
	wrapForNodeProxy {}
	proxyControlClass {
		^CXPlayerControl
	}

}


+SimpleNumber {
	prepareForProxySynthDef { arg proxy;
		^if(proxy.rate === 'audio', {
			{ Array.fill(proxy.numChannels, { K2A.ar(this) }) }
		}, { 
			{ Array.fill(proxy.numChannels, { Line.kr(this,this,0.001) }) }
		})
	}
}

+Instr {
	prepareForProxySynthDef {
		^func.prepareForProxySynthDef;
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
		//assume audio rate event stream for now.
		var pat, ok, argNames, msgFunc;
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
			
			pat = this.collect({ arg event;
				event.use({ 
					~nodeID = proxy.group.asNodeID; //in case of Pmono. see.
					~argNames = argNames;
					~msgFunc = msgFunc;
				});
				event;
			});
			pat.asEventStream
		}, nil);

	}

}


// add  Stream .wrapForNodeProxy
// this assumes initBus is done in Pattern-wrapForNodeProxy.

+Stream {

	wrapForNodeProxy {}
	
	proxyControlClass {
		^StreamControl
	}
}




