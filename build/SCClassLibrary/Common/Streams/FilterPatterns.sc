
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
	embedInStream { arg event;
		repeats.value.do { event = pattern.embedInStream(event) };
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
	embedInStream { arg event;
		var inevent;
		var envir = Event.make(func);
		var stream = pattern.asStream;
		
		loop { 
			event = event.copy;
			event.putAll(envir);
			inevent =stream.next(event);
			if (inevent.isNil) { ^event };
			event = yield(inevent);
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
		var val, inevent, filteredEvent;
		var valStream = value.asStream;		
		var evtStream = pattern.asStream;
		
		loop {
			val = valStream.next;
//			if (val.isNil or: event.isNil) { ^event };
//			event = this.filterEvent(event, val);
			if (val.isNil or: event.isNil) { 
				filteredEvent = nil;
			}{
				event = event.copy;
				filteredEvent = this.filterEvent(event, val);
			};
			inevent = evtStream.next(filteredEvent);
			if (inevent.isNil) { ^event };
			event = yield(inevent);
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
		var val, inEvent;
		var valStream = value.asStream;
		var evtStream = pattern.asStream;
		
		loop {
			inEvent = evtStream.next(event);
			if (inEvent.isNil) { ^event };
			val = valStream.next;
			if (val.isNil) { ^event };
			
			this.filterEvent(inEvent, val);
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
			val = valStream.next;
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
		var inevent;
		var val, delta;
		var valStream = value.asStream;
		var evtStream = pattern.asStream;
		
		loop {
			inevent = evtStream.next(event);
			if (inevent.isNil) { ^event };
			val = valStream.next;
			if (val.isNil) { ^event };
			
			delta = event[\delta];
			if (delta.notNil) {
				inevent[\delta] = delta * val;
			};
			inevent[\dur] = inevent[\dur] * val;
			event = yield(inevent);
		}
	}
}



Pstretchp : Pstretch {

	embedInStream { arg event;
		var evtStream, val, inevent, delta;
		var valStream = value.asStream;
		while {
			val = valStream.next;
			val.notNil
		} {
			evtStream = pattern.asStream;
			while {
				inevent = evtStream.next(event);
				if(event.isNil) { ^nil.yield };
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


Pplayer : FilterPattern {
	var <>playerPattern, <>subPattern;
	*new { arg playerPattern, subPattern;
		^super.newCopyArgs(playerPattern, subPattern)
	}
	embedInStream { arg event;
		var player, inevent;
		var playerStream = playerPattern.asStream;
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
		
		count.value.do {
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
	embedInStream { arg event;
		var inevent, isEventStream = event.isKindOf(Event); // check for usage in the beginning
		var stream = pattern.asStream;
		
		count.value.do({
			inevent = stream.next(event);
			if (inevent.isNil, { ^event });
			event = inevent.yield;
			if (isEventStream and: { event.isNil } ) {
				stream.next(nil);
				^nil.yield
			}
		});
		stream.next(nil);
		^event
	}
}	


Pfindur : FilterPattern {
	var <>dur, <>tolerance;
	*new { arg dur, pattern, tolerance = 0.001;
		^super.new(pattern).dur_(dur).tolerance_(tolerance)
	}
	storeArgs { ^[dur,pattern,tolerance] }
	embedInStream { arg event;
		var item, delta, elapsed = 0.0, nextElapsed, inevent;
	
		var stream = pattern.asStream;
		
		loop {
			if(event.isNil) {
				stream.next(nil); 
				^nil.yield 
			};
			inevent = stream.next(event);
			if(inevent.isNil) { ^event };
			delta = inevent.delta;
			nextElapsed = elapsed + delta;
			if (nextElapsed.roundUp(tolerance) >= dur) {
				// must always copy an event before altering it.
				// fix delta time and yield to play the event.
				stream.next(nil);
				inevent = inevent.copy.put(\delta, dur - elapsed).yield;
				^inevent;		// this terminates the Pfindur. Event will not be played here.
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
	
		stream = pattern.asStream;

		loop {
			inevent = stream.next(event);
			if(inevent.isNil) {
				if(quant.notNil) { 
					event = Event.silent(elapsed.roundUp(quant) - elapsed).yield 
					^event
				};
			};
			delta = inevent.delta;
			nextElapsed = elapsed + delta;
			
			if (maxdur.notNil and: { nextElapsed.round(tolerance) >= maxdur })
			{
				inevent = inevent.copy; 
				inevent.put(\delta, maxdur - elapsed);
				stream.next(nil);
				event = inevent.yield;
				^event
				
			}
			{
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
			var delta, elapsed = 0.0, nextElapsed, str=pattern.asStream;
			loop ({
				delta = str.next(inval);
				if(delta.isNil) { 
					(sum - elapsed).yield; 
					^inval
				};
				nextElapsed = elapsed + delta;
				if (nextElapsed.round(tolerance) >= sum) {
					(sum - elapsed).yield;
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
		
		inevent.put('freq', \rest);
		inevent.put('dur', lag);
		event = inevent.yield;
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
		var eventStream;
		var inevent;
		var streampairs = patternpairs.copy;
		var endval = streampairs.size - 1;
		
		forBy (1, endval, 2) { arg i;
			streampairs.put(i, streampairs[i].asStream);
		};
		eventStream = pattern.asStream;
		
		loop{ 
			if(event.isNil) {
				eventStream.next(nil);
				^nil.yield 
			};
			inevent = eventStream.next(event);

			if (inevent.isNil) { ^event };
			forBy (0, endval, 2) { arg i;
				var name = streampairs[i];
				var stream = streampairs[i+1];
				var streamout = stream.next(inevent);
				
				if (streamout.isNil) { ^event };
				if (name.isSequenceableCollection) {					
					streamout.do({ arg val, i;
						inevent.put(name[i], val);
					});
				}{
					inevent.put(name, streamout);
				};
			
			};
			event = yield(inevent);	
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

		while ({
			(inevent = stream.next(event)).notNil
		},{
			(nn = nstream.next(event)).notNil.if({
				nn.abs.do({
					event = inevent.copy.yield;
				});
			}, { ^event });
		});
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

Pwhile : FuncFilterPattern {
	embedInStream {arg event;
		while({ func.value },{
			event = pattern.embedInStream(event);
		});
		^event;
	}
}

Pwrap : FilterPattern {
	var <>lo,<>hi;
	*new { arg pattern,lo,hi;
		^super.new(pattern).lo_(lo).hi_(hi)
	}
	storeArgs { ^[pattern,lo,hi] }
	embedInStream { arg event;
		var next;
		var stream = pattern.asStream;
		while({
			(next = stream.next).notNil
		},{
			event = next.wrap(lo,hi).yield
		});
		^event;
	}
}

Ptrace : FilterPattern {
	var <>key, printStream, prefix;
	*new { arg pattern, key, printStream, prefix; 
		^super.newCopyArgs(pattern, key, printStream, prefix) 
	}
	asStream {
		^pattern.asStream.trace(key, printStream, prefix)
	}

}

Pclump : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.newCopyArgs(pattern, n) 
	}
	embedInStream { arg event;
		var next, list, nval;
		var stream = pattern.asStream;
		var nstream = n.asStream;
		loop {
			list = [];
			nval = nstream.next;
			if (nval.isNil) { ^event };
			nval.do {
				next = stream.next(event);
				if (next.isNil) {
					if (list.size > 0) { event = list.yield };
					^event 
				};
				list = list ++ next;
			};
			event = list.yield;
		}
	}
}

Pflatten : FilterPattern {
	var <>n;
	*new { arg n, pattern;
		^super.newCopyArgs(pattern, n) 
	}
	embedInStream { arg event;
		var next;
		var stream = pattern.asStream;
		while {
			next = stream.next(event);
			next.notNil;
		}{
			if (next.isKindOf(Collection)) {
				next = next.flatten(n);
				next.do {|item| event = item.yield };
			}{
				event = next.yield;
			}
		}
		^event
	}
}

Pflow : FilterPattern {
	*new {	
		Error("Pflow was replaced. please use Pstep instead").throw;
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
}


