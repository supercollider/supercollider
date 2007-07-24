Psym : FilterPattern {
	var <>dict;
	
	*new { arg pattern, dict;
		^super.new(pattern).dict_(dict)
	}
	
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
			inval ?? { ^nil.yield };
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
			inval ?? { ^nil.yield };
		};
		^inval
	
	}
}

