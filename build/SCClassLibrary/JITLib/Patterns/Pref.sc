
	
Pref : Pattern {
	var <key, <>pattern, <>clock;
	var <>changed = false, toBeChanged, scheduledForChange=false;
	
	classvar <>patterns;
	
	*initClass { 
		patterns = (); 
	}
	
	*at { arg key;
		^this.patterns.at(key);
	}
	*put { arg key, pattern;
		this.patterns.put(key, pattern);
	}
	
	*new { arg key, pattern;
		var p;
	 	p = this.at(key);
	 	if(pattern.notNil, {
			if(p.isNil, {
				p = this.newCopyArgs(key, pattern, TempoClock.default);
				this.put(key, p);
			}, {
				p.pattern_(pattern).changed_(true); 
			});
			
		}, { 
			if(p.isNil, {
				p = this.newCopyArgs(key, this.default, TempoClock.default);
				this.put(key, p);
			});
		})
		^p
	
	}
		
	*default { ^Pbind(\freq, \rest, \dur, 1) }

	asStream {
		^RefStream(this);
	}
	
	storeArgs { ^[key,pattern] }
	
	refresh {
		scheduledForChange = true;
		clock.play({
			toBeChanged.do { |str| str.pattern = pattern; str.resetPat }; 
			changed = false;
			toBeChanged = nil;
			scheduledForChange = false;
			nil
		}, 1.0)
		
	}
	
	
	schedForChange { arg str; 
		toBeChanged = toBeChanged.add(str);
		if(scheduledForChange.not) { this.refresh }
	}
	
	timeToNextBeat { arg quant=1;
		var t;
		t = clock.elapsedBeats;
		^t.roundUp(quant) - t
	}
	
	constrainStream { arg str;
		^Pseq([
			Pfindur(this.timeToNextBeat, str),
			pattern.asStream
		]).asStream
		 
	}
	
}

Prefn : Pref {

	 *default { ^Pn(0, inf) }
	 constrainStream { arg str; ^str }
}



RefStream : Pstr {
	
	var <parent, <changed; 
	
	*new { arg parent;
		^super.new.makeDependant(parent);
	}
	
	makeDependant { arg argParent;
		parent = argParent;
		this.pattern = argParent.pattern;
		this.resetPat;
	}

	key { ^parent.key }
	
		
	next { arg inval;
	
		var outval;
		if(changed.not and: { parent.changed }) {
			changed = true; 
			stream = parent.constrainStream(stream); 
			parent.schedForChange(this);
		};
		outval = stream.next(inval);
		^outval
	}
	
	resetPat { super.resetPat; changed = false }
	

}

