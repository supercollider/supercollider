Pattern : AbstractFunction {


	// concatenate Patterns
	++ { |aPattern|
		^Pseq.new([this, aPattern])
	}
	// compose Patterns
	<> { |aPattern|
		^Pchain(this, aPattern)
	}

	play { |clock, protoEvent, quant|
		^this.asEventStreamPlayer(protoEvent).play(clock, false, quant)
	}
	// phase causes pattern to start somewhere in the current measure rather than on a downbeat
	// offset allows pattern to compute ahead a bit to allow negative lags for strummed chords
	// and to ensure one pattern computes ahead of another

	asStream { ^Routine({ |inval| this.embedInStream(inval) }) }
	iter { ^this.asStream }
	streamArg { ^this.asStream }

	asEventStreamPlayer { |protoEvent|
		^EventStreamPlayer(this.asStream, protoEvent)
	}
	embedInStream { |inval|
		^this.asStream.embedInStream(inval)
	}
	do { |function|
		this.asStream.do(function)
	}

	// filtering operations
	collect { |function|
		^Pcollect.new(function, this)
	}
	select { |function|
		^Pselect.new(function, this)
	}
	reject { |function|
		^Preject.new(function, this)
	}

	// function composition
	composeUnaryOp { |operator|
		^Punop.new(operator, this)
	}
	composeBinaryOp { |operator, pattern, adverb|
		^Pbinop.new(operator, this, pattern, adverb)
	}
	reverseComposeBinaryOp { |operator, pattern, adverb|
		^Pbinop.new(operator, pattern, this, adverb)
	}
	composeNAryOp { |selector, argList|
		^Pnaryop.new(selector, this, argList)
	}

	//////////////////////

	mtranspose { |n| ^Paddp(\mtranspose, n, this) }
	ctranspose { |n| ^Paddp(\ctranspose, n, this) }
	gtranspose { |n| ^Paddp(\gtranspose, n, this) }
	detune { |n| ^Paddp(\detune, n, this) }

	scaleDur { |x| ^Pmulp(\dur, x, this) }
	addDur { |x| ^Paddp(\dur, x, this) }
	stretch { |x| ^Pmulp(\stretch, x, this) }
	lag { |t| ^Plag(t, this) }

	legato { |x| ^Pmulp(\legato, x, this) }

	db { |db| ^Paddp(\db, db, this) }

	clump { |n| ^Pclump(n, this) }
	flatten { |n = 1| ^Pflatten(n, this) }
	repeat { |n = inf| ^Pn(this, n) }
	keep { |n| ^Pfin(n, this) }
	drop { |n| ^Pdrop(n, this) }
	stutter { |n|
		^this.dupEach(n)
	}
	dupEach { |n| ^Pdup(n, this) }
	finDur { |dur, tolerance = 0.001| ^Pfindur(dur, this, tolerance) }
	fin { |n = 1| ^Pfin(n, this) }

	trace { |key, printStream, prefix = ""| ^Ptrace(this, key, printStream, prefix) }
	differentiate { ^Pdiff(this) }
	integrate { ^Plazy { var sum = 0; this.collect { |x| sum = sum + x } } }


	// realtime recording
	// for NRT see Pattern:asScore

	// path: if nil, auto-generate path
	// dur: if nil, record until pattern stops or is stopped externally
	// fadeTime: allow extra time after last Event for nodes to become silent

	record { |path, headerFormat, sampleFormat, numChannels = 2, dur = nil, fadeTime = 0.2, clock = (TempoClock.default), protoEvent = (Event.default), server = (Server.default), out = 0, outNumChannels|

		var buffer, nodeID, defname;
		var pattern = if(dur.notNil) { Pfindur(dur, this) } { this };

		headerFormat = headerFormat ?? { server.recHeaderFormat };
		sampleFormat = sampleFormat ?? { server.recSampleFormat };

		server.waitForBoot {
			var group, bus, free, monitor;

			buffer = Buffer.alloc(server, 32768, numChannels);
			buffer.write(path, headerFormat, sampleFormat, 0, leaveOpen: true);
			defname = SystemSynthDefs.generateTempName;
			SynthDef(defname, { |bufnum, bus|
				var sig = In.ar(bus, numChannels);
				DiskOut.ar(bufnum, sig)
			}).add;

			fadeTime = (fadeTime ? 0).roundUp(buffer.duration);

			bus = Bus.audio(server, numChannels);
			group = Group(server);
			Monitor.new.play(bus.index, bus.numChannels, out, outNumChannels ? numChannels, group);
			server.sync;

			free = {
				(type: \off, id: nodeID, server: server, hasGate: false).play;
				{
					bus.free; group.free;
					buffer.free;
					server.sendMsg(\d_free, defname)
				}.defer(server.latency)
			};

			Pprotect(
				Pfset(nil,
					Pseq([
						(
							type: \on, id: nodeID, instrument: defname,
							bus: bus, group: group, addAction: \addAfter,
							bufnum: buffer,
							delta: 0,
							callback: { nodeID = ~id }
						),
						pattern <> (out: bus),
						(type: \rest, delta: fadeTime)
					], 1),
					free
				),
				free // on error
			).play(clock, protoEvent, quant: 0)

		}
	}
}

