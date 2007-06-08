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
			
			pat = if(outval.isSequenceableCollection) {
				this.parallelise(
					outval.collect {|key|
						this.lookUp(key.asSymbol)
					}
				);
			} {
				this.lookUp(outval.asSymbol)
			};
			
			inval = pat.embedInStream(inval);
		}
	
	}
	parallelise { arg pat; ^Ppar(pat) }
	
}

Pnsym : Psym {
	lookupClass { ^Pdefn }
	parallelise { arg pat; ^Ptuple(pat) }

}


Pinbox : PatternProxy {
	var <>name;
	*new { arg name, source, condition; // could also be: name, quant, condition
		^super.new.source_(source).name_(name).condition_(condition)
	}
	
	receiveEvent { arg inval;
		var news;
		if(inval.notNil) {	// maybe some other class: multiple keys, or even a pattern matching.
				news = inval.at(\news);
				if(news.notNil) {
					this.source = news.at(name) ? pattern
				}
		};
	}
	
}

Poutbox : PatternProxy {
	*new {
		^super.basicNew.init
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


