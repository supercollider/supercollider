FilterPattern : Pattern {
	var <>pattern;

	*new { arg pattern;
		^super.new.pattern_(pattern)
	}
}


Pn : FilterPattern {
	var <>repeats, <>key;
	*new { arg pattern, repeats=inf, key;
		^super.newCopyArgs(pattern, repeats, key )
	}
	storeArgs { ^[pattern,repeats, key] }
	embedInStream { | event |
		if(key.isNil) {
			repeats.value(event).do { event = pattern.embedInStream(event) };
		} {
			repeats.value(event).do {
				event = pattern.embedInStream(event);
				event[key] = true;
			};
			event[key] = false;
		};
		^event;
	}
}

Pgate  : Pn {
	*new { arg pattern, repeats=inf,  key	;
		^super.new(pattern).repeats_(repeats).key_(key)
	}
	storeArgs { ^[pattern,repeats, key] }
	embedInStream { | event |
		var stream, output;
		repeats.do {
			stream = pattern.asStream;
			output = stream.next(event);
			while {
				if (event[key] == true) { output = stream.next(event) };
				output.notNil;
			} {
				event = output.copy.embedInStream(event)
			}
		};
		^event;
	}
}

FuncFilterPattern : FilterPattern {
	var <>func;

	*new { arg func, pattern;
		^super.new(pattern).func_(func)
	}
	storeArgs { ^[func,pattern] }
}

Pcollect : FuncFilterPattern {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			outval = stream.next(inval);
			if (outval.isNil) { ^inval };
			inval = yield(func.value(outval, inval));
		}
	}
	asStream {
		^pattern.asStream.collect(func);
	}
}

Pselect : FuncFilterPattern {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			while ({
				outval = stream.next(inval);
				if (outval.isNil) { ^inval };
				func.value(outval, inval).not
			});
			inval = yield(outval);
		}
	}
	asStream {
		^pattern.asStream.select(func);
	}
}

Preject : FuncFilterPattern {
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop {
			while ({
				outval = stream.next(inval);
				if (outval.isNil) { ^inval };
				func.value(outval, inval);
			});
			inval = yield(outval);
		}
	}
	asStream {
		^pattern.asStream.reject(func);
	}
}

Pfset : FuncFilterPattern {
	var	<>cleanupFunc;
	*new { |func, pattern, cleanupFunc|
		^super.new(func, pattern).cleanupFunc_(cleanupFunc)
	}
	embedInStream { arg inevent;
		var event, cleanup = EventStreamCleanup.new;
		// cleanup should actually not be passed in
		// but retaining (temporarily) for backward compatibility
		var envir = Event.make({ func.value(cleanup) });
		var stream = pattern.asStream;
		var once = true;

		loop {
			inevent.putAll(envir);
			event = stream.next(inevent);
			if(once) {
				cleanup.addFunction(event, { |flag|
					envir.use({ cleanupFunc.value(flag) });
				});
				once = false;
			};
			if (event.isNil) {
				^cleanup.exit(inevent)
			} {
				cleanup.update(event);
			};
			inevent = yield(event);
			if(inevent.isNil) { ^cleanup.exit(event) }
		};
	}
}


Psetpre : FilterPattern {
	var <>name, <>value;
	*new { arg name, value, pattern;
		^super.new(pattern).name_(name).value_(value)
	}
	storeArgs { ^[name,value,pattern] }
	filterEvent { arg event, val;
		^event[name] = val;
	}
	embedInStream { arg event;
		var cleanup = EventStreamCleanup.new;
		var val, inevent, filteredEvent;
		var valStream = value.asStream;
		var evtStream = pattern.asStream;

		loop {
			val = valStream.next(event);
			if (val.isNil or: event.isNil) {
				^cleanup.exit(event)
			}{
				event = event.copy;
				filteredEvent = this.filterEvent(event, val);
			};
			inevent = evtStream.next(filteredEvent);
			if (inevent.isNil) { ^cleanup.exit(event) };
			cleanup.update(inevent);
			event = yield(inevent);
			// if(event.isNil) { nil.yield; ^inevent }
		}
	}
}

Paddpre : Psetpre {
	filterEvent { arg event, val;
		^event[name] = event[name] + val;
	}
}

Pmulpre : Psetpre {
	filterEvent { arg event, val;
		^event[name] = event[name] * val;
	}
}

