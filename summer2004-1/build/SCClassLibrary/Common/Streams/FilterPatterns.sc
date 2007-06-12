
FilterPattern : Pattern {
	var <>pattern;

	*new { arg pattern;
		^super.new.pattern_(pattern)
	}
}

Pn : FilterPattern {
	var <>repeats;
	*new { arg pattern, repeats=inf;
		^super.new(pattern).repeats_(repeats)
	}
	storeArgs { ^[pattern,repeats] }
	asStream {
		^Routine.new({ arg inevent;
			var stream, outval, n;
			n = repeats.value; 
			if(inf === n)
			{
				stream = pattern.asStream;
				loop {
					outval = stream.next(inevent);
					if(outval.isNil, { 
						stream = pattern.asStream;
						outval = stream.next(inevent);
					});
					inevent = outval.yield(inevent)
				}
			} {
				n.do {
					inevent = pattern.embedInStream(inevent);
				};
			}
		});
	}
}

// will be removed
Ploop : Pn {}


FuncFilterPattern : FilterPattern {
	var <>func;
	
	*new { arg func, pattern;
		^super.new(pattern).func_(func)
	}
	storeArgs { ^[func,pattern] }
}

Pcollect : FuncFilterPattern {
	asStream {
		^pattern.asStream.collect(func);
	}
}

Pselect : FuncFilterPattern {
	asStream {
		^pattern.asStream.select(func);
	}
}

Preject : FuncFilterPattern {
	asStream {
		^pattern.asStream.reject(func);
	}
}



Pfset : FuncFilterPattern {
	asStream {
		var stream, envir;

		envir = Event.make(func);
		
		stream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			event = event.copy;
			event.putAll(envir);
			stream.next(event);
		});
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
	asStream {
		var evtStream, valStream;
		
		valStream = value.asStream;
		
		evtStream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			var val;
			val = valStream.next;
			if (val.notNil && event.notNil, { event = this.filterEvent(event, val) });
			evtStream.next(event);
		});
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
	asStream {
		var evtStream, valStream;
		
		valStream = value.asStream;
		
		evtStream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			var val;
			event = evtStream.next(event);
			val = valStream.next;
			if (val.isNil, { nil },{
				if (event.notNil, { this.filterEvent(event, val) });
				event
			});
		});
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
	asStream {
		^Routine.new({ arg inevent;
			var valStream, evtStream, val, outevent;
			valStream = value.asStream;
			while({
				val = valStream.next;
				val.notNil
			},{
				evtStream = pattern.asStream;
				while({
					outevent = evtStream.next(inevent);
					outevent.notNil
				},{
					this.filterEvent(outevent, val);
					inevent = outevent.yield;
				});
			});
		});
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
	asStream {
		var evtStream, valStream;
		
		valStream = value.asStream;
		
		evtStream = pattern.asStream;
		
		^FuncStream.new({ arg event;
			var val, delta;
			event = evtStream.next(event);
			val = valStream.next;
			if (val.isNil, { nil },{
				if (event.notNil) {
					delta = event[\delta];
					if (delta.notNil) {
						event[\delta] = delta * val;
					};
					event[\dur] = event[\dur] * val;
				};
				event
			});
		});
	}
}

Pstretchp : FilterPattern {
	var <>value;
	*new { arg value, pattern;
		^super.new(pattern).value_(value)
	}
	storeArgs { ^[value,pattern] }
	asStream {
		^Routine.new({ arg inevent;
			var valStream, evtStream, val, outevent, delta;
			valStream = value.asStream;
			while({
				val = valStream.next;
				val.notNil
			},{
				evtStream = pattern.asStream;
				while({
					outevent = evtStream.next(inevent);
					outevent.notNil
				},{
					delta = outevent[\delta];
					if (delta.notNil) {
						outevent[\delta] = delta * val;
					};
					outevent[\dur] = outevent[\dur] * val;
					inevent = outevent.yield;
				});
			});
		});
	}
}

