// contains numerical / value patterns

PatternProxy : Pattern {

	var <source, <pattern, <>envir;
	var >clock, quant, <>condition=true, reset;

	// quant value for new pattern insertio can be an array, like [quant, phase, timingOffset]
	// in EventPatternProxy it can be [quant, phase, timingOffset, onset]

	classvar <>defaultQuant, defaultEnvir;

	*new { arg source;
		^super.new.source_(source)
	}

	*default { ^1 } // safe for duration patterns
	*defaultValue { ^1 }

	clock { ^clock ? TempoClock.default }

	quant { ^quant ??  { this.class.defaultQuant } }
	quant_ { arg val; quant = val }

	source_ { arg obj;
		var pat = if(obj.isKindOf(Function)) { this.convertFunction(obj) }{ obj };
		if (obj.isNil) { pat = this.class.default };
		pattern = pat;
		source = obj; // keep original here.
		this.changed(\source, obj);
	}

	setSource { arg obj;
		pattern = obj;
		source = obj; // keep original here.
		this.changed(\source, obj);
	}

	count { arg n=1;
		condition = { |val,i| i % n == 0 }
	}

	reset { reset = reset ? 0 + 1 }

	sched { arg func;
		if(quant.isNil)
		{ func.value }
		{ this.clock.schedAbs(quant.nextTimeOnGrid(this.clock), { func.value; nil }) }
	}


	convertFunction { arg func;
		^Prout {
			var inval = func.def.prototypeFrame !? { inval = this.defaultEvent };
			func.value( inval ).embedInStream(inval)
		}
	}

	*parallelise { arg list; ^Ptuple(list) }

	pattern_ { arg pat; this.source_(pat) }

	timingOffset_ { arg val; quant = quant.instill(2, val) }
	timingOffset { arg val; ^quant.obtain(2) }
	phase_ { arg val; quant = quant.instill(1, val) }
	phase { arg val; ^quant.obtain(1) }
	quantBeat_ { arg val; quant = quant.instill(0, val) }
	quantBeat { arg val; ^quant.obtain(0) }

	set { arg ... args;
		if(envir.isNil) { this.envir = this.class.event };
		args.pairsDo { arg key, val; envir.put(key, val) };
		this.changed(\set, args);
	}

	unset { arg ... args;
		if(envir.notNil) {
			args.do { arg key; envir.removeAt(key) };
			this.changed(\unset, args);
		}
	}

	get { arg key;
		^if(envir.notNil) { envir[key] } { nil };
	}

	place { arg ... args;
		if(envir.isNil) { this.envir = this.class.event };
		args.pairsDo { arg key, val; envir.place(key, val) }; // place is defined in the event.
	}

	isEventPattern { ^false }

	receiveEvent { ^nil }

	embed { |val|
		^if(val.notNil) { Pchain(this, val) } { this }.embedInStream
	}

	embedInStream { arg inval, embed = true, default;

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

	endless { arg default;
		^Prout { arg inval;
			this.embedInStream(inval, true, default ? this.class.defaultValue)
		}
	}

	constrainStream { arg stream, newStream;
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
		this.all.do { arg pat; pat.clear }
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
	*basicNew { arg source;
		^super.new.init(source)
	}

	// global storage

	*at { arg key;
		^if(this.hasGlobalDictionary) { this.all.at(key) } { nil }
	}

	repositoryArgs { ^[this.key, this.source] }

	*postRepository { arg keys, stream;
		if(this.hasGlobalDictionary.not) { Error("This class has no global repository.").throw };
		keys = keys ?? { this.all.keys };
		stream = stream ? Post;
		keys.do { arg key;
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

	*event { arg proxyClass = PatternProxy;
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


// contains time patterns (tasks)

TaskProxy : PatternProxy {
	var <player, <>playQuant;
	classvar <>defaultQuant=1.0;

	storeArgs { ^[source] }

	source_ { arg obj;
		pattern = if(obj.isKindOf(Function)) { this.convertFunction(obj) }{ obj };
		if (obj.isNil) { pattern = this.class.default; source = obj; };
		this.wakeUp;
		source = obj;
		this.changed(\source, obj);
	}

	convertFunction { arg func;
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

	constrainStream { arg stream, newStream;
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

	playOnce { arg argClock, doReset = (false), quant;
		var clock = argClock ? this.clock;
		^PauseStream.new(this.asProtected.asStream).play(clock, doReset, quant ? this.quant)
	}

	play { arg argClock, doReset=false, quant;
		playQuant = quant ? this.quant;
		if(player.isNil) {
			player = this.playOnce(argClock, doReset, playQuant);
		} {
			// resets  when stream has ended or after pause/cmd-period:
			if (player.streamHasEnded or: { player.wasStopped }) { doReset = true };

			if(player.isPlaying.not) {
				player.play(argClock, doReset, playQuant);
			} {
				if (doReset) { player.reset };
			}
		}
	}
	wakeUp {
		if(this.isPlaying) { this.play(quant:playQuant) }	}
	asProtected {
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


// contains event patterns

EventPatternProxy : TaskProxy {
	var <>fadeTime;
	classvar <>defaultQuant=1.0;

	storeArgs { ^[source] }

	source_ { arg obj;
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

	embedInStream { arg inval, embed = true, default;

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
		var delta, tolerance;
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
							Pfindur(delta, stream, tolerance).asStream,
							cleanup
						),
						newStream
					]).asStream
				}
			}{
				Ppar([
					EmbedOnce(
						PfadeOut(stream, fadeTime, delta, tolerance),
						cleanup
					),
					PfadeIn(newStream, fadeTime, delta, tolerance)
				]).asStream
			}
		}
	}

	*parallelise { arg list; ^Ppar(list) }

	outset_ { arg val; quant = quant.instill(3, val) }
	outset { arg val; ^this.quant.obtain(3) }


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

