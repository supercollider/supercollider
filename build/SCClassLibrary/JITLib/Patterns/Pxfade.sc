
PfadeIn : FilterPattern {
	var <>fadeTime, <>holdTime=0, <>tolerance;
	*new { arg pattern, fadeTime=1.0, holdTime=0.0, tolerance=0.0001;
		^super.new(pattern).fadeTime_(fadeTime).holdTime_(holdTime).tolerance_(tolerance)
	}
	embedInStream { arg inval;
			var outval, elapsed=0, stream, c;
			stream = pattern.asStream;
			
		
			if(holdTime > 0.0) { Event.silent(holdTime).yield };
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
}

PfadeOut : PfadeIn {
	embedInStream { arg inval;
			var outval, elapsed=0, stream, c;
			stream = pattern.asStream;
			loop {
				outval = stream.next(inval);
				if(outval.isNil) { ^inval };
				
				elapsed = elapsed + outval.delta;
				if(elapsed.round(tolerance) <= holdTime) {
					inval = outval.yield;
				} {
					c = elapsed - holdTime / fadeTime;
					if(c >= 1.0) {
						stream.next(nil);
						^nil.yield
					} {
						outval = outval.copy;
						outval[\amp] = (1.0 - c.max(0)) * outval[\amp];
						//outval[\amp] = (1.0 - (c.max(0) * pi * 0.25).sin) * outval[\amp];

						inval = outval.yield;
					}
				}
			}
	}
}

