
// contains numerical patterns

Pdefn : Pattern {
	var <key, <pattern;
	var <>clock, <>quant, <>offset=0; 	// quant new pattern insertion
	classvar <all, <>defaultQuant, <>action;
	
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
	
	asStream { ^Routine.new({ arg inval; this.embedInStream(inval) }) }
	
	embedInStream {Êarg inval;
		var pat, stream, outval;
		pat = pattern;
		stream = pattern.asStream;
		action.value(this, stream);
		while { 
			if((pat !== pattern)) {
					pat = pattern;
					stream = this.constrainStream(stream);
			};
			outval = stream.next(inval);
			outval.notNil
		} {
			inval = outval.yield;
		}
		^inval
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
	doInTime { arg func;
		if(quant.isNil) { func.value } { this.sched({ func.value; nil }) }
	}
	
	*removeAll { all = IdentityDictionary.new; }
	
	// posting stream usage //
	
	*startPost { action = { arg str; ("----" + str.parent.key).postln } }
	*stopPost { action = nil }
	
}


Tdef : Pdefn {
	var <isPlaying=false, <player;
	classvar <all, <>defaultQuant=1.0;
	
	*initClass { 
		all = IdentityDictionary.new;
		CmdPeriod.add(this); 
	}
	*cmdPeriod { this.all.do { arg item; item.stop } }
	
	*new { arg key, func;
		var pattern;
		if(func.notNil) { 
			pattern = Prout({ arg x; 
				protect { 	// this error handling only helps if error is not in substream
					func.value(x);
					nil.alwaysYield; // prevent from calling handler
				} { 
					this.at(key).player.removedFromScheduler 
				} 
			}) 
		};
		^super.new(key, pattern)
	}
	
	pattern_ { arg pat;
		pattern = pat;
		if(isPlaying and: { player.isPlaying.not }) { this.play }
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
	
	stop { player.stop; isPlaying = false }
	pause { if(player.notNil) { this.doInTime { player.pause } } }
	resume { if(player.notNil) { this.doInTime { player.resume } } }

	clear { this.class.all.removeAt(key).stop }
	*removeAll { 
		this.all.do { arg pat; pat.stop }; 
		all = IdentityDictionary.new; 
	}
}



// contains event patterns

Pdef : Tdef {
	var <>fadeTime;
	
	classvar <all, <>defaultQuant=1.0;

	*new { arg key, pattern; // this is a copy from Pdefn, to avoid a redirection
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
	
	pattern_ { arg pat;
		if(pat.isKindOf(Function)) {Êpat = Plazy(pat) }; // allow functions to be passed in
		pattern = pat;
		if(isPlaying and: { player.isPlaying.not }) { this.play }
	}
		
	*default { ^Pbind(\freq, \rest) }
	
	storeArgs { ^[key,pattern] }
	
	
	constrainStream { arg str;
		var dt, tolerance;
		^if(quant.notNil) {
			dt = this.timeToNextBeat;
			tolerance = quant % dt % 0.125;
			if(fadeTime.isNil) {
				if(dt < 0.01) { 
					Routine({ arg inval;
						Event.silent(dt).yield;
						pattern.embedInStream(inval)
					})
				}{	
					Pseq([
						Pfindur(dt, str, tolerance),
						pattern
					])
				}
			} {
				Ppar([
					PfadeOut(str, fadeTime, dt, tolerance),
					PfadeIn(pattern, fadeTime, dt, tolerance)
				])
			}
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
	
	*initClass {
		var phraseEvent;
		
		all = IdentityDictionary.new; 
		CmdPeriod.add(this);
		
		Class.initClassTree(Event);
		phraseEvent = Event.parentEvents.noteEvent.copy;
		phraseEvent.use {
			~library = all;
			~prPlay = ~play;
			~play = #{
				var pat, event, recursionLevel;
				pat = ~library.at(~instrument);
				if(pat.notNil) {
					event = Event.new.parent_(currentEnvironment.copy);
					recursionLevel = ~recursionLevel;
						if(recursionLevel.isNil) {
							event.put(\instrument, \default); // avoid recursion
						} {
							if(recursionLevel > 0) {
								event.put(\recursionLevel, recursionLevel - 1);
								pat = Pset(\instrument, ~instrument, pat);						}
					};
					pat = Pfindur(~sustain.value, pat);
					pat.play(thisThread.clock, event, 0.0);
				} {
					~prPlay.value;
				}
			}
		
		};
		Event.parentEvents.put(\phraseEvent, phraseEvent);
	}
	
}




// general purpose lookup stream

Pdict : Pattern {
	var <>dict, <>which, <>repeats, <>default;
	*new { arg dict, which, repeats=inf, default;
		^super.newCopyArgs(dict, which, repeats, default);
	}
	
	asStream { ^Routine.new({ arg inval; this.embedInStream(inval) }) }	
	embedInStream { arg inval;
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
	}
}


