//these need some redesign. maybe crucial players

+Object {
	asPFunction {
		^{ this }
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

+AudioProxy {
	asPFunction { 
		^{this.value}
	}
}

+Instr {
	asPFunction { 
		^this.func
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

