 
Stream : AbstractFunction {	
	// 'reset' is defined in class Object to do nothing.
	// reading
	next { ^this.subclassResponsibility(thisMethod) }
	value { arg inval; ^this.next(inval) }
	valueArray { ^this.next }
	
	nextN { arg n;
		^Array.fill(n, { this.next; });
	}
	all {
		arg list;
		list = List.new;
		this.do({ arg item; list.add(item); });
		^list
	}
	
	// writing
	put { arg item;
		^this.subclassResponsibility(thisMethod) 
	}
	putN { arg n, item; 
		n.do({ this.put(item); });
	}
	putAll { arg aCollection;
		aCollection.do({ arg item; this.put(item); });
	}
	
	do { arg function;
		var item;
		while ({ 
			item = this.next; 
			item.notNil 
		},{
			function.value(item);
		});
	}
	
	// combination
	collect { arg argCollectFunc;
		// modify a stream
		var nextFunc, resetFunc;

		nextFunc = { arg inval;
			inval = this.next(inval);
			if ( inval.notNil, {
				argCollectFunc.value(inval) 
			},{ 
				nil 
			})
		};
		resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	reject { arg function;
		// reject elements from a stream
		var nextFunc, resetFunc;
		nextFunc = { arg inval;
			inval = this.next(inval);
			while ({ 
				inval.notNil and: { function.value(inval) }
			},{
				inval = this.next(inval);
			});
			inval
		};
		resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	select { arg function;
		// select elements from a stream
		var nextFunc, resetFunc;
		nextFunc = { arg inval;
			inval = this.next(inval);
			while ({ 
				inval.notNil and: { function.value(inval).not }
			},{
				inval = this.next(inval);
			});
			inval
		};
		resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	
	dot { arg function, stream;
		// combine item by item with another stream
		^FuncStream.new(
			{ arg inval;
				var x, y;
				x = this.next(inval);
				y = stream.next(inval);
				if ( x.notNil and: { y.notNil }, {
					function.value(x, y)
				});
			},
			{ this.reset; stream.reset; }
		);
	}
	interlace { arg function, stream;
		// interlace with another stream
		var nextx, nexty;
		
		nextx = this.next;
		nexty = stream.next;
		^FuncStream.new({
			var val;
			if ( nextx.isNil, {
				if ( nexty.isNil, {nil},{ val=nexty; nexty = stream.next; val });
			},{
				if ( nexty.isNil or: { function.value(nextx,nexty) }, 
					{ val=nextx; nextx = this.next; val },
					{ val=nexty; nexty = stream.next; val }
				);					
			});
		},
		{ 
			this.reset; stream.reset; 
			nextx = this.next;
			nexty = stream.next;
		});
	}
	
	++ { arg stream; ^this.appendStream(stream) }
	
	appendStream { arg stream;
		^Routine({ arg inval;
			inval = this.embedInStream(inval);
			stream.embedInStream(inval);
		});
	}
	
	collate { arg stream;
		// ascending order merge of two streams
		^this.interlace({ arg x, y; x < y }, stream);
	}
			

	// function composition
	composeUnaryOp { arg argSelector;
		^UnaryOpStream.new(argSelector, this)
	}
	composeBinaryOp { arg argSelector, argStream;
		^BinaryOpStream.new(argSelector, this, argStream)
	}
	reverseComposeBinaryOp { arg argSelector, argStream;
		^BinaryOpStream.new(argSelector, argStream, this)
	}
	composeNAryOp { arg argSelector, anArgList;
		^NAryOpStream.new(argSelector, this, anArgList);
	}

	embedInStream { arg inval;
		var outval;
		while ({
			outval = this.value(inval);
			outval.notNil
		},{
			inval = outval.yield;
		});
		^inval
	}
	
	asEventStreamPlayer { arg protoEvent;
		^EventStreamPlayer(this, protoEvent);
	}
	
	play { arg clock, quant=0.0;
		clock = clock ? TempoClock.default;
		clock.play(this, quant);
	}
	
	trace { arg key, printStream;
		printStream = printStream ? Post;
		^if(key.isNil) {
			this.collect { arg item; printStream << item << Char.nl; item }
		} {
			this.collect { arg item; printStream << item.at(key) << Char.nl; item }
		}
			 
	}
	
	constrain { arg sum, tolerance=0.001;
		^Routine.new({
			var delta, elapsed = 0.0, nextElapsed;
			loop ({
				delta = this.next;
				if(delta.isNil) { 
					(sum - elapsed).yield; 
					nil.alwaysYield 
				};
				nextElapsed = elapsed + delta;
				if (nextElapsed.round(tolerance) >= sum, {
					(sum - elapsed).yield;
					nil.alwaysYield;
				},{
					elapsed = nextElapsed;
					delta.yield;
				});
			});
		})
	}
	
	
}

FuncStream : Stream {
	var <>nextFunc; // Func is evaluated for each next state
	var <>resetFunc; // Func is evaluated for each next state
	*new { arg nextFunc, resetFunc;	
		^super.new.nextFunc_(nextFunc).resetFunc_(resetFunc)
	}
	next { arg inval;
		^nextFunc.value(inval) 
	}
	reset { 
		^resetFunc.value
	}
}


CleanupStream : Stream {
	var <stream, <>cleanup;
	
	*new { arg stream, cleanup;
		^super.newCopyArgs(stream, cleanup)
	}
	next { arg val;
		val = stream.next(val);
		if (val.isNil) {
			cleanup.value(this);
			cleanup = nil;
		}
		^val
	}
	reset {
		stream.reset
	}
}

// PauseStream is a stream wrapper that can be started and stopped.

PauseStream : Stream
{
	var <stream, <originalStream, <clock;
	
	*new { arg argStream, clock; 
		^super.newCopyArgs(nil, argStream, clock ? TempoClock.default) 
	}
	
	isPlaying { ^stream.notNil }
	play { arg argClock, doReset = false, quant=1.0;
		if (stream.notNil, { "already playing".postln; ^this });
		if (doReset, { this.reset });
		clock = argClock ? clock ? TempoClock.default;
		stream = originalStream; 
		clock.play(this, quant);
	}
	reset { ^originalStream.reset }
	stop {  stream = nil }
	removedFromScheduler { this.stop }
	
	pause { stream = nil }
	resume { arg argClock, quant=1.0; 
		^this.play(clock ? argClock, false, quant) 
	}
	
	refresh {
		stream = originalStream
	}

	start { ^this.play(clock, true) }
		
	stream_ { arg argStream; 
		originalStream = argStream; 
		if (stream.notNil, { stream = argStream });
	}

	next { arg inval; 
		var nextTime;
		nextTime = stream.next(inval);
		if (nextTime.isNil, { stream = nil });
		^nextTime
	}
	awake { arg beats, seconds, inClock;
		stream.beats = beats;
		^this.next(beats)
	}
}

// Task is a PauseStream for wrapping a Routine

Task : PauseStream {
	*new { arg func, clock; 
		^super.new(Routine(func), clock) 
	}
}

////////////////////////////////////////////////////////////////////////

EventStreamPlayer : PauseStream {
	var <>event, <>muteCount = 0;
	
	*new { arg stream, event;
		^super.new(stream).event_(event ? Event.default);
	}

	play { arg argClock, doReset = false, quant=1.0;
		if (stream.notNil) { "already playing".postln; ^this };
		if (doReset) { this.reset };
		clock = argClock ? clock ? TempoClock.default;
		stream = originalStream; 
		clock.play(this, quant);
	}
	
	mute { muteCount = muteCount + 1; }
	unmute { muteCount = muteCount - 1; }
	
	next { arg inTime;
		var outEvent, nextTime;
		outEvent = stream.next(event);
		if (outEvent.isNil) {
			stream = nil;
			^nil
		}{
			if (muteCount > 0) { outEvent.put(\freq, \rest) };
			if ((nextTime = outEvent.play).isNil) { stream = nil };
			^nextTime
		};
	}
	
	asEventStreamPlayer { ^this }
}


