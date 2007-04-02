Psym : FilterPattern {
	var <>dict;
	
	lookupClass { ^Pdef }
	
	embedInStream { arg inval;
		var str, outval, pat;
		str = pattern.asStream;
		while {
			outval = str.next(inval);
			outval.notNil
		} {
			inval = (
				(dict ??  { this.lookupClass.all }).at(outval.asSymbol) ?? { Pdef.default }
			).embedInStream(inval);
		}
	
	}
}

Pnsym : Psym {
	lookupClass { ^Pdefn }
}


Pinbox : PatternProxy {
	var <>name;
	*new { arg name, source; // some other condition?
		^super.new.source_(source).name_(name)
	}
	
	receiveEvent { arg inval;
		if(inval.notNil) {	// multiple keys, or even a pattern matching.
				if(inval.at(\news).notNil) {
					this.source = inval.at(\news).at(name) ? pattern
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


