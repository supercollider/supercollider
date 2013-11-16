Rest {
	var <>dur = 1;
	*new { |dur(1)|
		^super.newCopyArgs(dur)
	}
	// for use by Pfunc and Pfuncn
	*processRest { |inval|
		inval.put(\isRest, true);
		^1
	}
	processRest { |inval|
		inval.put(\isRest, true);
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

	storeOn { |stream| stream << "Rest(" << dur << ")" }
}

+ Object {
	processRest { ^this }
}

+ Collection {
	processRest { |inval|
		this.do(_.processRest(inval))
	}
}


+ SimpleNumber {
	// Some patterns call .delta on the eventstream's yield value
	// since a SimpleNumber is a valid rest, it must answer 'delta' with itself
	delta {}
	// but Ppar and several other patterns do "put(\delta, ...)"
	// so they need to convert the simplenumber into a real rest event
	asEvent { ^Event.silent(this) }
}

+ Event {
	asEvent {}
}

+ Nil {
	// Ppar etc. need stream.next(event).asEvent to be nil when the stream ends
	asEvent {}
}
