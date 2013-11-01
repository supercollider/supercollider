Psym : FilterPattern {
	var <>dict;

	*new { arg pattern, dict;
		^super.new(pattern).dict_(dict)
	}
	storeArgs { ^[pattern,dict] }

	lookupClass { ^Pdef }
	lookUp { arg key;
		^(dict ?? { this.lookupClass.all }).at(key) ?? { this.lookupClass.default }
	}

	embedInStream { arg inval;
		var str, outval, pat;
		str = pattern.asStream;
		while {
			outval = str.next(inval);
			outval.notNil
		} {

			pat = this.getPattern(outval);
			inval = pat.embedInStream(inval);
		};
		^inval

	}

	getPattern { arg key;
		^if(key.isSequenceableCollection) {
			this.lookupClass.parallelise(
				key.collect {|each|
					this.lookUp(each.asSymbol)
				}
			);
		} {
			this.lookUp(key.asSymbol)
		};
	}



}

Pnsym : Psym {
	lookupClass { ^Pdefn }
}


Ptsym : Psym {
	var <>quant, <>dur, <>tolerance;

	*new { arg pattern, dict, quant, dur, tolerance = 0.001;
		^super.newCopyArgs(pattern, dict, quant, dur, tolerance)
	}
	storeArgs { ^[ pattern, dict, quant, dur, tolerance ] }
	embedInStream { arg inval;
		var str, outval, pat, quantVal, quantStr, durVal, durStr;
		str = pattern.asStream;
		quantStr = quant.asStream;
		durStr = dur.asStream;

		while {
			outval = str.next(inval);
			quantVal = quantStr.next(inval) ? quantVal;
			durVal = durStr.next(inval) ? durVal;
			outval.notNil
		} {
			pat = Psync(this.getPattern(outval), quantVal, durVal, tolerance);
			inval = pat.embedInStream(inval);
		};
		^inval

	}
}

Pnsym1 : Pnsym {
	embedInStream { arg inval;
		var str, which, streams, outval, pat, currentStream;
		str = pattern.asStream;
		streams = IdentityDictionary.new;
		while {
			which = str.next(inval);
			which.notNil
		} {
			pat = this.getPattern(which);
			currentStream = streams.at(pat);
			if(currentStream.isNil) {
				currentStream = pat.asStream;
				streams.put(pat, currentStream);
			};
			outval = currentStream.next(inval);
			outval ?? { ^inval };
			inval = outval.yield
		};
		^inval

	}
}

Psym1 : Psym {
	embedInStream { arg inval, cleanup;
		var str, which, streams, outval, pat, currentStream;
		str = pattern.asStream;
		streams = IdentityDictionary.new;
		cleanup ?? { cleanup = EventStreamCleanup.new };

		while {
			which = str.next(inval);
			which.notNil
		} {
			pat = this.getPattern(which);
			currentStream = streams.at(pat);
			if(currentStream.isNil) {
				currentStream = pat.asStream;
				streams.put(pat, currentStream);
			};
			outval = currentStream.next(inval);
			if(outval.isNil) { ^cleanup.exit(inval) };

			cleanup.update(outval);
			inval = outval.yield
		};

		^cleanup.exit(inval);
	}
}