Pplayer : FilterPattern {
	var <>playerPattern, <>subPattern;
	*new { arg playerPattern, subPattern;
		^super.newCopyArgs(playerPattern, subPattern)
	}
	asStream {
		var playerStream, stream;
		playerStream = playerPattern.asStream;
		stream = subPattern.asStream;
		^FuncStream({ arg event;
			var player;
			event = stream.next(event);
			if (event.notNil) {
				player = playerStream.next(event);
				if (player.notNil) {
					event.parent = player.event;
				}
			};
		})
	}
}


Pfin : FilterPattern {
	var <>count;
	*new { arg count, pattern;
		^super.new(pattern).count_(count)
	}
	storeArgs { ^[count,pattern] }
	asStream { 
		^Routine.new({ arg event;
			var stream;
		
			stream = pattern.asStream;
			
			count.value.do({
				event = stream.next(event).yield;
			});
		});
	}
}	

Pfindur : FilterPattern {
	var <>dur, <>tolerance;
	*new { arg dur, pattern, tolerance = 0.001;
		^super.new(pattern).dur_(dur).tolerance_(tolerance)
	}
	storeArgs { ^[dur,pattern,tolerance] }
	asStream { 
		^Routine.new({ arg inevent;
			var item, stream, delta, elapsed = 0.0, nextElapsed;
		
			stream = pattern.asStream;
			
			loop ({
				inevent = stream.next(inevent);
				if(inevent.isNil) {
					nil.alwaysYield 
				};
				delta = inevent.delta;
				nextElapsed = elapsed + delta;
				if (nextElapsed.roundUp(tolerance) >= dur, {
					// must always copy an event before altering it.
					inevent = inevent.copy; 
					// fix delta time.
					inevent.put(\delta, dur - elapsed);
					inevent = inevent.yield;
					// the note is not truncated here. maybe you want that..
					
					// end of pattern
					nil.alwaysYield;
				},{
					elapsed = nextElapsed;
					inevent = inevent.yield;
				});
			});
		});
	}
}

Psync : FilterPattern {
	var <>quant, <>maxdur, <>tolerance;
	*new { arg pattern, quant, maxdur, tolerance = 0.001;
		^super.new(pattern).quant_(quant).maxdur_(maxdur).tolerance_(tolerance)
	}
	storeArgs { ^[pattern,quant,maxdur,tolerance] }
	asStream { 
		^Routine.new({ arg inevent;
			var item, stream, delta, elapsed = 0.0, nextElapsed, clock;
		
			stream = pattern.asStream;
			/*
			clock = inevent[\clock].debug ? TempoClock.default;
			elapsed = clock.elapsedBeats;
			elapsed = elapsed.roundUp(quant ? 1) - elapsed;
			if(elapsed > tolerance) { Event.silent(elapsed).yield };
			*/
			loop {
				inevent = stream.next(inevent);
				if(inevent.isNil) {
					if(quant.notNil) { Event.silent(elapsed.roundUp(quant) - elapsed).yield };
					nil.alwaysYield
				};
				delta = inevent.delta;
				nextElapsed = elapsed + delta;
				
				if (maxdur.notNil and: { nextElapsed.round(tolerance) >= maxdur })
				{
					inevent = inevent.copy; 
					inevent.put(\delta, maxdur - elapsed);
					inevent = inevent.yield;
					nil.alwaysYield;
				}
				{
					elapsed = nextElapsed;
					inevent = inevent.yield;
				};
			};
		});
	}
}


Pconst : FilterPattern {
	var <>sum, <>tolerance;
	*new { arg sum, pattern, tolerance=0.001;
		^super.new(pattern).sum_(sum).tolerance_(tolerance)
	}
	storeArgs { ^[sum,pattern,tolerance] }
	asStream {
		^pattern.asStream.constrain(sum,tolerance)
	}
}

