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
			proxy.freeSelf, 
			channelOffset
		); 
	}	
	wrapForNodeProxy { arg proxy, channelOffset=0;
		var synthDef, ok;
		synthDef = this.asProxySynthDef(proxy, channelOffset);
		ok = proxy.initBus(synthDef.rate, synthDef.numChannels);
		^if(ok && synthDef.notNil, {
			SynthDefControl.new(synthDef)
		}, nil);
	
	}
}

+AbstractPlayer {
	 
	wrapForNodeProxy { arg proxy, channelOffset=0;
		var synthDef, ok;
		synthDef = this.asSynthDef;
		ok = proxy.initBus(this.rate,this.numChannels);
		^if(ok && synthDef.notNil, {
				AbstractPlayerControl.new(synthDef)
		}, nil);
	}

}


+SimpleNumber {
	prepareForProxySynthDef { arg proxy;
		^if(proxy.rate === 'audio', {
			{Line.ar(this,this,0.001)}
		}, { 
			{Line.kr(this,this,0.001)}
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
		^this.asStream.wrapForNodeProxy(proxy, channelOffset);
	}

}

//maybe add Pevent.wrapForNodeProxy

+Stream {
	wrapForNodeProxy { arg proxy,channelOffset=0;
		//assume audio rate stream?
		var ok;
		ok = proxy.initBus('audio',2);
		^if(ok, {
			EventStreamControl.new(this.collect({ arg event;
				event.copy.use({ 
					~out = proxy.outbus.index + channelOffset;
					~nodeMap = proxy.nodeMap;
					~argNames = [\freq,\amp,\sustain,\pan,\out];//more later
			})
		
			}))
		}, nil);
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





