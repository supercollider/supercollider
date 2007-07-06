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
	var <>channel;
	
	*new { arg channel, quant, condition = true, source;
		^super.new.source_(source).channel_(channel).condition_(condition).quant_(quant)
	}
	
	receiveEvent { arg inval;
		var incoming, news = inval.eventAt(\news);
		if(news.isNil) { ^this };
		this.source = 
		if(channel.isSequenceableCollection) {
			this.class.parallelise(
				channel.collect { |each|
					news.at(each) ?? { this.class.default }
				}
			);
		} {
			news.at(channel) ?? { this.class.default } 
			// prev pattern could also be used. don't know.
		}
		
	}
	
	embedInStream { arg inval;
		if(channel.isKindOf(Pattern)) {
			channel.do { arg each;
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

// Pmail(\key, \channel, val, \channel, val...)

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

// Precv(\key, \channel, ... args)

// maybe we don't need such a thing for global access.
/*

Precv : Pinbox {
	classvar <>all;
	var <>key;
	
	*initClass { 
		all = IdentityDictionary.new;
	}
	*new { arg key, channel, quant, condition, source;
		var res = all.at(key);
		if(res.isNil) { res = super.new; all.put(key, res) };
		if(channel.notNil) { res.channel = channel };
		if(source.notNil) { res.source = source };
		if(condition.notNil) { res.condition = condition };
		if(quant.notNil) { 
			res.quant = if(quant < 0) { nil } { quant } // -1 overrides value.
		};
		^res
	}
}

*/