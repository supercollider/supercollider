
Pattern : AbstractFunction {
	
	
	// concatenate Patterns
	++ { arg aPattern;
		^Pseq.new([this, aPattern])
	}	

	play { arg clock, protoEvent, quant=1.0;
		^this.asEventStreamPlayer(protoEvent).play(clock, false, quant)	}

	asStream { ^Routine({ arg inval; this.embedInStream(inval) }) }
	iter { ^this.asStream }

	asEventStreamPlayer { arg protoEvent;
		^EventStreamPlayer(this.asStream, protoEvent);
	}
	embedInStream { arg inval;
		^this.asStream.embedInStream(inval);
	}
	
	// filtering operations
	collect { arg function;
		^Pcollect.new(function, this)
	}
	select { arg function;
		^Pselect.new(function, this)
	}
	reject { arg function;
		^Preject.new(function, this)
	}
	
	// function composition
	composeUnaryOp { arg operator;
		^Punop.new(operator, this)
	}
	composeBinaryOp { arg operator, pattern, adverb;
		^Pbinop.new(operator, this, pattern, adverb)
	}
	reverseComposeBinaryOp { arg operator, pattern, adverb;
		^Pbinop.new(operator, pattern, this, adverb)
	}
	composeNAryOp { arg selector, argList;
		^Pnaryop.new(selector, this, argList);
	}
	
	degreeToKey { arg scale, stepsPerOctave=12;
		^PdegreeToKey(this,scale,stepsPerOctave)
	}
	
	trace { arg key, printStream; ^Ptrace(this, key, printStream) }
}

Pfunc : Pattern {
	var <>nextFunc; // Func is evaluated for each next state
	var <>resetFunc; // Func is evaluated for each next state
	*new { arg nextFunc, resetFunc;	
		^super.newCopyArgs(nextFunc, resetFunc)
	}
	storeArgs { ^[nextFunc] ++ resetFunc }
	asStream {
//		^FuncStreamAsRoutine.new(nextFunc, resetFunc)
	^FuncStream.new(nextFunc, resetFunc)
	}
//	embedInStream { arg inval; loop { inval = yield(nextFunc.value(inval)) }  }
}

Prout : Pattern {
	var <>routineFunc;
	*new { arg routineFunc;
		^super.newCopyArgs(routineFunc)
	}
	storeArgs { ^[routineFunc] }
	asStream {
		^Routine.new(routineFunc)
	}
}

Proutine : Prout {
	embedInStream { arg inval; ^routineFunc.value(inval) }
}

Pfuncn : Pattern {
	var <>func, <>repeats;
	*new { arg func, repeats = 1;
		^super.newCopyArgs(func, repeats)
	}
	storeArgs { ^[func,repeats] }
	embedInStream {  arg inval;
		repeats.value.do({
			inval = func.value(inval).yield;
		});
		^inval
	}
}	



// Punop and Pbinop are used to create patterns in response to math operations
Punop : Pattern {
	var <>operator, <>a;
	*new { arg operator, a;
		^super.newCopyArgs(operator, a)
	}
	
	storeOn { arg stream; stream <<< a << "." << operator }

	asStream {
		var stream;
		stream = a.asStream;
		^UnaryOpStream.new(operator, stream);
	}
}

Pbinop : Pattern {
	var <>operator, <>a, <>b, <>adverb;
	*new { arg operator, a, b, adverb;
		^super.newCopyArgs(operator, a, b, adverb)
	}
	
	storeOn { arg stream;
			stream << "(" <<< a << " " << operator.asBinOpString;
			if(adverb.notNil) { stream << "." << adverb };
			stream << " " <<< b << ")"
	}

	asStream {
		var streamA, streamB;
		streamA = a.asStream;
		streamB = b.asStream;
		if (adverb.isNil) {
			^BinaryOpStream.new(operator, streamA, streamB);
		};
		if (adverb == 'x') {
			^BinaryOpXStream.new(operator, streamA, streamB);
		};
		^nil
	}
}

Pnaryop : Pattern {
	var <>operator, <>a, <>arglist;
	*new { arg operator, a, arglist;
		^super.newCopyArgs(operator, a, arglist)
	}
	storeOn { arg stream; stream <<< a << "." << operator << "(" <<<* arglist << ")" }
	asStream {
		var streamA, streamlist;
		streamA = a.asStream;
		streamlist = arglist.collect({ arg item; item.asStream });
		^NAryOpStream.new(operator, streamA, streamlist);
	}
}

Pevent : Pattern {
	var <>pattern, <>event;
	
	*new { arg pattern, event;
		^super.newCopyArgs(pattern, event);
	}
	storeArgs { ^[pattern, event] }
	embedInStream { arg inval;
		var stream, outval;
		stream = pattern.asStream;
		loop { 
			outval = stream.next(event);
			if (outval.isNil) {^inval};
			inval = outval.yield
		 }
	}
}


