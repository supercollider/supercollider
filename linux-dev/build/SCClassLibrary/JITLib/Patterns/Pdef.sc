Pdef : Pattern {
	
	var <key, <pattern;
	var <>last, <children;
	var <>clock;
	
	classvar <>patterns, <>numericals, <>lastEdited, <>clock;
	
	*new { arg key, pattern, argClock;
		var p;
	 	p = this.at(key);
		if(p.isNil, {
			p = this.make(key, pattern);
			this.put(key, p);
		}, {
			pattern = pattern ?? { Plib(\rest) };
			p.pattern_(pattern);
		});
		p.clock = argClock ? clock;
		p.changed;
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
		pattern = pat;
	}

	clear { children = IdentitySet.new  }	
	
	*at { arg key;
		^patterns.at(key);
	}
	*put { arg key, pattern;
		patterns.put(key, pattern);
	}
	
	*initClass {
		/*
		clock = QuantClock.new(1.0);
		patterns = IdentityDictionary.new; 
		numericals = IdentityDictionary.new; 
				
		//rest empty patterns//
		Pdef(\rest,
		  Pbind( 
			\freq, \rest, 
			\dur, 1//Pn(1,1)
		  )
		);
		
		//numerical Pdef not.
		//Pdefn(\rest, Pn(0, 1));
		*/
	}
	
	changed {
		this.sched({ children.do({ arg item; 
								item.pattern = pattern; 
								item.resetPat 
		}) });
	
	}
	
	backup {
		var lastPdef;
		lastPdef = this.class.at(key);
		lastEdited = key;
		if(lastPdef.notNil, { last = lastPdef });
	}
	
	sched { arg func;
		clock.sched(0, func);
	}
	
	asStream {
		^DefStream(this)
	
	}
	
	storeOn { arg stream;
		stream << "Pdef(";
		stream <<< key;
		stream << ", ";
		stream <<< pattern;
	
		stream << ")";
	}
	printOn { arg stream;
		stream << "Pdef(";
		stream <<< key;
		stream << ", ";
		stream << pattern;
		stream << ")";
	
	}
}
	