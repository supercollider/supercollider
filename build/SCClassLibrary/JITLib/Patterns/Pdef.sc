
// contains numerical patterns

Pdefn : Pattern {
	var <key, <pattern;
	var <>clock, <>quant, <>offset=0; 	// quant new pattern insertion
	classvar <>all, <>defaultQuant;
	
	*initClass { 
		all = IdentityDictionary.new;
	}
	
	*at { arg key;
		^this.all.at(key);
	}
	*put { arg key, pattern;
		this.all.put(key, pattern);
	}
	
	*new { arg key, pattern;
		var p;
	 	p = this.at(key);
	 	if(p.notNil) {
	 		if(pattern.notNil) { p.pattern_(pattern) };
	 	} {
	 		if(pattern.isNil) { pattern = this.default };
	 		p = this.newCopyArgs(key).pattern_(pattern)
	 			.clock_(TempoClock.default).quant_(this.defaultQuant);
			this.put(key, p);
		};
		^p
	}
	
	*default { ^1 } // safe for duration patterns
	
	constrainStream { arg str; ^pattern.asStream }
	
	pattern_ { arg pat;
		if(quant.isNil) {
			pattern = pat
		} {
			this.sched { pattern = pat; nil }
		}
	}
	
	asStream {
		^RefStream(this);
	}
	
	storeArgs { ^[key,pattern] }
	
	timeToNextBeat {
		var t;
		t = clock.elapsedBeats;
		^t.roundUp(quant) - t + offset
	}

	clear { all.removeAt(key) }
	sched { arg task;
		clock.schedAbs(clock.elapsedBeats.roundUp(quant) + offset, task)
	}
	
	*removeAll { all = IdentityDictionary.new; }
	
	// posting stream usage //
	
	*startPost { RefStream.action = { arg str; ("----" + str.parent.key).postln } }
	*stopPost { RefStream.action = nil }
	
}


// contains event patterns

Pdef : Pdefn {
	var <isPlaying=false;
	var <player;
	classvar <>all, <>defaultQuant=1.0;
	
	*initClass { 
		all = IdentityDictionary.new; 
		CmdPeriod.add(this); 
	}
	*cmdPeriod { this.all.do { arg item; item.stop } }
		
	*default { ^Pbind(\freq, \rest) }
	
	pattern_ { arg pat;
		pattern = pat;
		if(isPlaying and: { player.isPlaying.not }) { this.play }
	}

	constrainStream { arg str;
		^if(quant.notNil) {
			Pseq([
				Pfindur(this.timeToNextBeat, str, 0.001),
				pattern
			])
		} { pattern }.asStream
	}
	
	// playing one instance //
	// problem: offset is not implemented in PauseStream
	
	playOnce { arg argClock, protoEvent, quant;
		^EventStreamPlayer(this.asStream, protoEvent)
			.play(argClock ? clock, true, quant ? this.quant) 
	}
	
	play { arg argClock, protoEvent, quant;
		isPlaying = true;
		if(player.isPlaying.not) { player = this.playOnce(argClock, protoEvent, quant) }
	}
	stop { player.stop; isPlaying = false }
	pause { if(player.notNil) { player.pause } }
	resume { if(player.notNil) { player.resume } }

	clear { this.class.all.removeAt(key).stop }
	*removeAll { 
		this.all.do { arg pat; pat.stop }; 
		all = IdentityDictionary.new; 
	}

	
}



Tdef : Pdef {
	classvar <>all, <>defaultQuant=1.0;
	
	*initClass { 
		all = IdentityDictionary.new;
		CmdPeriod.add(this); 
	}
	
	*new { arg key, func;
		var pattern;
		if(func.notNil) { 
			pattern = Prout({ arg x; 
				protect { func.value(x) } { this.at(key).player.removedFromScheduler } 
			}) 
		};
		^super.new(key, pattern)
	}

	*default { ^Routine({ loop { 1.wait } }) }
	
	storeArgs { ^[key,pattern.routineFunc] }
	
	constrainStream { arg str;
		^if(quant.notNil and: { str.notNil }) {
			Pseq([
				Pconst(this.timeToNextBeat, str, 0.001),
				pattern
			])
		} { pattern }.asStream
	}
	
	playOnce { arg argClock, doReset = false, quant;
		^PauseStream.new(this.asStream)
			.play(argClock ? clock, doReset, quant ? this.quant)
	}
	play { arg argClock, doReset = false, quant;
		isPlaying = true;
		if(player.isPlaying.not) { player = this.playOnce(argClock, doReset, quant) }
	}

}


// created by Pdef, Pdefn and Tdef

RefStream : Stream {
	classvar <>action;
	var <>stream, <>pattern; 
	var <parent; 
	
	*new { arg parent;
		^super.new.parent_(parent);
	}
	
	parent_ { arg argParent;
		parent = argParent;
		action.value(this);
	}
	
	next { arg inval;
		var outval;
		if((parent.pattern !== pattern)) {
			pattern = parent.pattern;
			stream = parent.constrainStream(stream);
		};
		outval = stream.next(inval);
		^outval
	}
	
}


// general purpose lookup stream

Pdict : Pattern {
	var <>dict, <>which, <>repeats, <>default;
	*new { arg dict, which, repeats=inf, default;
		^super.newCopyArgs(dict, which, repeats, default);
	}	
	asStream {
		^Routine.new({ arg inval;
			var keyStream, key;
			keyStream = which.asStream;
			repeats.value.do({
				key = keyStream.next;
				if(key.notNil) {
					inval = (dict.at(key) ? default).embedInStream(inval);
				} {
					nil.alwaysYield
				}
			})
		});
	}
}


