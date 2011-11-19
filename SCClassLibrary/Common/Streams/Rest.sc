Rest {
	var <>dur = 0;
	*new { |dur(0)|
		^super.newCopyArgs(dur)
	}
	// for use by Pfunc and Pfuncn
	*processRest { |inval|
		inval.put(\type, \rest);
		^1
	}
	processRest { |inval|
		inval.put(\type, \rest);
		^dur
	}
	// for use e.g. in ListPatterns
	*embedInStream { |inval|
		^this.processRest(inval).yield;  // the actual return value is the next inval
	}
	embedInStream { |inval|
		^this.processRest(inval).yield;
	}
	*asStream {
		^Routine({ |inval|
			loop {
				inval = this.embedInStream(inval);
			}
		})
	}
	asStream {
		^Routine({ |inval|
			loop {
				inval = this.embedInStream(inval);
			}
		})
	}
}

+ Object {
	processRest { ^this }
}

+ Collection {
	processRest { |inval|
		this.do(_.processRest(inval))
	}
}
