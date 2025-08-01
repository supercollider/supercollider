// contains numerical / value patterns

PatternProxy : Pattern {

	var <source, <pattern, <>envir;
	var >clock, quant, <>condition=true, reset;

	// quant value for new pattern insertio can be an array, like [quant, phase, timingOffset]
	// in EventPatternProxy it can be [quant, phase, timingOffset, onset]

	classvar <>defaultQuant, defaultEnvir;

	*new { |source|
		^super.new.source_(source)
	}

	*default { ^1 } // safe for duration patterns
	*defaultValue { ^1 }

	clock { ^clock ? TempoClock.default }

	quant { ^quant ??  { this.class.defaultQuant } }
	quant_ { |val| quant = val }

	source_ { |obj|
		var pat = if(obj.isKindOf(Function)) { this.convertFunction(obj) }{ obj };
		if (obj.isNil) { pat = this.class.default };
		pattern = pat;
		source = obj; // keep original here.
		this.changed(\source, obj);
	}

	setSource { |obj|
		pattern = obj;
		source = obj; // keep original here.
		this.changed(\source, obj);
	}

	count { |n=1|
		condition = { |val,i| i % n == 0 }
	}

	reset { reset = reset ? 0 + 1 }

	sched { |func|
		if(quant.isNil)
		{ func.value }
		{ this.clock.schedAbs(quant.nextTimeOnGrid(this.clock), { func.value; nil }) }
	}


	convertFunction { |func|
		^Prout {
			var inval = func.def.prototypeFrame !? { inval = this.defaultEvent };
			func.value( inval ).embedInStream(inval)
		}
	}

	*parallelise { |list| ^Ptuple(list) }

	pattern_ { |pat| this.source_(pat) }

	timingOffset_ { |val| quant = quant.instill(2, val) }
	timingOffset { |val| ^quant.obtain(2) }
	phase_ { |val| quant = quant.instill(1, val) }
	phase { |val| ^quant.obtain(1) }
	quantBeat_ { |val| quant = quant.instill(0, val) }
	quantBeat { |val| ^quant.obtain(0) }

	set { |...args|
		if(envir.isNil) { this.envir = this.class.event };
		args.pairsDo { arg key, val; envir.put(key, val) };
		this.changed(\set, args);
	}

	unset { |...args|
		if(envir.notNil) {
			args.do { |key| envir.removeAt(key) };
			this.changed(\unset, args);
		}
	}

	get { |key|
		^if(envir.notNil) { envir[key] } { nil };
	}

	place { |...args|
		if(envir.isNil) { this.envir = this.class.event };
		args.pairsDo { arg key, val; envir.place(key, val) }; // place is defined in the event.
	}

	isEventPattern { ^false }

	receiveEvent { ^nil }

	embed { |val|
		^if(val.notNil) { Pchain(this, val) } { this }.embedInStream
	}

	embedInStream { |inval, embed = true, default|

		var outval, count = 0;
		var pat = pattern;
		var test = condition;
		var resetTest = reset;
		var stream = pattern.streamArg(embed);

		while {
			this.receiveEvent(inval); // used in subclass

			if(
				(reset !== resetTest)
				or: { pat !== pattern and: { test.value(outval, count) } }
			) {
				pat = pattern;
				test = condition;
				resetTest = reset;
				count = 0;
				stream = this.constrainStream(stream, pattern.streamArg(embed));
			};
			outval = stream.next(inval) ? default;
			count = count + 1;
			outval.notNil
		}{
			inval = outval.yield;
		};
		^inval
	}

	asStream {
		^Routine { arg inval;
			this.embedInStream(inval, false)
		}
	}

	endless { |default|
		^Prout { arg inval;
			this.embedInStream(inval, true, default ? this.class.defaultValue)
		}
	}

	constrainStream { |stream, newStream|
		^if(this.quant.isNil) {
			newStream
		} {
			Pseq([PfinQuant(EmbedOnce(stream), this.quant, clock), newStream]).asStream
		}
	}

	defaultEvent {
		if(envir.isNil) { envir = this.class.event };
		^if(envir[\independent] === true) { (parent:envir) } { envir }
	}


	storeArgs { ^[pattern] }

	copy {
		^super.copy.copyState(this)
	}

	copyState { |proxy|
		envir = proxy.envir.copy;
		this.source = proxy.source;
	}


	// these following methods are factored out for the benefit of subclasses
	// they only work for Pdef/Tdef/Pdefn.

	*removeAll {
		this.clear;
		this.all.makeEmpty;
	}

	*clear {
		this.all.do { |pat| pat.clear };
		this.all.clear;
	}

	clear {
		this.stop;
		this.source = nil;
		^nil
	}

	remove {
		if(this.class.hasGlobalDictionary) { this.class.all.removeAt(this.key) };
		this.clear;
	}

	// backward compatibility
	*basicNew { |source|
		^super.new.init(source)
	}

	// global storage

	*at { |key|
		^if(this.hasGlobalDictionary) { this.all.at(key) } { nil }
	}

	repositoryArgs { ^[this.key, this.source] }

	*postRepository { |keys, stream|
		if(this.hasGlobalDictionary.not) { Error("This class has no global repository.").throw };
		keys = keys ?? { this.all.keys };
		stream = stream ? Post;
		keys.do { |key|
			var item;
			item = this.all[key];
			if(item.notNil and: { item.source !== this.default }) {
				stream << item.class.name << "(" <<<* item.repositoryArgs << ")";
				if(item.envir.notNil and: { item.envir.notEmpty }) {
					stream << ".set(" <<<* item.envir.asKeyValuePairs << ")"
				};

				stream << ";\n"
			};
		};
	}

	*event { |proxyClass = (PatternProxy)|
		var event, res;
		if(defaultEnvir.isNil) {
			defaultEnvir = Event.default;
			defaultEnvir.parent = defaultEnvir.parent.copy.putAll(
				(
					forward: #{ 1 },
					proxyClass: proxyClass,
					get: {|e, key|
						var x = e.at(key);
						if(x.isKindOf(e.proxyClass).not) { x = e.proxyClass.new; e.put(key, x); };
						x
					},
					place: { |e, key, val|
						var x = e.at(key);
						if(x.isKindOf(e.proxyClass).not) { x = e.proxyClass.new; e.put(key, x) };
						x.source_(val)
					}
				)
			);
		};
		event = defaultEnvir.copy;
		//	event[\self] = event; // this risks a crash on storeOn.
		^event
	}

	*hasGlobalDictionary { ^false }

}