Plag : FilterPattern {
	var <>lag;
	*new { arg lag, pattern;
		^super.new(pattern).lag_(lag)
	}
	storeArgs { ^[lag,pattern] }
	asStream {
		var stream, item;
		
		stream = pattern.asStream;
		
		^Routine.new({ arg inevent;	
			var newevent;	
			newevent = inevent.copy;
			newevent.put('freq', \rest);
			newevent.put('dur', lag);
			inevent = newevent.yield;
			loop ({
				inevent = stream.next(inevent).yield;
			});
		});
	}
}


Pbindf : FilterPattern {
	var <>patternpairs;
	*new { arg pattern ... pairs;
		if (pairs.size.odd, { Error("Pbindf should have odd number of args.\n").throw });
		^super.new(pattern ? Event.default).patternpairs_(pairs)
	}
	storeArgs { ^[pattern] ++ patternpairs }
	asStream {
		var streampairs, endval, eventStream;
		
		streampairs = patternpairs.copy;
		endval = streampairs.size - 1;
		forBy (1, endval, 2) { arg i;
			streampairs.put(i, streampairs[i].asStream);
		};
		eventStream = pattern.asStream;
		
		^FuncStream.new({ arg inevent;
			var event;
			var sawNil = false;

			event = eventStream.next(inevent);

			if (event.isNil) { nil } {
				forBy (0, endval, 2) { arg i;
					var name, stream, streamout;
					name = streampairs[i];
					stream = streampairs[i+1];
					
					streamout = stream.next(event);
					
					if (streamout.isNil) {
						sawNil = true;
					}{
						if (name.isSequenceableCollection) {					
							streamout.do({ arg val, i;
								event.put(name[i], val);
							});
						}{
							event.put(name, streamout);
						};
					};
				};
				if (sawNil) { nil } { 
					event 
				};
			};
		},{
			eventStream.reset;
			
			streampairs = patternpairs.copy;
			endval = streampairs.size - 1;
			forBy (1, endval, 2) { arg i;
				streampairs.put(i, streampairs[i].asStream);
			};
		});
	}
}


Pstutter : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.new(pattern).n_(n)
	}
	storeArgs { ^[n,pattern] }
	asStream { 
		^Routine.new({ arg inevent;
			var event, stream;
		
			stream = pattern.asStream;
		
			while ({
				(event = stream.next(inevent)).notNil
			},{
				n.do({
					inevent = event.copy.yield;
				});
			});
		});
	}
}


PdurStutter : Pstutter { // float streams
	
	asStream {
		^Routine.new({
			var durs,stutts,dur,stut;
			durs = pattern.asStream;
			stutts = n.asStream;
			while({
				(dur = durs.next).notNil
				and: {(stut = stutts.next).notNil}
			},{
				if(stut > 0,{ // 0 skips it
					if(stut > 1,{
						dur = dur / stut;
						stut.do({
							dur.yield;
						})
					},{
						dur.yield
					})
				})
			})		
		})
	}		
}
	
Pwhile : FuncFilterPattern {
	asStream {
		^Routine.new({ arg inval;
			while({ func.value },{
				inval = pattern.embedInStream(inval);
			});
		});
	}
}

Pwrap : FilterPattern {
	var <>lo,<>hi;
	*new { arg pattern,lo,hi;
		^super.new(pattern).lo_(lo).hi_(hi)
	}
	storeArgs { ^[pattern,lo,hi] }
	asStream {
		^Routine({
			var stream,next;
			stream = pattern.asStream;
			while({
				(next = stream.next).notNil
			},{
				next.wrap(lo,hi).yield
			})
		})
	}
}

Ptrace : FilterPattern {
	var <>key, printStream;
	*new { arg pattern, key, printStream; 
		^super.newCopyArgs(pattern, key, printStream) 
	}
	asStream {
		^pattern.asStream.trace(key, printStream)
	}

}
