
// contains numerical patterns

Pdefn : Pattern {
	var <key, <pattern, <>clock;
	var <>quant; 	// quant new pattern insertion
	classvar <>all, <>defaultQuant;
	
	*initClass { 
		all = ();
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
	
	*default { ^0 }
	
	constrainStream { arg str; ^pattern.asStream }
	
	pattern_ { arg pat;
		if(quant.isNil) {
			pattern = pat
		} {
			clock.play({ 
				pattern = pat;
				nil 
			}, quant)
		}
	}
	
	asStream {
		^RefStream(this);
	}
	
	storeArgs { ^[key,pattern] }
	
	timeToNextBeat {
		var t;
		t = clock.elapsedBeats;
		^(t.roundUp(quant) - t); // + offset
	}

	clear { all.removeAt(key) }
//	sched { arg task;
//		clock.schedAbs(clock.elapsedBeats.roundUp(quant) + offset, task) }
//	}
	
	*removeAll { this.initClass }
		// posting stream usage //
	
	*startPost { RefStream.action = { arg str; ("----" + str.parent.key).postln } }
	*stopPost { RefStream.action = nil }
	
}


// contains event patterns

Pdef : Pdefn {
	var <isPlaying=false;
	var <player;
	classvar <>all, <>defaultQuant=1.0;
	
	*initClass { all = () }
		
	*default { ^Pbind(\freq, \rest) }
	
	pattern_ { arg pat;
		pattern = pat;
		if(isPlaying and: { player.isPlaying.not }) { this.play }
	}
	
	/*
	
	pattern_ { arg pat;
		if(min.notNil or: { max.notNil }) { pat = Psync(pat, min, max) };
		pattern = pat;
		if(isPlaying and: { this.streamIsPlaying.not }) { this.play }
	}

	
	
	*/

	constrainStream { arg str;
		^if(quant.notNil) {
			Pseq([
				Pfindur(this.timeToNextBeat, str, 0.001),
				pattern
			])
		} { pattern }.asStream
	}
	// playing one instance //
	
	*cmdPeriod { all.do { arg item; item.stop } }
	
	playOnce { arg argClock, protoEvent, quant;
		^EventStreamPlayer(this.asStream, protoEvent)
			.play(argClock ? clock, true, quant ? this.quant) 
	}
	
	play { arg argClock, protoEvent, quant;
		isPlaying = true;
		CmdPeriod.add(this.class);
		if(player.isPlaying.not) { player = this.playOnce(argClock, protoEvent, quant) }
	}
	stop { player.stop; isPlaying = false }
	pause { if(player.notNil) { player.pause } }
	resume { if(player.notNil) { player.resume } }

	clear { this.class.all.removeAt(key).stop }
	*removeAll { this.all.do { arg pat; pat.stop }; this.initClass; }

	
}


Tdef : Pdef {
	classvar <>all, <>defaultQuant=1.0;
	
	*initClass { all = () }
	
	*new { arg key, func;
		var pattern;
		if(func.notNil) { pattern = Prout(func) };
		^super.new(key, pattern)
	}

	*default { ^Routine({ loop { 1.wait } }) }
	
	storeArgs { ^[key,pattern.routineFunc] }
	
	constrainStream { arg str;
		^if(quant.notNil) {
			Pseq([
				Pconst(this.timeToNextBeat, str, 0.001),
				pattern
			])
		} { pattern }.asStream
	}
	
	play { arg argClock, doReset = false, quant;
		^PauseStream.new(this.asStream)
			.play(argClock ? clock, doReset, quant ? this.quant)
	}

}

//create an addressable and resetable single instance of a stream

Pstr : Stream {
	var <>stream, <>pattern;
	
	*new { arg pattern; 
		^this.basicNew(pattern).resetPat;
	}
	
	*basicNew { arg pattern;
		^super.new.pattern_(pattern)
	}
	
	next { arg inval; ^stream.next(inval) }
	
	reset { stream.reset }
	
	resetPat { stream = pattern.asStream }
	
	refresh { arg pat;
		if(pat.notNil, { pattern = pat; this.resetPat }, { this.reset });
	}
	
}

// created by Pdef, Pdefn and Tdef

RefStream : Pstr {
	classvar <>action;
	var <parent, <changed; 
	
	*new { arg parent;
		^super.new.makeDependant(parent);
	}
	
	makeDependant { arg argParent;
		parent = argParent;
		pattern = argParent.pattern;
		action.value(this);
		this.resetPat;
		
	}

	key { ^parent.key }
	
	next { arg inval;
		var outval;
		if((parent.pattern !== pattern) and: { changed.not }) {
			pattern = parent.pattern;
			stream = parent.constrainStream(stream);
		};
		outval = stream.next(inval);
		^outval
	}
	
	resetPat {  stream = pattern.asStream; changed = false }
	

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