Pfunc : Pattern {
	var <>nextFunc; // Func is evaluated for each next state
	var <>resetFunc; // Func is evaluated for each next state
	*new { |nextFunc, resetFunc|
		^super.newCopyArgs(nextFunc, resetFunc)
	}
	storeArgs { ^[nextFunc] ++ resetFunc }
	asStream {
		^FuncStream.new(nextFunc, resetFunc)
	}
}

Prout : Pattern {
	var <>routineFunc;
	*new { |routineFunc|
		^super.newCopyArgs(routineFunc)
	}
	storeArgs { ^[routineFunc] }
	asStream {
		^Routine.new(routineFunc)
	}
	embedInStream { |inval| ^routineFunc.value(inval) }
}

Pfuncn : Pattern {
	var <>func, <>repeats;
	*new { |func, repeats = 1|
		^super.newCopyArgs(func, repeats)
	}
	storeArgs { ^[func, repeats] }
	embedInStream { |inval|
		repeats.value(inval).do {
			inval = func.value(inval).yield
		};
		^inval
	}
}


// Punop and Pbinop are used to create patterns in response to math operations
Punop : Pattern {
	var <>operator, <>a;
	*new { |operator, a|
		^super.newCopyArgs(operator, a)
	}

	storeOn { |stream| stream <<< a << "." << operator }

	embedInStream { |inval|
		var stream, outval;
		stream = a.asStream;
		loop {
			outval = stream.next(inval);
			if(outval.isNil) { ^inval };
			inval = yield(outval.perform(operator))
		}
	}

	asStream {
		^UnaryOpStream.new(operator, a.asStream)
	}
}

Pbinop : Pattern {
	var <>operator, <>a, <>b, <>adverb;
	*new { |operator, a, b, adverb|
		^super.newCopyArgs(operator, a, b, adverb)
	}

	storeOn { |stream|
		stream << "(" <<< a << " " << operator.asBinOpString;
		if(adverb.notNil) { stream << "." << adverb };
		stream << " " <<< b << ")"
	}

	asStream {
		if(adverb.isNil) {
			^BinaryOpStream.new(operator, a.asStream, b.asStream)
		};
		if(adverb == 'x') {
			^BinaryOpXStream.new(operator, a.asStream, b.asStream)
		};
		^nil
	}
}

Pnaryop : Pattern {
	var <>operator, <>a, <>arglist;
	*new { |operator, a, arglist|
		^super.newCopyArgs(operator, a, arglist)
	}
	storeOn { |stream| stream <<< a << "." << operator << "(" <<<* arglist << ")" }

	embedInStream { |inval|
		var streamA, streamlist, vala, values, isNumeric;
		streamA = a.asStream;
		// optimization
		isNumeric = arglist.every { |item|
			item.isNumber or: { item.class === Symbol }
		};

		if(isNumeric) {
			loop {
				vala = streamA.next(inval);
				if(vala.isNil) { ^inval };
				inval = yield(vala.performList(operator, arglist))
			}
		} {
			streamlist = arglist.collect { |item| item.asStream };
			loop {
				vala = streamA.next(inval);
				if(vala.isNil) { ^inval };
				values = streamlist.collect { |item|
					var result = item.next(inval);
					if(result.isNil) { ^inval };
					result
				};
				inval = yield(vala.performList(operator, values))
			}
		}
	}

	asStream {
		^NAryOpStream.new(operator, a.asStream, arglist.collect { |item| item.asStream })
	}
}

