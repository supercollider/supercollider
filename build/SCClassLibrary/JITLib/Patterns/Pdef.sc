

//pattern reference for numerical (non-event) streams
//when putting in patterns with infinite length the limit
//needs to be set, otherwise the streams will never update
	


Pdefn : PauseStream {
	var <key, <pattern, <>limit; 	
	classvar <>patterns;
	
	*initClass { 
		patterns = IdentityDictionary.new; 
	}
	
	*default { ^Pn(0, inf) }
	
	*at { arg key;
		^this.patterns.at(key);
	}
	*put { arg key, pattern;
		this.patterns.put(key, pattern);
	}
	//
	*newDefault { arg key,pattern;
		^this.make(key)
	}
	setProperties { arg  argPattern, argLimit;
		pattern = argPattern ? this.class.default; 
		limit = argLimit;
	}
	//
	*new { arg key, pattern, limit;
		var p;
	 	p = this.at(key);
	 	if(pattern.notNil, {
			if(p.isNil, {
				p = this.make(key, pattern, limit);
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
	
		
	*make { arg key, pattern, limit;
		^super.new.initProperty(key, pattern, limit).clear;
	}

	
	initProperty { arg argKey, argPattern, argLimit;
		key = argKey; pattern = argPattern; limit = argLimit;
	}
	
	clear {}
	
	embedInStream { arg inval;
		^this.asStream.embedInStream(inval)
	}
	
	asStream {
		 ^if(limit.notNil, {
			Pfin(limit, pattern).asStream
		}, {
			pattern.asStream
		})
	}
	
	pattern_ { arg pat; pattern = pat }
	
	
	
	storeArgs { ^if(limit.notNil, { [key,pattern,limit] }, { [key,pattern] }) }
	
}




// pattern reference for event streams.
// streames can be infinite, they get released on change

Pdef : Pdefn {
	var <children;
	var <>event, <>muteCount = 0; // some properties from EventStream
	classvar <>patterns, <>players;
	
		
	asStream {
	
		^RefStream(this);
	
	}
	
	pattern_ { arg pat;
		
		pattern = pat;
		this.changed;
	}

	
		
	*default { 
				^Pbind(
					\freq, \rest, 
					\dur, 1
		 	 	)
	}
	
	// remove playing streams from me
	
	*initClass {
		patterns = IdentityDictionary.new; 
		CmdPeriod.add(this)
	}
	
	*cmdPeriod { this.clear }
	
	*clear {
		players.collect({ arg item; item.key }).debug;
		players.copy.do({ arg item;
			item.stop;
		});
	}
	
	clear { 
		children = IdentitySet.new;
		event = Event.default; //for now 
 	}	
	
	
	play { arg argClock, doReset = false;
		if (stream.notNil, { "already playing".postln; ^this });
		//if(originalStream.isNil, { 
			if(originalStream.notNil, { children.remove(originalStream) });	
			this.initStream; 
		//});		
		if (doReset, { this.reset });
		clock = argClock ? TempoClock.default;
		stream = originalStream; 
		this.class.players = this.class.players.add(this);
		clock.play(this);
	}
	
	stop {
		super.stop;
		children.do({ arg item; 
				item.releaseUpstream;
		});
		this.class.players.remove(this);
		 
	}
	
	initStream { 
		originalStream = this.asStream;
	}
	
	update {
		children.do({ arg item; item.pattern = pattern });
	}
	
	changed { 
		clock.play({
			\changed.debug;
			this.update; nil 
		});
	}
	
	
	mute { muteCount = muteCount + 1; }
	unmute { muteCount = muteCount - 1; }
	
	next { arg inTime;
		var outEvent, nextTime;
		outEvent = stream.next(event); //change later
		if (outEvent.isNil, {
			stream = nil;
			^nil
		},{
			if (muteCount > 0, { outEvent.put(\freq, \rest) });
			if ((nextTime = outEvent.play).isNil, { stream = nil });
			^nextTime
		});
	}
	
}


RefStream : Pstr {
	
	var <parent, <relatives, <informedAboutMe; 
	
	//relatives:
	//release them when the stream changes, as they are not needed anymore.
	
	//informedAboutMe: 
	//remove me from their list of relatives when I get released
	
	*new { arg parent;
		^super.new.clear.init(parent);
	}
	
	init { arg argParent; 
		parent = argParent;
		this.makeDependant(argParent);
		this.pattern = argParent.pattern; 
		this.clear;
	}
	
	key { ^parent.key }
	
	clear {
		relatives = IdentitySet.new;
		informedAboutMe = IdentitySet.new;
	}
	
	relativesAsString {
		^relatives.collect({ arg item; if(item.notNil, {item.key}) })
	}
	
	pattern_ { arg pat;
			pattern = pat;
			this.resetPat;
	}
	
	resetPat {
			this.releaseUpstream;
			stream = pattern.asStream;
	}
	
	embedInStream { arg inval;
		var incomingRelatives;
		//check for referential loop:
		incomingRelatives = inval.at(\relatives).asCollection; //implement nil.any
		if(incomingRelatives.any({ arg item; item.parent === parent }), {
			"avoided a referential loop.".postln; 
			Routine({ loop({ parent.event.yield }) }).embedInStream;
		}, {
			super.embedInStream(inval)
		});
		^inval
		
	}
	
	next { arg inval;
	
		var outval;
		inval = this.addToEvent(inval);
		outval = stream.next(inval);
		^if(outval.isNil, { 
			this.release;
			nil
		}, {
			outval;
		});
	
	}
	
	// private implementation //
	
	addToEvent { arg event;
		var set;
		set = event.at(\relatives);
		if(set.isNil, {
				set = IdentitySet.with(this);
			}, {
				set.do({ arg item; 
							item.addRelative(this);
				});
				informedAboutMe = set;
				set = set.copy.add(this);
		});
		
		event = event.copy.put(\relatives, set);
		^event 

	}
	
	makeDependant { arg argParent;
			
		parent = argParent;
		parent.children.add(this);
	}
	
	addRelative { arg defstream;
		relatives.add(defstream);
	}
	
	removeRelative { arg defstream; //when a stream ends by itself, it does this.
		relatives.remove(defstream);
	}
	
	releaseUpstream {
		if(relatives.notNil, {
			relatives.do({ arg item; item.release });
			relatives.makeEmpty;
		})
	}
	
	release {
		informedAboutMe.do({ arg item; item.removeRelative(this) });
		parent.children.remove(this);
	}

	

}





