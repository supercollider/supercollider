//these need some redesign. maybe crucial players

+Object { 
	//assumes some GraphBuilder
	prepareForProxySynthDef {
		^this
	}
	asProxySynthDef { arg proxy;
		^ProxySynthDef(proxy, this.prepareForProxySynthDef); 
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

+ ProxySynthDef {
	asProxySynthDef { arg proxy;
		^this; 
	}
}

+ SynthDef {
	asProxySynthDef { 
		"cannot use a normal synthDef as input".inform;
		^nil; 
	}
}


+ Pattern {
	prepareForProxySynthDef { arg proxy;
		var eventStream;
		eventStream = Pevent(this, 
			Event.protoEvent.put(\bus, proxy.outbus)
		);
			eventStream.play;
		^0.0
	}
}