PdegreeToKey : Pnaryop {
	*new { |pattern, scale, stepsPerOctave = 12|
		^super.new('degreeToKey', pattern, [scale, stepsPerOctave])
	}
	// this is not reversible
	// but it would save as something that played the same
	//storeArgs { ^[ pattern, scale, stepsPerOctave ] }
}

Pchain : Pattern {
	var <>patterns;
	*new { |... patterns|
		^super.newCopyArgs(patterns)
	}
	<> { |aPattern|
		var list;
		list = patterns.copy.add(aPattern);
		^this.class.new(*list)
	}
	embedInStream { |inval|
		var streams, inevent, cleanup = EventStreamCleanup.new;
		streams = patterns.collect(_.asStream);
		loop {
			inevent = inval.copy;
			streams.reverseDo { |str|
				inevent = str.next(inevent);
				if(inevent.isNil) { ^cleanup.exit(inval) }
			};
			cleanup.update(inevent);
			inval = yield(inevent)
		}
	}
	storeOn { |stream|
		stream << "(";
		patterns.do { |item, i| if(i != 0) { stream << " <> " }; stream <<< item };
		stream << ")"
	}
}


Pevent : Pattern {
	var <>pattern, <>event;

	*new { |pattern, event|
		^super.newCopyArgs(pattern, event ?? { Event.default })
	}
	storeArgs { ^[pattern, event] }
	embedInStream { |inval|
		var outval;
		var stream = pattern.asStream;
		loop {
			outval = stream.next(event);
			if(outval.isNil) { ^inval };
			inval = outval.yield
		}
	}
}


Pbind : Pattern {
	var <>patternpairs;
	*new { |... pairs, kwargs|
		if(pairs.size.odd) { Error("Pbind should have even number of args.\n").throw };
		^super.newCopyArgs(pairs ++ kwargs)
	}

	storeArgs { ^patternpairs }
	embedInStream { |inevent|
		var event;
		var sawNil = false;
		var streampairs = patternpairs.copy;
		var endval = streampairs.size - 1;

		forBy(1, endval, 2) { |i|
			streampairs.put(i, streampairs[i].asStream)
		};

		loop {
			if(inevent.isNil) { ^nil.yield };
			event = inevent.copy;
			forBy(0, endval, 2) { |i|
				var name = streampairs[i];
				var stream = streampairs[i + 1];
				var streamout = stream.next(event);
				if(streamout.isNil) { ^inevent };

				if(name.isSequenceableCollection) {
					if(name.size > streamout.size) {
						("the pattern is not providing enough values to assign to the key set:" + name).warn;
						^inevent
					};
					name.do { |key, i|
						event.put(key, streamout[i])
					}
				} {
					event.put(name, streamout)
				}

			};
			inevent = event.yield
		}
	}
}

Pmono : Pattern {
	var <>synthName, <>patternpairs;
	*new { |name ... pairs|
		if(pairs.size.odd) { Error("Pmono should have odd number of args.\n").throw };
		^super.newCopyArgs(name.asSymbol, pairs)
	}

	embedInStream { |inevent|
		^PmonoStream(this).embedInStream(inevent)
	}
}

PmonoArtic : Pmono {
	embedInStream { |inevent|
		^PmonoArticStream(this).embedInStream(inevent)
	}
}


Pseries : Pattern {	// arithmetic series
	var <>start = 0, <>step = 1, <>length = inf;
	*new { |start = 0, step = 1, length = inf|
		^super.newCopyArgs(start, step, length)
	}
	storeArgs { ^[start, step, length] }

	embedInStream { |inval|
		var outval, counter = 0;
		var cur = start.value(inval);
		var len = length.value(inval);
		var stepStr = step.asStream, stepVal;
		while { counter < len } {
			stepVal = stepStr.next(inval);
			if(stepVal.isNil) { ^inval };
			outval = cur;
			cur = cur + stepVal;
			counter = counter + 1;
			inval = outval.yield
		};
		^inval
	}
}

