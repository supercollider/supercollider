//these need some redesign. maybe crucial players

+Object { 
	//assumes some GraphBuilder
	asPFunction {
		^{ this }
	}
	asProxySynthDef { arg proxy;
		^ProxySynthDef(proxy, this.asPFunction); 
	}
	
}

+SimpleNumber {
	asPFunction {
		^{ Control.names([\value]).kr([this])  }
	}
}


+Function {
	asPFunction {
		^this
	}
}
+NodeProxy {
	asPFunction { 
		^{this.value}
	}
}
+Instr {
	asPFunction { 
		^this.func
	}
}
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


/*
+Pattern {
	asPFunction { arg proxy;
		var eventStream;
		eventStream = Pevent(this, Event.protoEvent.use({
						~bus = proxy.bus;
		});
		eventStream.play;
		nil
	}

}
*/

