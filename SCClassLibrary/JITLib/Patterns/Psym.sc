Psym : FilterPattern {
	var <>dict;

	*new { |pattern, dict|
		^super.new(pattern).dict_(dict)
	}

	embedInStream { |inval|
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

	getPattern { |key|
		^if(key.isSequenceableCollection) {
			this.lookupClass.parallelise(
				key.collect {|each|
					this.lookUp(each)
				}
			)
		} {
			this.lookUp(key)
		}
	}

	lookupClass { ^Pdef }

	lookUp { |key|
		^(dict ?? { this.lookupClass.all }).at(key.asSymbol) ?? { this.lookupClass.default }
	}

	storeArgs {
		^[pattern, dict]
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

	storeArgs {
		^[pattern, dict, quant, dur, tolerance]
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
				Error("Psym1 cannot embed arrayed keys: %\n".format(which)).throw;
			};
			pat = this.lookUp(which);
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



// general purpose lookup stream

Pdict : Pattern {

	var <>dict, <>which, <>repeats, <>default;

	*new { |dict, which, repeats=inf, default|
		^super.newCopyArgs(dict, which, repeats, default)
	}

	embedInStream { |inval|
		var keyStream = which.asStream;
		var key;
		repeats.value(inval).do {
			key = keyStream.next(inval);
			if(key.isNil) { ^inval };
			inval = (dict.at(key) ? default).embedInStream(inval);
		};
		^inval
	}

	storeArgs {
		^[dict, which, repeats, default]
	}

}
