
// contains numerical patterns

PatternProxy : Pattern {
	var <pattern;
	var <>clock, <>quant; 	// quant new pattern insertion. quant should be larger than dur for now
	
	classvar <>defaultQuant, <>action;
	
	// basicNew should be used for instantiation: *new is used in Pdef/Tdef/Pdefn
	*basicNew {Êarg source;
		^super.new.init(source)
	}
		
	*default { ^1 } // safe for duration patterns
	
	init { arg src;
		clock = TempoClock.default; 
		quant = this.class.defaultQuant;
		this.source = src ?? { this.class.default }
	}
	
	constrainStream { arg str; ^pattern.asStream }
	
	source_ { arg pat; this.sched { pattern = pat } }
	source {Ê^pattern }
	pattern_ { arg pat; this.source_(pat) }
		
	embedInStream {Êarg inval;
		var pat, stream, outval;
		pat = pattern;
		stream = pattern.asStream;
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
	
	sched { arg func;
		if(quant.isNil) 
			{ func.value } 
			{ clock.schedAbs(quant.nextTimeOnGrid(clock), { func.value; nil }) }
	}

	storeArgs { ^[pattern] }
	
	/////////////////////////
	// these following methods are factored out for the benefit of subclasses
	// they only work for Pdef/Tdef/Pdefn
	
	*new { arg key, item;
		var res;
		res = this.at(key);
		if(res.isNil) {
				res = this.basicNew(item).key_(key);
				this.put(key, res);
		} {
				if(item.notNil) { res.source = item }
		}
		^res
	
	}

	
	*cmdPeriod { this.all.do { arg item; item.stop } }
	
	*removeAll { 
		this.all.do { arg pat; pat.stop }; 
		this.all = IdentityDictionary.new; 
	}
	
	*at { ^nil }
	*put {}
	key_ {}
	
	clear { this.class.all.removeAt(this.key).stop }
	
	repositoryArgs { ^[this.key, this.source] }
	
	*postRepository { arg keys, stream;
		keys = keys ?? { this.all.keys };
		stream = stream ? Post;
		keys.do { arg key; 
			var item;
			item = this.all[key];
			if(item.notNil) {
				stream << item.class.name << "(" <<<* item.repositoryArgs << ");";
				stream << Char.nl;
			};
		};
	}
	////////////////
	
}

Pdefn : PatternProxy {
	var <>key;
	classvar <>all;
	
	*initClass { 
		all = IdentityDictionary.new;
	}
	*at { arg key;
		^all.at(key);
	}
	*put { arg key, pattern;
		all.put(key, pattern);
	}
	storeArgs { ^[key] } // assume it was created globally
	
}




TaskProxy : PatternProxy {
	var <isPlaying=false, <source;
	var <player, <>playQuant;
	classvar <>defaultQuant=1.0;
	
		
	source_ { arg function;
			pattern = Prout { arg x; 
				protect { 	// this error handling only helps if error is not in substream
					function.value(x);
					nil.alwaysYield; // prevent from calling handler
				} { 
					player.removedFromScheduler 
				} 
			};
			this.wakeUp;
			source = function;
	}
	
	wakeUp { if(isPlaying and: { player.isPlaying.not }) { this.play(quant:playQuant) } }
	

	*default { ^#{ loop { 1.wait } } }
	
	
	constrainStream { arg str;
		^if(quant.notNil and: { str.notNil }) {
			Pseq([
				Pconst(clock.timeToNextBeat(quant), str, 0.001),
				pattern
			])
		} { pattern }.asStream
	}
	
	playOnce { arg argClock, doReset = false, quant;
		clock = clock ? argClock;
		^PauseStream.new(this.asStream).play(clock, doReset, quant ? this.quant)
	}
	
	play { arg argClock, doReset = false, quant;
		isPlaying = true;
		playQuant = quant;
		if(player.isPlaying.not) { player = this.playOnce(argClock, doReset, quant) }
	}
	
	stop { player.stop; isPlaying = false }
	
	pause { if(player.notNil) { this.sched { player.pause } } }
	resume { if(player.notNil) { this.sched { player.resume } } }

	
		
	storeArgs { ^[source] }
}



Tdef : TaskProxy {
	var <>key;
	classvar <>all;
	
	
	*initClass { 
		all = IdentityDictionary.new;
		CmdPeriod.add(this); 
	}
	*at { arg key;
		^all.at(key);
	}
	*put { arg key, pattern;
		all.put(key, pattern);
	}

	storeArgs { ^[key] }
	}



// contains event patterns

EventPatternProxy : TaskProxy {
	var <>fadeTime;
	classvar <>defaultQuant=1.0;
	
	source_ { arg item;
		if(item.isKindOf(Function)) // allow functions to be passed in
			{Êsource = item; pattern = PlazyEnvir(item) } 
			{ pattern = source = item };
		this.wakeUp;
	}
	
	*default { ^Pbind(\freq, \rest) }
	
	constrainStream { arg str;
		var dt, tolerance;
		^if(quant.notNil) {
			dt = clock.timeToNextBeat(quant);
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
	
	playOnce { arg argClock, protoEvent, quant;
		clock = clock ? argClock;
		^EventStreamPlayer(this.asStream, protoEvent)
				.play(clock, true, quant ? this.quant) 
	}
	
	// start playing //
	
	play { arg argClock, protoEvent, quant;
		isPlaying = true;
		if(player.isPlaying.not) { player = this.playOnce(argClock, protoEvent, quant) }
	}
	
	storeArgs { ^[source] }
	
}

Pdef : EventPatternProxy {
	var <>key;
	
	classvar <>all;	
				
	storeArgs { ^[key] }
	
	*at { arg key;
		^all.at(key);
	}
	*put { arg key, pattern;
		all.put(key, pattern);
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
			~synthDef = \default;
			~embeddingLevel = 0; // infinite recursion catch
			~play = #{
				var pat, event, outerEvent, recursionLevel, instrument, embeddingLevel;
				pat = ~library.at(~instrument);
				if(pat.notNil and: { (embeddingLevel = ~embeddingLevel) < 8 })
				{
					// preserve information from outer pattern
					outerEvent = currentEnvironment.copy;
					recursionLevel = ~recursionLevel;
					if(recursionLevel.notNil) {
						if(recursionLevel > 0) {
							// in recursion, some inner values have to be overridden
							instrument = ~instrument;
							pat = pat.collect { |inval|
									inval.use {
											~instrument = instrument;
											~parent = outerEvent;
											~recursionLevel = recursionLevel - 1;
										};
										inval
							};
						} {
							// play pattern in the ordinary way
							outerEvent.put(\play, ~prPlay);
							outerEvent.put(\instrument, ~synthDef);
						};
					} {	// avoid recursion, if instrument not set.
						outerEvent.put(\embeddingLevel, embeddingLevel + 1);
						outerEvent.parent_(Event.parentEvents.noteEvent);
					};
					
					pat = Pfindur(~sustain.value, pat, 0.0); // avoid cutting off last event
					pat.play(thisThread.clock, outerEvent, 0.0);
				} {
					~prPlay.value;
				}
			}
		
		};
		Event.parentEvents.put(\phraseEvent, phraseEvent);
	}
	
}



PbindProxy : Pattern {
	var <>pairs, <source;
	
	*new { arg ... pairs;
		^super.newCopyArgs(pairs).init
	}
	init {
		forBy(0, pairs.size-1, 2) { arg i;
			pairs[i+1] = PatternProxy.basicNew(pairs[i+1])
		};
		source = EventPatternProxy.basicNew(Pbind(*pairs));
	}
	embedInStream { arg inval;
		^source.embedInStream(inval)
	}
	find { arg key; // optimize later maybe.
		pairs.pairsDo {Ê|u,x,i| if(u === key) {Ê^i } }; ^nil
	}
	quant_ { arg val;
		pairs.pairsDo { arg key, item; item.quant = val }; // maybe use ref later
		source.quant = val;
	}
	quant { ^source.quant }
	
	at { arg key; var i; i = this.find(key); ^if(i.notNil) { pairs[i+1] } {Ênil } }
	
	// does not yet work with adding arrayed keys/values
	set { arg ... args; // key, val ...
		var changedPairs=false, quant;
		quant = this.quant;
		args.pairsDo { |key, val|
			var i, remove;
			i = this.find(key);
			if(i.notNil)
			{ 
				if(val.isNil) {
					pairs.removeAt(i);
					pairs.removeAt(i);
					changedPairs = true;
				}{
					pairs[i+1].source = val
				};
			}{ 
				pairs = pairs ++ [key, PatternProxy.basicNew(val).quant_(quant)];
				changedPairs = true;
			};
		
		};
		if(changedPairs) { source.source =  Pbind(*pairs) };
		
	}
	
	//source_ { arg pat; source.source = pat }
	storeArgs { ^pairs.collect(_.source) }
}


Pbindef : Pdef {
	*new { arg ... pairs;
		var key, pat, src;
		key = pairs.removeAt(0);
		pat = super.new(key);
		src = pat.source;
		if(pairs.isEmpty.not) {
			if(src.class === PbindProxy) {
				src.set(*pairs);
				pat.wakeUp;
			} {
				if(src.class === Pbind and: {Êsrc.patternpairs != #[\freq, \rest]}) 
				{
					src.patternpairs.pairsDo { |key, pat|
						if(pairs.includes(key).not) { 
							pairs = pairs.add(key); 
							pairs = pairs.add(pat);
						}
					}
				};
				
				src = PbindProxy.new(*pairs).quant_(pat.quant);
				pat.source = src
			};
		};
		
		^pat
		
	}
	storeArgs { ^[key]++pattern.storeArgs }
	repositoryArgs { ^this.storeArgs }
	quant_ { arg val; super.quant = val; source.quant = val }
}




// general purpose lookup stream

Pdict : Pattern {
	var <>dict, <>which, <>repeats, <>default;
	*new { arg dict, which, repeats=inf, default;
		^super.newCopyArgs(dict, which, repeats, default);
	}
	
	embedInStream { arg inval;
		var keyStream, key;
		keyStream = which.asStream;
		repeats.value.do({
			key = keyStream.next;
			if(key.isNil) { ^inval };
			inval = (dict.at(key) ? default).embedInStream(inval);
		});
		^inval
	}
}