Pset : FilterPattern {
	var <>name, <>value;
	*new { arg name, value, pattern;
		^super.new(pattern).name_(name).value_(value)
	}
	storeArgs { ^[name,value,pattern] }
	filterEvent { arg event, val;
		^event[name] = val;
	}
	embedInStream { arg event;
		var cleanup = EventStreamCleanup.new;
		var val, inEvent;
		var valStream = value.asStream;
		var evtStream = pattern.asStream;

		loop {
			inEvent = evtStream.next(event);
			// if (event.isNil) { ^nil.yield };
			if (inEvent.isNil) { ^cleanup.exit(event) };
			val = valStream.next(inEvent);
			if (val.isNil) { ^cleanup.exit(event) };

			this.filterEvent(inEvent, val);
			cleanup.update(inEvent);
			event = inEvent.yield;
		}
	}
}


Padd : Pset {
	filterEvent { arg event, val;
		^event[name] = event[name] + val;
	}
}

Pmul : Pset {
	filterEvent { arg event, val;
		^event[name] = event[name] * val;
	}
}


Psetp : Pset {
	embedInStream { arg event;
		var evtStream, val, inevent;
		var valStream = value.iter;

		while {
			val = valStream.next(event);
			val.notNil
		}{
			evtStream = pattern.asStream;
			while {
				inevent = evtStream.next(event);
				if(event.isNil) { ^nil.yield };
				inevent.notNil;
			} {
				this.filterEvent(inevent, val);
				event = inevent.yield;
			};
		};
		^event;
	}
}

Paddp : Psetp {
	filterEvent { arg event, val;
		^event[name] = event[name] + val;
	}
}

Pmulp : Psetp {
	filterEvent { arg event, val;
		^event[name] = event[name] * val;
	}
}


Pstretch : FilterPattern {
	var <>value;
	*new { arg value, pattern;
		^super.new(pattern).value_(value)
	}
	storeArgs { ^[value,pattern] }
	embedInStream {  arg event;
		var cleanup = EventStreamCleanup.new;
		var inevent;
		var val, delta;
		var valStream = value.asStream;
		var evtStream = pattern.asStream;

		loop {
			inevent = evtStream.next(event).asEvent;
			if (inevent.isNil) { ^cleanup.exit(event) };
			val = valStream.next(inevent);
			if (val.isNil) { ^cleanup.exit(event) };

			delta = event[\delta];
			if (delta.notNil) {
				inevent[\delta] = delta * val;
			};
			inevent[\dur] = inevent[\dur] * val;
			cleanup.update(inevent);
			event = yield(inevent);
		}
	}
}



Pstretchp : Pstretch {

	embedInStream { arg event;
		var evtStream, val, inevent, delta;
		var valStream = value.asStream;
		while {
			val = valStream.next(event).asEvent;
			val.notNil
		} {
			evtStream = pattern.asStream;
			while {
				inevent = evtStream.next(event);
				// if(event.isNil) { ^nil.yield };
				inevent.notNil
			} {
				delta = inevent[\delta];
				if (delta.notNil) {
					inevent[\delta] = delta * val;
				};
				inevent[\dur] = inevent[\dur] * val;
				event = inevent.yield;
			};
		};
		^event;
	}
}


// needs testing - hjh
Pplayer : FilterPattern {
	var <>subPattern;
	*new { arg playerPattern, subPattern;
		^super.newCopyArgs(playerPattern, subPattern)
	}
	storeArgs { ^[ pattern, subPattern ] }
	embedInStream { arg event;
		var player, inevent;
		var playerStream = pattern.asStream;
		var stream = subPattern.asStream;
		loop{
			inevent = stream.next(event);
			if (inevent.isNil) { ^event };
			player = playerStream.next(event);
			if (player.isNil) { ^event };
			inevent.parent = player.event;
			event = yield(inevent);
		}
	}
	// backward compatibility: unnecessary var playerPattern was removed
	playerPattern { ^pattern }
	playerPattern_ { |playerPattern| pattern = playerPattern }
}



Pdrop : FilterPattern {
	var <>count;
	*new { arg count, pattern;
		^super.new(pattern).count_(count)
	}
	storeArgs { ^[count,pattern] }
	embedInStream { arg event;
		var inevent;
		var stream = pattern.asStream;

		count.value(event).do {
			inevent = stream.next(event);
			if (inevent.isNil, { ^event });
		};
		loop {
			inevent = stream.next(event);
			if (inevent.isNil, { ^event });
			event = inevent.yield;
		};
	}
}

