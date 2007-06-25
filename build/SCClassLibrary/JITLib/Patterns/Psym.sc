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
		};
		nil.yield;
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
		};
		nil.yield;
		^inval
	
	}
}


Pinbox : EventPatternProxy {
	var <>name;
	
	*new { arg name, quant, condition = true, source;
		^super.new.source_(source).name_(name).condition_(condition).quant_(quant)
	}
	
	receiveEvent { arg inval;
		var incoming, news = inval.eventAt(\news);
		if(news.isNil) { ^this };
		this.source = 
		if(name.isSequenceableCollection) {
			this.class.parallelise(
				name.collect { |each|
					news.at(each) ? pattern
				}
			);
		} {
			news.at(name) ? pattern;
		}
		
	}
	
	embedInStream { arg inval;
		if(name.isKindOf(Pattern)) {
			name.do { arg each;
				inval = Pinbox(each, quant, condition, source).embedInStream(inval)
			};
		} {
			inval = super.embedInStream(inval);
		};
		if(inval.isNil) { nil.yield };
		^inval;
	}

	
}

Poutbox : EventPatternProxy {
	*new {
		^super.new.init
	}
	init {
		this.envir = this.class.event;
		this.source_(Pfunc { |inval| composeEvents(inval, envir) })
	}
	getNews {
		var ev = super.get(\news);
		if(ev.isNil) { super.set(\news, ev = ()) };
		^ev
	}
	set { arg ... args;
		this.getNews.putPairs(args)
	}
	get { arg ... args;
		this.getNews.getPairs(args)
	}
}


Pmail : Poutbox {
	classvar <>all;
	var <>key;
	
	*initClass { 
		all = IdentityDictionary.new;
	}
	*new { arg key ... args;
		var res = all.at(key);
		if(res.isNil) { res = super.new; all.put(key, res) };
		^res.set(*args);
	}

}
