Pdefn : Pattern {
	var <key, <pattern;
	var <>last, <children, updated=false;
	
	classvar <>patterns, <>numericals, <>lastEdited, <>clock;
	
	*initClass { 
		patterns = IdentityDictionary.new; 
		numericals = IdentityDictionary.new; 
		clock = QuantClock.new(1.0);
		patterns.put(\rest,  
			Pdef.make(\rest, Pbind(nil,
				\freq, \rest, 
				\dur, 1
		 	 )));
		numericals.put(\rest,  Pdefn.make(\rest, Pn(0, 1)));
	
	}
	
	
	*new { arg key, pattern;
		var p;
	 	p = this.at(key);
	 	if(pattern.notNil, {
			if(p.isNil, {
				p = this.make(key, pattern);
				this.put(key, p);
			}, {
				if(pattern.isNil, { pattern = this.at(\rest) });
				p.pattern_(pattern);
			});
				p.changed;
		}, { p = p ? this.at(\rest) })
		^p
	
	}
		
	*make { arg key, pattern;
		^if(pattern.notNil, {
			super.newCopyArgs(key,pattern).clear;
		}, { 
			"this is no pattern".postln; 
			this.at(\rest) 
		})
	}
	
	pattern_ { arg pat;
		this.backup;
		updated = false;
		pattern = pat;
	}

	clear { children = IdentitySet.new  }	
	
	clock { ^nil }
	
	backup {
		var lastPdef;
		lastPdef = this.class.at(key);
		lastEdited = key;
		if(lastPdef.notNil, { last = lastPdef });
	}
	
	changed {
			if(updated.not, {
		 		children.do({ arg item; 
								item.pattern = pattern; 
								item.resetPat 
				});
				updated = true;
			});
	}
	
	asStream {
		^DefStream(this)
	
	}

	*at { arg key;
		^numericals.at(key);
	}
	*put { arg key, pattern;
		numericals.put(key, pattern);
	}
	
	storeArgs { ^[key,pattern] }
	
}


Pdef : Pdefn {
	
	
	var <>clock;
	
	*new { arg key, pattern, argClock;
		var p;
		p = super.new(key, pattern);
	 	p.clock = argClock ? clock;
		^p
	}
	
	*at { arg key;
		^patterns.at(key);
	}
	*put { arg key, pattern;
		patterns.put(key, pattern);
	}
	
		
	changed {
		this.sched({ children.do({ arg item; 
								item.pattern = pattern; 
								item.resetPat 
		}) });
	
	}
	
	sched { arg func;
		if(clock.isNil, func, {
			clock.sched(0, func);
		})
	}
	
	storeArgs { ^[key,pattern,clock] }
	
}