Pdefn : PatternProxy {
	var <key;
	classvar <>all;

	*initClass {
		all = IdentityDictionary.new;
	}
	*new { |key, item|
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	map { |...args|
		if(envir.isNil) { this.envir = () };
		args.pairsDo { |key, name| envir.put(key, Pdefn(name)) };
		this.changed(\map, args);
	}

	storeArgs { ^[key] } // assume it was created globally

	copy { |toKey|
		if(toKey.isNil or: { key == toKey }) { Error("can only copy to new key (key is %)".format(toKey)).throw };
		^this.class.new(toKey).copyState(this)
	}

	dup { |n = 2| ^{ this }.dup(n) } // avoid copy in Object::dup

	prAdd { |argKey|
		key = argKey;
		all.put(argKey, this);
	}

	*hasGlobalDictionary { ^true }

}


// contains time patterns (tasks)

TaskProxy : PatternProxy {
	var <player, <>playQuant;
	classvar <>defaultQuant=1.0;

	storeArgs { ^[source] }

	source_ { |obj|
		pattern = if(obj.isKindOf(Function)) { this.convertFunction(obj) }{ obj };
		if (obj.isNil) { pattern = this.class.default; source = obj; };
		this.wakeUp;
		source = obj;
		this.changed(\source, obj);
	}

	convertFunction { |func|
		^Prout { |inevent|
			var inval = func.def.prototypeFrame !? { this.defaultEvent };
			if(inevent.isNumber or: {inevent.isNil} or: { inval.isNil }) {
				inevent = inval
			} {
				inevent.copy.parent_(inval);
			};
			func.value(inevent)
		}
	}


	isEventPattern { ^true }

	*default { ^Pn(this.defaultValue, 1) }

	// here, the streams return values which are used as durations
	// for scheduling the stream itself

	constrainStream { |stream, newStream|
		^if(this.quant.isNil) {
			newStream
		} {
			Pseq([
				EmbedOnce(Pconst(thisThread.clock.timeToNextBeat(this.quant), stream, 0.001)),
				newStream
			]).asStream
		}
	}

	align { arg argQuant;
		quant = argQuant;
		this.source = this.source.copy;
	}


	////////// playing interface //////////

	playOnce { |argClock, doReset = (false), quant|
		var pauseStream = PauseStream(
			Pprotect(this, { pauseStream.streamError }).asStream
		);
		^pauseStream.play(argClock ? this.clock, doReset, quant ? this.quant)
	}

	play { arg argClock, doReset=false, quant;
		argClock = argClock ? this.clock;
		playQuant = quant ? this.quant;
		if(player.isNil) {
			player = this.playOnce(argClock, doReset, playQuant);
		} {
			// resets when stream has ended or after pause/cmd-period:
			if (player.streamHasEnded or: { player.wasStopped }) { doReset = true };

			if(player.isPlaying.not) {
				player.play(argClock, doReset, playQuant);
			} {
				if (doReset) { player.reset };
			}
		}
	}

	wakeUp {
		if(this.isPlaying) { this.play(quant:playQuant) }
	}

	asProtected {
		// let's deprecate this method in 3.12
		^Pprotect(this, { if(this.player.notNil) { this.player.streamError } })
	}

	// check playing states:
	isPlaying { ^player.notNil and: { player.wasStopped.not } }
	isActive { ^this.isPlaying and: { player.streamHasEnded.not } }
	hasSource { ^source.notNil }
	hasEnvir { ^envir.notNil }
	hasPlayer { ^player.notNil }
	hasEnded { ^player.isNil or: { player.streamHasEnded } }
	isPaused { ^player.isNil or: { player.wasStopped  } }
	canPause { ^player.notNil and: { player.canPause } }

	fork { arg clock, quant, event;
		if (event.notNil and: { envir.notNil }) { event.parent_(envir) };
		^Routine { this.embedInStream(event) }.play(clock ? thisThread.clock, quant)
	}

	stop { player.stop; player = nil; }

	pause { if(player.notNil) { this.sched { player.pause } } }
	resume { arg clock, quant;
		player !? { player.resume(clock ? this.clock, quant ? this.quant) }
	}

}

Tdef : TaskProxy {
	var <key;
	classvar <>all;

	*initClass {
		all = IdentityDictionary.new;
	}

	*new { |key, item|
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	storeArgs { ^[key] }

	copy { |toKey|
		if(toKey.isNil or: { key == toKey }) { Error("can only copy to new key (key is %)".format(toKey)).throw };
		^this.class.new(toKey).copyState(this)
	}

	dup { |n = 2| ^{ this }.dup(n) } // avoid copy in Object::dup

	prAdd { |argKey|
		key = argKey;
		all.put(argKey, this);
	}

	*hasGlobalDictionary { ^true }

}

// contains event patterns

EventPatternProxy : TaskProxy {
	var <>fadeTime;
	classvar <>defaultQuant=1.0;

	storeArgs { ^[source] }

	source_ { |obj|
		if(obj.isKindOf(Function)) // allow functions to be passed in
		{ pattern = PlazyEnvirN(obj) }
		{ if (obj.isNil)
			{ pattern = this.class.default }
			{ pattern = obj }
		};
		envir !? { pattern = pattern <> envir };
		this.wakeUp;
		source = obj;
		this.changed(\source, obj);
	}

	envir_ { arg dict;
		envir = dict;
		this.source = source;
	}

	*defaultValue { ^Event.silent }

	embedInStream { |inval, embed = true, default|

		var outval, count=0;
		var pat = pattern;
		var test = condition;
		var resetTest = reset;
		var stream = pattern.streamArg(embed);
		var cleanup = EventStreamCleanup.new;

		while {
			this.receiveEvent(inval);
			if(
				(reset !== resetTest)
				or: { pat !== pattern and: { test.value(outval, count) } }
			) {
				pat = pattern;
				test = condition;
				resetTest = reset;
				count = 0;
				// inval is the next event that will be yielded
				// constrainStream may add some values to it so *it must be yielded*
				stream = this.constrainStream(stream, pattern.streamArg(embed), inval, cleanup);
			};
			outval = stream.next(inval) ? default;
			count = count + 1;
			outval.notNil
		}{
			outval = cleanup.update(outval);
			inval = outval.yield;
			if(inval.isNil) { ^nil.alwaysYield }
		};
		^inval

	}

	constrainStream { arg stream, newStream, inval, cleanup;
		var delta, tolerance, fadeOutCleanup;
		var quantBeat, catchUp, deltaTillCatchUp, forwardTime, quant = this.quant;

		^if(this.quant.isNil) {
			cleanup !? { cleanup.exit(inval) };
			newStream
		} {
			quantBeat = this.quantBeat ? 0;
			catchUp = this.outset;

			delta = thisThread.clock.timeToNextBeat(quant);
			tolerance = quantBeat % delta % 0.125;

			if(catchUp.notNil) {
				deltaTillCatchUp = thisThread.clock.timeToNextBeat(catchUp);
				forwardTime = quantBeat - delta + deltaTillCatchUp;
				delta = newStream.fastForward(forwardTime, tolerance) + deltaTillCatchUp;
			};

			if(fadeTime.isNil) {
				if(delta == 0) {
					cleanup !? { cleanup.exit(inval) };
					newStream
				} {
					Pseq([
						EmbedOnce(
							Psync(stream, delta, delta, tolerance, delta).asStream,
							cleanup
						),
						newStream
					]).asStream
				}
			}{
				fadeOutCleanup = cleanup.copy;
				cleanup.clear; // change need be seen by caller function, i.e. embedInStream
				Ppar([
					EmbedOnce(
						PfadeOut(stream, fadeTime, delta, tolerance),
						fadeOutCleanup
					),
					PfadeIn(newStream, fadeTime, delta, tolerance)
				]).asStream
			}
		}
	}

	*parallelise { |list| ^Ppar(list) }

	outset_ { |val| quant = quant.instill(3, val) }
	outset { |val| ^this.quant.obtain(3) }


	// branching from another thread

	fork { arg argClock, quant, protoEvent; // usual fork arg order: clock, quant, ...
		argClock = argClock ? thisThread.clock;
		^EventStreamPlayer(this.asStream, protoEvent).play(argClock, true, quant)
	}

	// playing one instance //

	playOnce { arg argClock, protoEvent, quant;
		^this.fork(argClock ? this.clock, quant ? this.quant, protoEvent)
	}


	////////// playing interface //////////

	play { arg argClock, protoEvent, quant, doReset=false;
		argClock = argClock ? this.clock;
		playQuant = quant ? this.quant;
		if(player.isNil) {
			player = EventStreamPlayer(this.asProtected.asStream, protoEvent);
			player.play(argClock, doReset, playQuant);
		} {
			// resets  when stream has ended or after pause/cmd-period:
			if(player.streamHasEnded or: { player.wasStopped }) { doReset = true };
			protoEvent !? { player.event = protoEvent };
			if(player.isPlaying.not) {
				player.play(argClock, doReset, playQuant);
			} {
				if(doReset) { player.reset };
			}
		}
	}


}

Pdef : EventPatternProxy {
	var <key;

	classvar <>all;

	storeArgs { ^[key] }

	*new { |key, item|
		var res = this.at(key);
		if(res.isNil) {
			res = super.new(item).prAdd(key);
		} {
			if(item.notNil) { res.source = item }
		}
		^res

	}

	map { |...args|
		if(envir.isNil) { this.envir = () };
		args.pairsDo { |key, name| envir.put(key, Pdefn(name)) }
	}

	prAdd { |argKey|
		key = argKey;
		all.put(argKey, this);
	}

	copy { |toKey|
		if(toKey.isNil or: { key == toKey }) { Error("can only copy to new key (key is %)".format(toKey)).throw };
		^this.class.new(toKey).copyState(this)
	}

	dup { |n = 2| ^{ this }.dup(n) } // avoid copy in Object::dup

	*hasGlobalDictionary { ^true }

	*initClass {
		var phraseEventFunc;

		all = IdentityDictionary.new;
		Class.initClassTree(Event);

		phraseEventFunc = {
			var pat, event, outerEvent, recursionLevel, instrument, embeddingLevel, freq, rest;

			embeddingLevel = ~embeddingLevel ? 0; // infinite recursion catch
			freq = ~freq.value;
			rest = freq.isKindOf(Symbol); // check for outer rests
			if(rest) { ~freq = freq };
			pat = (~repository ? all).at(~instrument);

			if(pat.notNil and: { embeddingLevel < 8 })
			{
				pat = pat.pattern; // optimization. outer pattern takes care for replacement
				// preserve information from outer pattern, but not delta.

				recursionLevel = ~recursionLevel;
				if(~transparency.isNil or:
					{ ~transparency > (recursionLevel ? 0) }
				) {
					outerEvent = currentEnvironment.copy
				} {
					outerEvent = Event.default;
					outerEvent.use {
						~type = \phrase;
						~recursionLevel = recursionLevel;
					}
				};

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
						~type = \note;
					};
				} {	// avoid recursion, if instrument not set.
					outerEvent.put(\embeddingLevel, embeddingLevel + 1);
					outerEvent.parent_(Event.parentEvents.default);
				};
				// maybe add a Pprotect here.
				// pat.asProtected
				pat = Pfindur(~sustain.value, pat);
				outerEvent.put(\delta, nil); // block delta modification by Ppar
				outerEvent.put(\instrument, ~synthDef ? \default);

				pat.play(thisThread.clock, outerEvent, 0.0);
			} {
				~type = \note;
				~play.value;
			}

		};

		Event.addEventType(\phrase, phraseEventFunc);
	}

}



