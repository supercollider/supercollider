 
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
	++ { arg stream;
		// concatenate two streams
		^this.interlace(true, stream);
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
		^this.notYetImplemented
	}
	
	spawn { arg spawn, eventCount, synth;
		// this method assumes that the stream returns something that responds to spawn
		this.next.spawn(spawn, eventCount, synth);	
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
	
	play { arg clock;
		clock = clock ? SystemClock;
		clock.play(this);
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


// PauseStream is a stream wrapper that can be started and stopped.

PauseStream : Stream
{
	var <stream, originalStream, <clock;
	
	*new { arg argStream; 
		^super.newCopyArgs(nil, argStream, TempoClock.default) 
	}
	
	play { arg argClock, doReset = false;
		clock = argClock ? TempoClock.default;
		if (stream.notNil, { "already playing".postln; ^this });
		if (doReset, { this.reset });
		stream = originalStream; 
		super.play(clock) 
	}
	reset { ^originalStream.reset }
	stop { stream = nil }

	pause { stream = nil }
	resume { ^this.play(clock, false) }
	
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
	*new { arg func; 
		^super.new(Routine(func)) 
	}
}

////////////////////////////////////////////////////////////////////////

EventStream : PauseStream {
	var <>muteCount = 0;
	
	*new { arg stream;
		^super.new(stream);
	}
	
	mute { muteCount = muteCount + 1; }
	unmute { muteCount = muteCount - 1; }
	
	next { arg inTime;
		var event, nextTime;
		event = stream.next(inTime);	
		if (event.isNil, {	
			stream = nil;
			^nil
		},{
			if (muteCount > 0, { event.put(\freq, \rest) });
			if ((nextTime = event.play).isNil, { stream = nil });
			^nextTime
		});
	}
}


