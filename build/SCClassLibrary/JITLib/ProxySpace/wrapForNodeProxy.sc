//these need some redesign. maybe crucial players

+Object { 
	//assumes some GraphBuilder
	prepareForProxySynthDef {
		^this
	}
	asProxySynthDef { arg proxy, channelOffset=0;
		^ProxySynthDef(
			proxy.generateUniqueName, 
			this.prepareForProxySynthDef(proxy),
			proxy.lags, 
			proxy.prepend, 
			proxy.freeSelf, 
			channelOffset
		); 
	}	
	wrapForNodeProxy { arg proxy, lags, channelOffset=0;
		var synthDef, ok;
		synthDef = this.asProxySynthDef(proxy, lags, channelOffset);
		ok = proxy.initBus(synthDef.rate, synthDef.numChannels);
		^if(ok && synthDef.notNil, {
			SynthDefControl.new(synthDef)
		}, nil);
	
	}
}

+SynthDef {
	wrapForNodeProxy { arg proxy, channelOffset=0;
		//we don't know how many channels it has, user's responsibility
		^SynthDefControl.new(this, false) //assumes there is no gate in the def.
	}

}

+AbstractPlayer {
	 
	wrapForNodeProxy { arg proxy, channelOffset=0;
		var synthDef, ok;
		this.prepareForPlay(proxy.group.asGroup, true, proxy.outbus);
		synthDef = this.asSynthDef;
		ok = proxy.initBus(this.rate,this.numChannels);
		^if(ok && synthDef.notNil, {
				//AbstractPlayerControl.new(synthDef)
				SynthDefControl(synthDef, false)
		}, nil);
	}

}


+SimpleNumber {
	prepareForProxySynthDef { arg proxy;
		^if(proxy.rate === 'audio', {
			{ Array.fill(proxy.numChannels, { Line.ar(this,this,0.001) }) }
		}, { 
			{ Array.fill(proxy.numChannels, { Line.kr(this,this,0.001) }) }
		})
	}
//problem here: when there is a fade, the bus is still written on after the setting.
//	wrapForNodeProxy { arg proxy, lags, channelOffset=0;
//		var ok;
//		ok = proxy.initBus('control',1);
//		^if(ok, { NumericalControl(proxy.outbus, this) }, nil)
//	}

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

+NodeProxy {
	prepareForProxySynthDef { arg proxy;
		^{ this.value(proxy) }
	}
}

+SoundDef {
	prepareForProxySynthDef {
		^obj
	}
}

+Pattern {
	wrapForNodeProxy { arg proxy, channelOffset=0;
		//assume audio rate event stream for now.
		var pat, ok, outbus, argNames, msgFunc;
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
			outbus = proxy.index; //doesn't change
			
			pat = this.collect({ arg event;
				event.use({ 
					~nodeID = proxy.asNodeID; //in case of Pmono. see.
					~argNames = argNames;
					~msgFunc = msgFunc;
				});
				event;
			});
			pat.asEventStream.wrapForNodeProxy
		}, nil);

	}

}


// add  Stream .wrapForNodeProxy
// this assumes initBus is done in Pattern-wrapForNodeProxy.

+EventStream {
	wrapForNodeProxy { 
			^EventStreamControl.new(this)
		
	}
}




/*
//for now:
+Patch {
	defArgs {
		//needs prepare play!
		//^this.synthDefArgs
		^instr.defArgs
	}
	argNames {
		^instr.argNames
	}

}


+InstrE {
	prepareForProxySynthDef { 
		^{var c, defArgs, argNames;
			defArgs = this.defArgs;
			argNames = this.argNames;
			argNames.do({ arg key, i;
				var val;
				val = envir.at(key);
				if(val.notNil, {
					defArgs.put(i, val)
				});
			});
			c = Control.names(this.argNames).kr(defArgs);
			func.valueArray(c)
		}
	}
}
*/