PbindProxy : Pattern {
	var <>pairs, <source;

	*new { | ... pairs, kwargs |
		^super.newCopyArgs(pairs ++ kwargs).init
	}
	init {
		forBy(0, pairs.size-1, 2) { arg i;
			var proxy = PatternProxy.new;
			proxy.setSource(pairs[i+1]);
			pairs[i+1] = proxy
		};
		source = EventPatternProxy(Pbind(*pairs));

	}
	clear {
		pairs = [];
		source.clear;
	}
	embedInStream { arg inval;
		^source.embedInStream(inval)
	}
	find { |key|
		pairs.pairsDo { |u,x,i| if(u == key) { ^i } }; ^nil
	}
	quant_ { |val|
		pairs.pairsDo { |key, item| item.quant = val }; // maybe use ref later
		source.quant = val;
	}
	quant { ^source.quant }
	envir { ^source.envir }
	envir_ { arg envir; source.envir_(envir) }

	at { |key| var i; i = this.find(key); ^if(i.notNil) { pairs[i+1] } { nil } }

	set { |...args| // key, val ...
		var changedPairs = false, quant;
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
					pairs[i+1].quant_(quant).setSource(val)
				};
			}{
				pairs = pairs ++ [key, PatternProxy.new.quant_(quant).setSource(val)];
				changedPairs = true;
			};

		};
		if(changedPairs) { source.source =  Pbind(*pairs) };

	}

	storeArgs {
		var result = Array(pairs.size);
		pairs.pairsDo { |key, value|
			result.add(key).add(value.source)
		};
		^result
	}
}


