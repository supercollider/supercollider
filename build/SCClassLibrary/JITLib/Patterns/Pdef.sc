

//pattern reference for numerical (non-event) streams

Pdefn : Pattern {
	var <key, <pattern;
	var <children, updated=false;
	
	classvar <>patterns, <>numericals;
	
	
	*initClass { 
		patterns = IdentityDictionary.new; 
		numericals = IdentityDictionary.new; 
	}
	
	*default { ^Pn(0, inf) }
	
	
	*new { arg key, pattern;
		var p;
		if(pattern.isKindOf(this), { "cannot allow this".inform; ^nil });
	 	p = this.at(key);
	 	if(pattern.notNil, {
			if(p.isNil, {
				p = this.make(key, pattern);
				this.put(key, p);
			}, {
				p.pattern_(pattern);
			});
			
		}, { 
			if(p.isNil, {
				p = this.make(key, this.default);
				this.put(key, p);
			});
		})
		^p
	
	}
		
	*make { arg key, pattern;
		^super.newCopyArgs(key,pattern).clear;
	}
	
	asStream {
		^DefStream(this).defaultValue_(0);
	
	}
	
	pattern_ { arg pat;
		pattern = pat;
		this.changed;
	}

	clear { children = IdentitySet.new  }	
	
	update {
		children.do({ arg item; item.update });
	}
	
	changed { this.update }
	
//	undo { //crashes
//		var pat;
//		pat = pattern;
//		pattern = last;
//		last = pat;
//		this.changed;
//	}
////	
//	backup {
//		var lastPdef;
//		lastPdef = this.class.at(key);
//		lastEdited = key;
//		if(lastPdef.notNil, { last = lastPdef });
//	}
	
	*at { arg key;
		^numericals.at(key);
	}
	*put { arg key, pattern;
		numericals.put(key, pattern);
	}
	
	storeArgs { ^[key,pattern] }
	
}

//pattern reference for event streams

Pdef : Pdefn {
	classvar <>playStreams;
	
	
	
	*at { arg key;
		^patterns.at(key);
	}
	*put { arg key, pattern;
		patterns.put(key, pattern);
	}
	
	*default { 
				^Pbind(nil,
					\freq, \rest, 
					\dur, 1
		 	 	)
	}
	
	//remove playing streams from me
	
	*initClass {
		CmdPeriod.add(this)
	}
	
	*cmdPeriod { this.clear }
	
	*clear {
		playStreams.do({ arg item;
			item.freeChain;
			item.releaseDependant;
		});
		playStreams = nil;
	}
	
	changed {
		schedToCurrentBeat({ this.update });
						
	}
	
	play { arg clock;
		var str;
		str = super.play(clock);
		this.class.playStreams = this.class.playStreams.add(str.stream);
		^str
		
	}	
	
	asStream {
		^DefStream(this).defaultValue_(Event.default);
	
	}
	
}

//remove nils from stream

Pnil {
	var <>pattern, <>value;
	*new { arg pattern, value=0; 
		^super.newCopyArgs(pattern, value) 
	}
	
	asStream {
		^Routine({ arg inval;
			var str, outval;
			str = pattern.asStream;
			inf.do({
				if(inval.isNil, { inval = value });
				outval = str.next(inval);
				inval = outval.yield(inval);
			});
		});
	}
}


