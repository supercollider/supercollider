PfadeIn : FilterPattern {
	var <>fadeTime, <>holdTime=0, <>tolerance;
	*new { arg pattern, fadeTime=1.0, holdTime=0.0, tolerance=0.0001;
		^super.new(pattern).fadeTime_(fadeTime).holdTime_(holdTime).tolerance_(tolerance)
	}
	embedInStream { arg inval;
			var outval, elapsed=0, stream, c;
			stream = pattern.asStream;


			if(holdTime > 0.0) { Event.silent(holdTime, inval).yield };
			loop {
				outval = stream.next(inval);
				if(outval.isNil) { ^nil.yield };

				elapsed = elapsed + outval.delta;
				c = elapsed / fadeTime;
				if(c >= 1.0) {
					inval = outval.yield;
					^stream.embedInStream(inval);
				} {
					outval = outval.copy;
					outval[\amp] = c.max(0) * outval[\amp];
				//	outval[\amp] = (c.max(0) * pi * 0.25).sin * outval[\amp];

					inval = outval.yield;
				}
			}
	}
	storeArgs { ^[ pattern, fadeTime, holdTime, tolerance ] }
}

PfadeOut : PfadeIn {
	asStream { | cleanup| ^Routine({ arg inval; this.embedInStream(inval, cleanup) }) }

	embedInStream { arg inval, cleanup;
		var outval, elapsed=0, stream, c;
		stream = pattern.asStream;
		cleanup ?? { cleanup = EventStreamCleanup.new };
		loop {
			inval = stream.next(inval) ?? { ^cleanup.exit(inval) };
			cleanup.update(inval);

			elapsed = elapsed + inval.delta;
			if(elapsed.round(tolerance) <= holdTime) {
				inval = inval.yield;
			} {
				c = elapsed - holdTime / fadeTime;
				if(c >= 1.0) {
					^cleanup.exit(inval);
				} {
					inval[\amp] = (1.0 - c.max(0)) * inval[\amp];

					inval = inval.yield;
				}
			}
		}
	}
}

PfinQuant : FilterPattern {
	var <>quant, <>clock;

	*new { arg pattern, quant, clock;
		^super.new(pattern).quant_(quant).clock_(clock)
	}

	embedInStream { arg inval;
		var value, stream = pattern.asStream;
		var referenceClock = clock ? thisThread.clock;
		var endAt = quant.nextTimeOnGrid(referenceClock);
		while {
			value = stream.next(inval);
			value.notNil and: { referenceClock.beats < endAt }
		} {
			inval = value.yield;
		};
		^inval
	}


}