Pbind : Pattern {
	var <>patternpairs;
	*new { arg ... pairs;
		if (pairs.size.odd, { Error("Pbind should have even number of args.\n").throw; });
		^super.newCopyArgs(pairs)
	}
	
	storeArgs { ^patternpairs }
	embedInStream { arg inevent;
		var streampairs, endval;
		var event;
		var sawNil = false;
		
		streampairs = patternpairs.copy;
		endval = streampairs.size - 1;
		forBy (1, endval, 2) { arg i;
			streampairs.put(i, streampairs[i].asStream);
		};

		loop {
			if (inevent.isNil) { ^nil };
			event = inevent.copy;
			forBy (0, endval, 2) { arg i;
				var name, stream, streamout;
				name = streampairs[i];
				stream = streampairs[i+1];		
				streamout = stream.next(event);
				if (streamout.isNil) { ^inevent };

				if (name.isSequenceableCollection) {
					streamout.do { arg val, i;
						event.put(name[i], val);
					};
				}{
					event.put(name, streamout);
				};
				
			};
			inevent = event.yield;
		}		
	}
}

Pseries : Pattern {	// arithmetic series
	var <>start=0, <>step=1, <>length=inf;
	*new { arg start = 0, step = 1, length=inf;
		^super.newCopyArgs(start, step, length)
	}
	storeArgs { ^[start,step,length] }	

	embedInStream { arg inval;
		var outval, cur, counter = 0;
		cur = start;
		while ({counter < length}, {
			outval = cur;
			cur = cur + step;
			counter = counter + 1;
			inval = outval.yield;
		});
		^inval;
	}
}
		
Pgeom : Pattern {	// geometric series
	var <>start=1.0, <>grow=1.0, <>length=inf;
	*new { arg start = 0, grow = 1, length=inf;
		^super.newCopyArgs(start, grow, length)
	}
	storeArgs { ^[start,grow,length] }
	embedInStream { arg inval;
		var outval, cur, counter = 0;
		cur = start;
		while ({counter < length}, {
			outval = cur;
			cur = cur * grow;
			counter = counter + 1;
			inval = outval.yield;
		});
		^inval;
	}
}


Pbrown : Pattern {
	var <>lo, <>hi, <>step, <>length;
	*new { arg lo, hi, step, length=inf;
		^super.newCopyArgs(lo, hi, step, length)
	}
	storeArgs { ^[lo,hi,step,length] }
	embedInStream {
		var inval;
		var cur;
		cur = lo rrand: hi;
		length.do({
			cur = (cur + step.xrand2).fold(lo,hi);
			inval = cur.yield;				
		});
		^inval;
	}
}	

Pwhite : Pattern {
	var <>lo, <>hi, <>length;
	*new { arg lo, hi, length=inf;
		^super.newCopyArgs(lo, hi, length)
	}
	storeArgs { ^[lo,hi,length] }
	embedInStream { var inval;
		length.do({
			inval = rrand(lo,hi).yield;				
		});
		^inval;
	}
}	
				
Pstep2add : Pattern {
	var <>pattern1, <>pattern2;
	*new { arg pattern1, pattern2;
		^super.newCopyArgs(pattern1, pattern2)
	}
	storeArgs { ^[pattern1,pattern2] }
	embedInStream { arg inval;
		var stream1, stream2, val1, val2;
		
		stream1 = pattern1.asStream;
		while ({
			(val1 = stream1.next(inval)).notNil;
		},{
			stream2 = pattern2.asStream;
			while ({
				(val2 = stream2.next(inval)).notNil;
			},{
				inval = yield( val1 + val2 );
			});
		});
		^inval
	}
}

Pstep3add : Pattern {
	var <>pattern1, <>pattern2, <>pattern3;
	*new { arg pattern1, pattern2, pattern3;
		^super.newCopyArgs(pattern1, pattern2, pattern3)
	}
	storeArgs { ^[pattern1,pattern2,pattern3] }
	
	embedInStream { arg inval;
		var stream1, stream2, stream3, val1, val2, val3;
		
		stream1 = pattern1.asStream;
		while ({
			(val1 = stream1.next(inval)).notNil;
		},{
			stream2 = pattern2.asStream;
			while ({
				(val2 = stream2.next(inval)).notNil;
			},{
				stream3 = pattern3.asStream;
				while ({
					(val3 = stream3.next(inval)).notNil;
				},{
					inval = yield( val1 + val2 + val3 );
				});
			});
		});
		^inval;
	}
}


PstepNfunc : Pattern {
	var <function, <>patterns;
	*new { arg func, patterns;
		^super.newCopyArgs(func, patterns)
	}
	
	embedInStream { arg inval;
		var val;
		
		var f, streams, vals, size, max;
		size = patterns.size;
		max = size - 1;
		streams = Array.newClear(size);
		vals = Array.newClear(size);
		f = { arg inval, level=0;
				var val;
				streams[level] = patterns[level].asStream;
				while({
					vals[level] = val = streams[level].next(inval);
					val.notNil;
				}, {
					if(level < max, {
						f.value(inval, level + 1)
					}, {
						inval = yield(function.value(vals));
					})
				});
			inval;	
		};
		^f.value(inval);
	}
	
}

PstepNadd : PstepNfunc {
	*new { arg ... patterns;
		^super.new({ arg vals; vals.sum }, patterns)
	}
	storeArgs { ^patterns }
}

// returns relative time (in beats) from moment of embedding

Ptime : Pattern {
	var <>repeats;
	*new { arg repeats=inf;
		^super.newCopyArgs(repeats)
	}
	embedInStream {
		var start;
		start = thisThread.beats;
		repeats.do { (thisThread.beats - start).yield }
	}
}