Pfin : FilterPattern {
	var <>count;
	*new { arg count, pattern;
		^super.new(pattern).count_(count)
	}
	storeArgs { ^[count,pattern] }
	asStream { | cleanup| ^Routine({ arg inval; this.embedInStream(inval, cleanup) }) }

	embedInStream { arg event, cleanup;
		var inevent;
		var stream = pattern.asStream;
		cleanup ?? { cleanup = EventStreamCleanup.new };
		count.value(event).do({
			inevent = stream.next(event) ?? { ^event };
			cleanup.update(inevent);
			event = inevent.yield;
		});
		^cleanup.exit(event)
	}
}



// it is not correct to call stream.next(nil) on a value stream
// but there is no good way to distinguish in Pfin so we need a subclass
// might be ok to deprecate this now

Pfinval : Pfin {
	embedInStream { arg event;
		var inevent;
		var stream = pattern.asStream;

		count.value(event).do({
			inevent = stream.next(event);
			if (inevent.isNil, { ^event });
			event = inevent.yield;
		});
		^event
	}
}

Pfindur : FilterPattern {
	var <>dur, <>tolerance;
	*new { arg dur, pattern, tolerance = 0.001;
		^super.new(pattern).dur_(dur).tolerance_(tolerance)
	}
	storeArgs { ^[dur,pattern,tolerance] }
	asStream { | cleanup| ^Routine({ arg inval; this.embedInStream(inval, cleanup) }) }

	embedInStream { arg event, cleanup;
		var item, delta, elapsed = 0.0, nextElapsed, inevent,
		localdur = dur.value(event);
		var stream = pattern.asStream;

		cleanup ?? { cleanup = EventStreamCleanup.new };
		loop {
			inevent = stream.next(event).asEvent ?? { ^event };
			cleanup.update(inevent);
			delta = inevent.delta;
			nextElapsed = elapsed + delta;
			if (nextElapsed.roundUp(tolerance) >= localdur) {
				// must always copy an event before altering it.
				// fix delta time and yield to play the event.
				inevent = inevent.copy.put(\delta, localdur - elapsed).yield;
				^cleanup.exit(inevent);
			};

			elapsed = nextElapsed;
			event = inevent.yield;

		}
	}
}

Psync : FilterPattern {
	var <>quant, <>maxdur, <>tolerance;
	*new { arg pattern, quant, maxdur, tolerance = 0.001;
		^super.new(pattern).quant_(quant).maxdur_(maxdur).tolerance_(tolerance)
	}
	storeArgs { ^[pattern,quant,maxdur,tolerance] }

	embedInStream { arg event;
		var item, stream, delta, elapsed = 0.0, nextElapsed, clock, inevent;
		var	localquant = quant.value(event), localmaxdur = maxdur.value(event);
		var cleanup = EventStreamCleanup.new;

		stream = pattern.asStream;

		loop {
			inevent = stream.next(event).asEvent;
			if(inevent.isNil) {
				if(localquant.notNil) {
					delta = elapsed.roundUp(localquant) - elapsed;
					if(delta > 0) { Event.silent(delta, event).yield };
					^cleanup.exit(event);
				};
			};
			cleanup.update(inevent);

			delta = inevent.delta;
			nextElapsed = elapsed + delta;

			if (localmaxdur.notNil and: { nextElapsed.round(tolerance) >= localmaxdur }) {
				inevent = inevent.copy;
				inevent.put(\delta, localmaxdur - elapsed);
				event = inevent.yield;
				^cleanup.exit(event);
			} {
				elapsed = nextElapsed;
				event = inevent.yield;
			};
		};
	}
}


Pconst : FilterPattern {
	var <>sum, <>tolerance;
	*new { arg sum, pattern, tolerance=0.001;
		^super.new(pattern).sum_(sum).tolerance_(tolerance)
	}
	storeArgs { ^[sum,pattern,tolerance] }

	embedInStream { arg inval;
		var delta, elapsed = 0.0, nextElapsed, str=pattern.asStream,
		localSum = sum.value(inval);
		loop ({
			delta = str.next(inval);
			if(delta.isNil) {
				inval = (localSum - elapsed).yield;
				^inval
			};
			nextElapsed = elapsed + delta;
			if (nextElapsed.round(tolerance) >= localSum) {
				inval = (localSum - elapsed).yield;
				^inval
			}{
				elapsed = nextElapsed;
				inval = delta.yield;
			};
		});
	}
}

