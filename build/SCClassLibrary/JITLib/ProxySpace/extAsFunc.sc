//these need some redesign. maybe crucial players

+Object { 
	//assumes some GraphBuilder
	prepareForProxySynthDef {
		^this
	}
	asProxySynthDef { arg proxy, channelOffset=0;
		^ProxySynthDef(proxy, this.prepareForProxySynthDef(proxy), channelOffset); 
	}	
	wrapForNodeProxy { arg proxy, channelOffset=0;
		var synthDef;
		synthDef = this.asProxySynthDef(proxy, channelOffset);
		^if(synthDef.notNil, {
			SynthDefContainer.new(synthDef)
		}, nil);
	}
}


+SimpleNumber {
	prepareForProxySynthDef {
		^{ Control.names([\value]).kr([this])  }
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
	prepareForProxySynthDef {
		^{ this.value }
	}
}

+Pattern {
	wrapForNodeProxy { arg proxy, channelOffset=0;
		^this.asStream.wrapForNodeProxy(proxy, channelOffset);
	}

}

+Stream {
	wrapForNodeProxy { arg proxy,channelOffset=0;
		^EventStreamContainer.new(this.collect({ arg event;
			event.copy.use({ 
				~outIndex = proxy.outbus.index + channelOffset;
				~nodeMap = proxy.nodeMap;
				~argNames = [\freq,\amp,\sustain,\pan,\outIndex];//more later
				//~group = proxy.group.nodeID;
			})
		
		});)
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
*/

/*
+AbstractFunction {
	prepareForProxySynthDef { 
		^{this.value}
	}
}

+Instr {
	prepareForProxySynthDef { 
		^this.func
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





