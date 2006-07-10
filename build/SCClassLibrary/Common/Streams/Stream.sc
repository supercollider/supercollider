 
Stream : AbstractFunction {	
	// 'reset' is defined in class Object to do nothing.
	// reading
	next { ^this.subclassResponsibility(thisMethod) }
	iter { ^this }
		
	value { arg inval; ^this.next(inval) }
	valueArray { ^this.next }
	
	nextN { arg n, inval;
		^Array.fill(n, { this.next(inval) });
	}
	all {
		// don't do this on infinite streams.
		var array;
		this.do {|item| array = array.add(item); };
		^array
	}
	
	// writing
	put { arg item;
		^this.subclassResponsibility(thisMethod) 
	}
	putN { arg n, item; 
		n.do({ this.put(item); });
	}
	putAll { arg aCollection;
		aCollection.do {|item| this.put(item); };
	}
	
	do { arg function;
		var item, i=0;
		while { 
			item = this.next; 
			item.notNil 
		}{
			function.value(item, i);
			i = i + 1;
		};
	}
	generate { arg function, item;
		var i=0;
		while { 
			item = this.next(item); 
			item.notNil 
		}{
			function.value(item, i);
			i = i + 1;
		};
	}
	
	// combination
	collect { arg argCollectFunc;
		// modify a stream
		var nextFunc = { arg inval;
			inval = this.next(inval);
			if ( inval.notNil, {
				argCollectFunc.value(inval) 
			})
		};
		var resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	reject { arg function;
		// reject elements from a stream
		var nextFunc = { arg inval;
			inval = this.next(inval);
			while { 
				inval.notNil and: { function.value(inval) }
			}{
				inval = this.next(inval);
			};
			inval
		};
		var resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	select { arg function;
		// select elements from a stream
		var nextFunc = { arg inval;
			inval = this.next(inval);
			while { 
				inval.notNil and: { function.value(inval).not }
			}{
				inval = this.next(inval);
			};
			inval
		};
		var resetFunc = { this.reset };
		^FuncStream.new(nextFunc, resetFunc);
	}
	
	dot { arg function, stream;
		// combine item by item with another stream
		^FuncStream.new(
			{ arg inval;
				var x = this.next(inval);
				var y = stream.next(inval);
				if ( x.notNil and: { y.notNil }, {
					function.value(x, y)
				});
			},
			{ this.reset; stream.reset; }
		);
	}
	
	interlace { arg function, stream;
		// interlace with another stream
		var nextx = this.next;
		var nexty = stream.next;
		^FuncStream.new({
			var val;
			if ( nextx.isNil ) {
				if ( nexty.isNil) {nil}{ val = nexty; nexty = stream.next; val };
			}{
				if ( nexty.isNil or: { function.value(nextx,nexty) }, 
					{ val = nextx; nextx = this.next; val },
					{ val = nexty; nexty = stream.next; val }
				);					
			};
		},
		{ 
			this.reset; stream.reset; 
			nextx = this.next;
			nexty = stream.next;
		});
	}

	++ { arg stream; ^this.appendStream(stream) }
	
	appendStream { arg stream;
		var reset = false;
		^Routine({ arg inval;
			if (reset) {
				this.reset;
				stream.reset;
			};
			reset = true;
			inval = this.embedInStream(inval);
			stream.embedInStream(inval);
		});
	}
	
	collate { arg stream;
		// ascending order merge of two streams
		^this.interlace({|x y| x < y }, stream);
	}
	
	<> { arg obj; ^Pchain(this, obj).asStream }
			

	// function composition
	composeUnaryOp { arg argSelector;
		^UnaryOpStream.new(argSelector, this)
	}
	composeBinaryOp { arg argSelector, argStream, adverb;
		if(adverb.isNil) {
			^BinaryOpStream.new(argSelector, this, argStream)
		} {
			if (adverb == 'x') {
				^BinaryOpXStream.new(argSelector, this, argStream);
			};
		};
		^nil
	}
	reverseComposeBinaryOp { arg argSelector, argStream, adverb;
		if(adverb.isNil) {
			^BinaryOpStream.new(argSelector, argStream, this)
		} {
			if (adverb == 'x') {
				^BinaryOpXStream.new(argSelector, argStream, this);
			};
		};
		^nil
	}
	composeNAryOp { arg argSelector, anArgList;
		^NAryOpStream.new(argSelector, this, anArgList);
	}

	embedInStream { arg inval;
		var outval;
		while {
			outval = this.value(inval);
			outval.notNil
		}{
			inval = outval.yield;
		};
		^inval
	}
	
	asEventStreamPlayer { arg protoEvent;
		^EventStreamPlayer(this, protoEvent);
	}
	
	play { arg clock, quant=0.0;
		clock = clock ? TempoClock.default;
		clock.play(this, quant);
	}
	
	trace { arg key, printStream, prefix="";
		printStream = printStream ? Post;
		^if(key.isNil) {
			this.collect {|item| printStream << prefix << item << Char.nl; item }
		} {
			this.collect {|item|
				var val = item.at(key);
				if(val.isKindOf(Function) and: { item.isKindOf(Environment) }) 
				{ 
					val = item.use { val.value };
					printStream << prefix << val << "\t(printed function value)\n"; 
				} {
					printStream << prefix << val << Char.nl;
				};
				 
				item 
			}
		}
			 
	}
	
	constrain { arg sum, tolerance=0.001;
		^Pconst(sum, tolerance).asStream
	}
	
	repeat { arg repeats = inf;
		^r { arg inval;
			repeats.do {
				inval = this.reset.embedInStream(inval)
			}
		}
	}
	
	
}

OneShotStream : Stream {
	var value, once = true;
	*new { arg value;
		^super.newCopyArgs(value)
	}
	next { ^if (once) {once = false; value} }
	reset { once = true }
	storeArgs { ^[value] }
}

EmbedOnce : Stream  {
	var <stream;
	*new { arg stream;
		^super.newCopyArgs(stream.asStream)
	}
	next { arg inval;
		var val = stream.next(inval);
		if(val.isNil) { stream = nil }; // embed once, then release memory
		^val
	}
	storeArgs { ^[stream] }
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
	storeArgs { ^[nextFunc, resetFunc] }
}


CleanupStream : Stream {
	var <stream, <>cleanup;
	
	*new { arg stream, cleanup;
		^super.newCopyArgs(stream, cleanup)
	}
	next { arg inval;
		var outval = stream.next(inval);
		if (outval.isNil) {
			cleanup.value(this, inval);
			cleanup = nil;
		}
		^outval
	}
	reset {
		stream.reset
	}
}

// PauseStream is a stream wrapper that can be started and stopped.

PauseStream : Stream
{
	var <stream, <originalStream, <clock, <nextBeat, <streamHasEnded=false, isWaiting = false;
	
	*new { arg argStream, clock; 
		^super.newCopyArgs(nil, argStream, clock ? TempoClock.default) 
	}
	
	isPlaying { ^stream.notNil }
	
	play { arg argClock, doReset = (false), quant=0.0;
		if (stream.notNil, { "already playing".postln; ^this });
		if (doReset, { this.reset });
		clock = argClock ? clock ? TempoClock.default;
		streamHasEnded = false;
		stream = originalStream; 
		isWaiting = true;	// make sure that accidental play/stop/play sequences
						// don't cause memory leaks
		clock.play({
			if(isWaiting and: { nextBeat.isNil }) {
				clock.sched(0, this);
				isWaiting = false;
			};
			nil
		}, quant);
		^this
	}
	reset { ^originalStream.reset }
	stop {
		stream = nil;
		isWaiting = false;
	}
	removedFromScheduler { nextBeat = nil; this.stop }
	wasStopped { ^streamHasEnded.not and: { stream.isNil } }
	
	pause {
		stream = nil;
		isWaiting = false;
	}
	resume { arg argClock, quant=1.0; 
		^this.play(clock ? argClock, false, quant) 
	}
	
	refresh {
		stream = originalStream
	}

	start { ^this.play(clock, true) }
		
	stream_ { arg argStream; 
		originalStream = argStream; 
		if (stream.notNil, { stream = argStream; streamHasEnded = argStream.isNil; });
	}

	next { arg inval; 
		var nextTime = stream.next(inval);
		if (nextTime.isNil) { streamHasEnded = stream.notNil; stream = nextBeat = nil }
			{ nextBeat = inval + nextTime };	// inval is current logical beat
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
	storeArgs { ^originalStream.storeArgs 
				++ if(clock != TempoClock.default) { clock } 
	}
}

////////////////////////////////////////////////////////////////////////

EventStreamPlayer : PauseStream {
	var <>event, <>muteCount = 0;
	
	*new { arg stream, event;
		^super.new(stream).event_(event ? Event.default);
	}
	
		// play can and should inherit from PauseStream, don't know why it's duplicated here
//	play { arg argClock, doReset = false, quant=1.0;
//		if (stream.notNil) { "already playing".postln; ^this };
//		if (doReset) { this.reset };
//		clock = argClock ? clock ? TempoClock.default;
//		streamHasEnded = false;
//		stream = originalStream; 
//		clock.play(this, quant);
//	}
	
	stop { stream.next(nil); stream = nextBeat = nil;  }
	mute { muteCount = muteCount + 1; }
	unmute { muteCount = muteCount - 1; }
	
	next { arg inTime;
		var nextTime;
		var outEvent = stream.next(event);
		if (outEvent.isNil) {
			streamHasEnded = stream.notNil;
			stream = nextBeat = nil;
			^nil
		}{
			if (muteCount > 0) { outEvent.put(\freq, \rest) };
			if ((nextTime = outEvent.play).isNil) { stream = nil };
			nextBeat = inTime + nextTime;	// inval is current logical beat
			^nextTime
		};
	}
	
	asEventStreamPlayer { ^this }
}


////////////////////////////////////////////////////////////////////////


GenStream : Stream 
{
	// a stream wrapper for the Gen class
	var <gen, prResetGen, firstTime=true;
	*new { arg gen;
		^super.newCopyArgs(gen, gen)
	}
	reset {
		gen = prResetGen;
		firstTime = true;
	}
	next {
		if (firstTime) { firstTime = false; }
		{
			gen = gen.genNext;
		}
		^gen.genCurrent
	}
	gen_ { arg inGen;
		gen = prResetGen = inGen;
		firstTime = true;
	}
}