Plag : FilterPattern {
	var <>lag;
	*new { arg lag, pattern;
		^super.new(pattern).lag_(lag)
	}
	storeArgs { ^[lag,pattern] }
	embedInStream { arg event;
		var item;
		var stream = pattern.asStream;
		var inevent = event.copy;

		event = Event.silent(lag.value(event), event).yield;

		loop {
			inevent = stream.next(event);
			if (inevent.isNil) { ^event};
			event = inevent.yield;
		};
	}
}


Pbindf : FilterPattern {
	var <>patternpairs;
	*new { arg pattern ... pairs;
		if (pairs.size.odd, { Error("Pbindf should have odd number of args.\n").throw });
		^super.new(pattern ? Event.default).patternpairs_(pairs)
	}
	storeArgs { ^[pattern] ++ patternpairs }
	embedInStream { arg event;
		var cleanup = EventStreamCleanup.new;
		var eventStream;
		var outevent;
		var streampairs = patternpairs.copy;
		var endval = streampairs.size - 1;

		forBy (1, endval, 2) { arg i;
			streampairs.put(i, streampairs[i].asStream);
		};
		eventStream = pattern.asStream;

		loop{
			outevent = eventStream.next(event);
			if (outevent.isNil) { ^cleanup.exit(event) };

			forBy (0, endval, 2) { arg i;
				var name = streampairs[i];
				var stream = streampairs[i+1];
				var streamout = stream.next(outevent);

				if (streamout.isNil) { ^cleanup.exit(event) };
				if (name.isSequenceableCollection) {
					if (name.size > streamout.size) {
						("the pattern is not providing enough values to assign to the key set:" + name).warn;
						^outevent
					};
					name.do { arg key, i;
						outevent.put(key, streamout[i]);
					};
				}{
					outevent.put(name, streamout);
				};

			};
			cleanup.update(outevent);
			event = yield(outevent);
		};
	}
}


Pstutter : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.new(pattern).n_(n)
	}
	storeArgs { ^[n,pattern] }
	embedInStream { arg event;
		var inevent, nn;

		var stream = pattern.asStream;
		var nstream = n.asStream;

		while {
			(inevent = stream.next(event)).notNil
		} {
			if((nn = nstream.next(event)).notNil) {
				nn.abs.do {
					event = inevent.copy.yield;
				};
			} { ^event };
		};
		^event;
	}
}


PdurStutter : Pstutter { // float streams

	embedInStream { arg event;
		var dur, stut;
		var durs = pattern.asStream;
		var stutts = n.asStream;
		while({
			(dur = durs.next(event)).notNil
			and: {(stut = stutts.next(event)).notNil}
			},{
				if(stut > 0,{ // 0 skips it
					if(stut > 1,{
						dur = dur / stut;
						stut.do({
							event = dur.yield;
						})
						},{
							event = dur.yield
					})
				})
		})
		^event;
	}
}

Pclutch : FilterPattern {
	var <>connected;
	*new { arg pattern, connected = true;
		^super.new(pattern).connected_(connected)
	}
	storeArgs { ^[ pattern, connected ] }
	embedInStream { arg inval;
		var clutchStream = connected.asStream;
		var stream = pattern.asStream;
		var outval, clutch;
		while {
			clutch = clutchStream.next(inval);
			clutch.notNil
		} {
			if(clutch === true or: { clutch == 1 }) {
				outval = stream.next(inval);
				if(outval.isNil) { ^inval };
				inval = outval.yield;
			} {
				outval ?? { outval = stream.next(inval) };
				inval = outval.copy.yield;
			};
		}
	}
}

Pwhile : FuncFilterPattern {
	embedInStream {arg event;
		while({ func.value(event) },{
			event = pattern.embedInStream(event);
		});
		^event;
	}
}

