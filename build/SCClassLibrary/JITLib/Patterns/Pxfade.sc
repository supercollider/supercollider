
PfadeIn : FilterPattern {
	var <>fadeTime, <>holdTime=0, <>tolerance;
	*new { arg pattern, fadeTime=1.0, holdTime=0.0, tolerance=0.0001;
		^super.new(pattern).fadeTime_(fadeTime).holdTime_(holdTime).tolerance_(tolerance)
	}
	asStream {
		^Routine.new { arg inval;
			var outval, elapsed=0, stream, c;
			stream = pattern.asStream;
			
		
			if(holdTime > 0.0) { Event.silent(holdTime).yield };
			loop {
				outval = stream.next(inval);
				if(outval.isNil) { nil.alwaysYield };
				
				elapsed = elapsed + outval.delta;
				c = elapsed / fadeTime;
				if(c >= 1.0) {
					stream.embedInStream(inval);
					nil.alwaysYield;
				
				} {
					outval[\amp] = c.max(0) * outval[\amp];
					inval = outval.yield;
				}
			}
		}
	}
}

PfadeOut : PfadeIn {
	asStream {
		^Routine.new { arg inval;
			var outval, elapsed=0, stream, c;
			stream = pattern.asStream;
			loop {
				outval = stream.next(inval);
				if(outval.isNil) { nil.alwaysYield };
				
				elapsed = elapsed + outval.delta;
				if(elapsed.round(tolerance) <= holdTime) {
					inval = outval.yield;
				} {
					c = elapsed - holdTime / fadeTime;
					if(c >= 1.0) { 
						nil.alwaysYield 
					} {
						outval[\amp] = (1.0 - c) * outval[\amp];
						inval = outval.yield;
					}
				}
			}
		}	
	}
}