Pbindef : Pdef {
	*new { | key ... pairs, kwargs |
		var pat, src;
		pairs = pairs ++ kwargs;
		pat = super.new(key);
		if(pairs.isEmpty.not) {
			src = pat.source;
			if(src.isKindOf(PbindProxy)) {
				src.set(*pairs);
				pat.wakeUp;
			} {
				if(src.isKindOf(Pbind)) {
					src = PbindProxy(*src.patternpairs.copy).set(*pairs);
				} {
					src = PbindProxy(*pairs);
				};
				pat.source = src;
			};
		};
		^pat
	}

	storeArgs { ^[key]++pattern.storeArgs }
	repositoryArgs { ^this.storeArgs }
	quant_ { |val| super.quant = val; source.quant = val }

	*hasGlobalDictionary { ^true }

}


// general purpose lookup stream

Pdict : Pattern {
	var <>dict, <>which, <>repeats, <>default;
	*new { arg dict, which, repeats=inf, default;
		^super.newCopyArgs(dict, which, repeats, default);
	}
	storeArgs { ^[dict,which,repeats,default ] }
	embedInStream { arg inval;
		var keyStream, key;
		keyStream = which.asStream;
		repeats.value(inval).do({
			key = keyStream.next(inval);
			if(key.isNil) { ^inval };
			inval = (dict.at(key) ? default).embedInStream(inval);
		});
		^inval
	}
}