Pgeom : Pattern {	// geometric series
	var <>start = 1.0, <>grow = 1.0, <>length = inf;
	*new { |start = 0, grow = 1, length = inf|
		^super.newCopyArgs(start, grow, length)
	}
	storeArgs { ^[start, grow, length] }
	embedInStream { |inval|
		var outval, counter = 0;
		var cur = start.value(inval);
		var len = length.value(inval);
		var growStr = grow.asStream, growVal;
		while { counter < len } {
			growVal = growStr.next(inval);
			if(growVal.isNil) { ^inval };
			outval = cur;
			cur = cur * growVal;
			counter = counter + 1;
			inval = outval.yield
		};
		^inval
	}
}


Pbrown : Pattern {
	var <>lo, <>hi, <>step, <>length;

	*new { |lo = 0.0, hi = 1.0, step = 0.125, length = inf|
		^super.newCopyArgs(lo, hi, step, length)
	}

	storeArgs { ^[lo, hi, step, length] }

	embedInStream { |inval|
		var cur;
		var loStr = lo.asStream, loVal;
		var hiStr = hi.asStream, hiVal;
		var stepStr = step.asStream, stepVal;

		loVal = loStr.next(inval);
		hiVal = hiStr.next(inval);
		stepVal = stepStr.next(inval);
		cur = rrand(loVal, hiVal);

		if(loVal.isNil or: { hiVal.isNil } or: { stepVal.isNil }) { ^inval };

		length.value(inval).do {
			loVal = loStr.next(inval);
			hiVal = hiStr.next(inval);
			stepVal = stepStr.next(inval);
			if(loVal.isNil or: { hiVal.isNil } or: { stepVal.isNil }) { ^inval };
			cur = this.calcNext(cur, stepVal).fold(loVal, hiVal);
			inval = cur.yield
		};

		^inval
	}

	calcNext { |cur, step|
		^cur + step.xrand2
	}
}

Pgbrown : Pbrown {
	calcNext { |cur, step|
		^cur * (1 + step.xrand2)
	}
}

Pwhite : Pattern {
	var <>lo, <>hi, <>length;
	*new { |lo = 0.0, hi = 1.0, length = inf|
		^super.newCopyArgs(lo, hi, length)
	}
	storeArgs { ^[lo, hi, length] }
	embedInStream { |inval|
		var loStr = lo.asStream;
		var hiStr = hi.asStream;
		var hiVal, loVal;
		length.value(inval).do {
			hiVal = hiStr.next(inval);
			loVal = loStr.next(inval);
			if(hiVal.isNil or: { loVal.isNil }) { ^inval };
			inval = rrand(loVal, hiVal).yield
		};
		^inval
	}
}

Pprob : Pattern {
	var <>hi, <>lo, <>length, <tableSize, <distribution, <table;

	*new { |distribution, lo = 0.0, hi = 1.0, length = inf, tableSize|
		^super.newCopyArgs(hi, lo, length, tableSize).distribution_(distribution)
	}
	distribution_ { |list|
		var n = tableSize ?? { max(64, list.size) }; // resample, if too small
		distribution = list;
		table = distribution.asRandomTable(n)
	}
	tableSize_ { |n|
		tableSize = n;
		this.distribution_(distribution)
	}
	embedInStream { |inval|
		var loStr = lo.asStream;
		var hiStr = hi.asStream;
		var hiVal, loVal;

		length.value(inval).do {
			loVal = loStr.next(inval);
			hiVal = hiStr.next(inval);
			if(hiVal.isNil or: { loVal.isNil }) { ^inval };
			inval = ((table.tableRand * (hiVal - loVal)) + loVal).yield
		};
		^inval
	}
}

Pstep2add : Pattern {
	var <>pattern1, <>pattern2;
	*new { |pattern1, pattern2|
		^super.newCopyArgs(pattern1, pattern2)
	}
	storeArgs { ^[pattern1, pattern2] }
	embedInStream { |inval|
		var stream1, stream2, val1, val2;

		stream1 = pattern1.asStream;
		while {
			(val1 = stream1.next(inval)).notNil
		} {
			stream2 = pattern2.asStream;
			while {
				(val2 = stream2.next(inval)).notNil
			} {
				inval = yield(val1 + val2)
			}
		};
		^inval
	}
}