Pwrap : FilterPattern {
	var <>lo, <>hi;
	*new { arg pattern,lo,hi;
		^super.new(pattern).lo_(lo).hi_(hi)
	}

	storeArgs { ^[pattern,lo,hi] }

	embedInStream { arg event;
		var next;
		var stream = pattern.asStream;
		var loStr = lo.asStream;
		var hiStr = hi.asStream;
		var loVal, hiVal;
		while({
			loVal = loStr.next(event);
			hiVal = hiStr.next(event);
			next = stream.next(event);
			next.notNil and: { loVal.notNil } and: { hiVal.notNil }
			},{
				event = next.wrap(loVal, hiVal).yield
		});
		^event;
	}
}

Ptrace : FilterPattern {
	var <>key, printStream, prefix;

	*new { arg pattern, key, printStream, prefix = "";
		^super.newCopyArgs(pattern, key, printStream, prefix)
	}
	storeArgs { ^[ pattern, key, printStream, prefix ] }

	embedInStream { arg inval;
		var func, collected;
		printStream = printStream ? Post;
		if(key.isNil) {
			collected = pattern.collect {|item| printStream << prefix << item << Char.nl; item }
		} {
			func = { |val, item, prefix|
				if(val.isKindOf(Function) and: { item.isKindOf(Environment) })
				{
					val = item.use { val.value };
					printStream << prefix << val << "\t(printed function value)\n";
				} {
					printStream << prefix << val << Char.nl;
				};
			}.flop;
			collected = pattern.collect {|item|
				var val = item.atAll(key.asArray).unbubble;
				func.value(val, item, prefix);
				item
			}
		};
		^collected.embedInStream(inval)
	}
}

Pclump : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.new(pattern).n_(n)
	}
	embedInStream { arg event;
		var next, list, nval;
		var stream = pattern.asStream;
		var nstream = n.asStream;
		loop {
			list = [];
			nval = nstream.next(event);
			if (nval.isNil) { ^event };
			nval.do {
				next = stream.next(event);
				if (next.isNil) {
					if (list.size > 0) { event = list.yield };
					^event
				};
				list = list.add(next);
			};
			event = list.yield;
		}
	}
	storeArgs { ^[ n, pattern ] }
}

Pflatten : Pclump {
	embedInStream { arg event;
		var next, nval;
		var stream = pattern.asStream;
		var nstream = n.asStream;
		while {
			next = stream.next(event);
			nval = nstream.next(event);
			next.notNil and: { nval.notNil };
		}{
			if (next.isKindOf(SequenceableCollection)) {
				next = next.flatten(nval);
				next.do {|item| event = item.yield };
			}{
				event = next.yield;
			}
		}
		^event
	}
}

Pdiff : FilterPattern {
	embedInStream { arg event;
		var stream = pattern.asStream;
		var next, prev = stream.next(event);
		while {
			next = stream.next(event);
			next.notNil;
		}{
			event = (next - prev).yield;
			prev = next;
		}
		^event
	}
}

Prorate : FilterPattern {
	var <>proportion;

	*new { arg proportion, pattern=1;
		^super.new(pattern).proportion_(proportion)
	}

	embedInStream { arg inval;
		var val, c;
		var str = pattern.asStream;
		var prop = proportion.asStream;
		loop {
			val = str.next(inval);
			c = prop.next(inval);
			if(val.isNil or: { c.isNil }) { ^inval };
			if(c.isSequenceableCollection) {
				c.do { |el|
					inval = yield(el * val)
				}
			} {
				inval = yield(c * val);
				inval = yield(1 - c * val);
			}
		}
	}
	storeArgs { ^[proportion,pattern] }
}

Pavaroh : FilterPattern {

	var <>aroh, <>avaroh, <>stepsPerOctave;
	*new { arg pattern, aroh, avaroh, stepsPerOctave=12;
		^super.newCopyArgs(pattern, aroh, avaroh, stepsPerOctave)

	}
	storeArgs { ^[pattern, aroh, avaroh, stepsPerOctave ] }

	embedInStream { arg inval;
		var me, melast = 0, scale;
		var mestream = pattern.asStream;
		var stepsStr = stepsPerOctave.asStream, stepVal;

		while {
			stepVal = stepsStr.next(inval);
			me = mestream.next(inval);
			me.notNil and: { stepVal.notNil }
		} {
			scale = if(me >= melast) { aroh } { avaroh };
			melast = me;
			inval = me.degreeToKey(scale, stepVal).yield
		};
		^inval
	}
}
