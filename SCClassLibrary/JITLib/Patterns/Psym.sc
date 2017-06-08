Psym : FilterPattern {
	var <>dict;

	*new { arg pattern, dict;
		^super.new(pattern).dict_(dict)
	}

	storeArgs { ^[pattern,dict] }

	lookupClass { ^Pdef }

	lookUp { arg key;
		^(dict ?? { this.lookupClass.all }).at(key.asSymbol) ?? { this.lookupClass.default }
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
					this.lookUp(each)
				}
			);
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

	*new { arg pattern, dict, quant, dur, tolerance = 0.001;
		^super.newCopyArgs(pattern, dict, quant, dur, tolerance)
	}

	storeArgs { ^[ pattern, dict, quant, dur, tolerance ] }

	embedInStream { arg inval;
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
			inval = pat.embedInStream(inval);
		};
		^inval

	}
}

Pnsym1 : Pnsym {

	embedInStream { arg inval;

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

	embedInStream { arg inval;

		var which, outval, last;
		var str = pattern.asStream;
		var streams = IdentityDictionary.new;
		var cleanup = EventStreamCleanup.new;

		var embedKeyInStream = { |key, parallel = false|
			var currentStream = streams.at(key);
			var outval;
			if(currentStream.isNil) {
				currentStream = this.lookUp(key).asStream;
				streams.put(key, currentStream);
			};
			outval = currentStream.next(inval);
			if(outval.isNil) { ^cleanup.exit(inval) };
			if(parallel) { outval.put(\delta, 0) };
			cleanup.update(outval);
			inval = outval.yield
		};

		while {
			which = str.next(inval);
			which.notNil
		} {
			if(which.isSequenceableCollection) {
				last = which.lastIndex;
				which.do { |key, i| embedKeyInStream.value(key, i < last) }
			} {
				embedKeyInStream.value(which)
			}
		};

		^cleanup.exit(inval);
	}


}