Pstep3add : Pattern {
	var <>pattern1, <>pattern2, <>pattern3;
	*new { |pattern1, pattern2, pattern3|
		^super.newCopyArgs(pattern1, pattern2, pattern3)
	}
	storeArgs { ^[pattern1, pattern2, pattern3] }

	embedInStream { |inval|
		var stream1, stream2, stream3, val1, val2, val3;

		stream1 = pattern1.asStream;
		while {
			(val1 = stream1.next(inval)).notNil
		} {
			stream2 = pattern2.asStream;
			while {
				(val2 = stream2.next(inval)).notNil
			} {
				stream3 = pattern3.asStream;
				while {
					(val3 = stream3.next(inval)).notNil
				} {
					inval = yield(val1 + val2 + val3)
				}
			}
		};
		^inval
	}
}


PstepNfunc : Pattern {
	var <function, <>patterns;
	*new { |function, patterns|
		^super.newCopyArgs(function ? { |x| x }, patterns)
	}
	storeArgs { ^[function, patterns] }
	embedInStream { |inval|
		var val;
		var size = patterns.size;
		var max = size - 1;
		var streams = Array.newClear(size);
		var vals = Array.newClear(size);
		// this variable is needed because of recursion
		var f = { |inval, level = 0|
			var val;
			streams[level] = patterns[level].asStream;
			while {
				vals[level] = val = streams[level].next(inval);
				val.notNil
			} {
				if(level < max) {
					inval = f.value(inval, level + 1)
				} {
					inval = yield(function.value(vals))
				}
			};
			inval
		};
		^f.value(inval)
	}

}

PstepNadd : PstepNfunc {
	*new { |... patterns|
		^super.new({ |vals| vals.sum }, patterns)
	}
	storeArgs { ^patterns }
}

// returns relative time (in beats) from moment of embedding

Ptime : Pattern {
	var <>repeats;
	*new { |repeats = inf|
		^super.newCopyArgs(repeats)
	}
	storeArgs { ^[repeats] }
	embedInStream { |inval|
		var start = thisThread.beats;
		repeats.value(inval).do { inval = (thisThread.beats - start).yield };
		^inval
	}
}

// if an error is thrown in the stream, func is evaluated

Pprotect : FilterPattern {
	var <>func;
	*new { |pattern, func|
		^super.new(pattern).func_(func)
	}
	storeArgs { ^[pattern, func] }

	embedInStream { |inval|
		var result = prTry { pattern.embedInStream(inval) };
		if(result.isException) {
			func.value(result, thisThread);
			result.throw  // to nested error handlers
		} {
			// here, `protect` calls the error handler
			// but Pprotect has a slightly different meaning
			^result
		}
	}
}



// access a key from the input event
Pkey : Pattern {
	var	<>key, <>repeats;

	*new { |key, repeats|
		^super.newCopyArgs(key, repeats)
	}

	storeArgs { ^[key, repeats] }

	asStream {
		var	keystream = key.asStream;
		// avoid creating a routine
		var stream = FuncStream({ |inevent| inevent !? { inevent[keystream.next(inevent)] } });
		^if(repeats.isNil) { stream } { stream.fin(repeats) }
	}

	embedInStream { |inval|
		var outval, keystream = key.asStream;
		(repeats.value(inval) ?? { inf }).do {
			outval = inval[keystream.next(inval)];
			if(outval.isNil) { ^inval };
			inval = outval.yield
		};
		^inval
	}
}

Pif : Pattern {
	var	<>condition, <>iftrue, <>iffalse, <>default;
	*new { |condition, iftrue, iffalse, default|
		^super.newCopyArgs(condition, iftrue, iffalse, default)
	}
	storeArgs { ^[condition, iftrue, iffalse, default] }
	asStream {
		var	condStream = condition.asStream,
			trueStream = iftrue.asStream,
			falseStream = iffalse.asStream;

		^FuncStream({ |inval|
			var test;
			if((test = condStream.next(inval)).isNil) {
				nil
			} {
				if(test) {
					trueStream.next(inval) ? default
				} {
					falseStream.next(inval) ? default
				}
			}
		}, {		// reset func
			condStream.reset;
			trueStream.reset;
			falseStream.reset
		})
	}
}
