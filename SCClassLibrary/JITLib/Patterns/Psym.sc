Psym : FilterPattern {
	var <>dict;

	*new { |pattern, dict|
		^super.new(pattern).dict_(dict)
	}

	storeArgs { ^[pattern, dict] }

	lookupClass { ^Pdef }

	lookUp { |key|
		^(dict ?? { this.lookupClass.all }).at(key.asSymbol) ?? { this.lookupClass.default }
	}

	embedInStream { |inval|
		var str, outval, pat;
		str = pattern.asStream;
		while {
			outval = str.next(inval);
			outval.notNil
		} {
			pat = this.getPattern(outval);
			inval = pat.embedInStream(inval)
		};
		^inval

	}

	getPattern { |key|
		^if(key.isSequenceableCollection) {
			this.lookupClass.parallelise(
				key.collect { |each|
					this.lookUp(each)
				}
			)
		} {
			this.lookUp(key)
		}
	}

}

Pnsym : Psym {
	lookupClass { ^Pdefn }
}

Ptsym : Psym {
	var <>quant, <>dur, <>tolerance;

	*new { |pattern, dict, quant, dur, tolerance = 0.001|
		^super.newCopyArgs(pattern, dict, quant, dur, tolerance)
	}

	storeArgs { ^[pattern, dict, quant, dur, tolerance] }

	embedInStream { |inval|
		var outval, pat, quantVal, durVal;
		var str = pattern.asStream;
		var quantStr = quant.asStream;
		var durStr = dur.asStream;

		while {
			outval = str.next(inval);
			quantVal = quantStr.next(inval) ? quantVal;
			durVal = durStr.next(inval) ? durVal;
			outval.notNil
		} {
			pat = Psync(this.getPattern(outval), quantVal, durVal, tolerance);
			inval = pat.embedInStream(inval)
		};
		^inval

	}
}

Pnsym1 : Pnsym {

	embedInStream { |inval|

		var which, outval, pat, currentStream;
		var str = pattern.asStream;
		var streams = IdentityDictionary.new;

		while {
			which = str.next(inval);
			which.notNil
		} {
			pat = this.getPattern(which);
			currentStream = streams.at(pat);
			if(currentStream.isNil) {
				currentStream = pat.asStream;
				streams.put(pat, currentStream)
			};
			outval = currentStream.next(inval);
			outval ?? { ^inval };
			inval = outval.yield
		};
		^inval

	}
}

Psym1 : Psym {

	embedInStream { |inval|

		var which, outval, pat, currentStream;
		var str = pattern.asStream;
		var streams = IdentityDictionary.new;
		var cleanup = EventStreamCleanup.new;

		while {
			which = str.next(inval);
			which.notNil
		} {
			if(which.isSequenceableCollection) {
				Error("Psym1 cannot embed arrayed keys: %\n".format(which)).throw
			};
			pat = this.lookUp(which);
			currentStream = streams.at(pat);
			if(currentStream.isNil) {
				currentStream = pat.asStream;
				streams.put(pat, currentStream)
			};
			outval = currentStream.next(inval);
			if(outval.isNil) { ^cleanup.exit(inval) };

			cleanup.update(outval);
			inval = outval.yield
		};

		^cleanup.exit(inval)
	}
}
