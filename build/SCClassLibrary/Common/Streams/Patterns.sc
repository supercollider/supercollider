
Pattern : AbstractFunction {
	
	
	// concatenate Patterns
	++ { arg aPattern;
		^Pseq.new([this, aPattern])
	}	

	play { arg protoEvent, clock;
		Pevent(this, protoEvent).play(clock)
	}
	
	asStream {
		^this.subclassResponsibility(thisMethod)
	}
	asEventStream { arg protoEvent;
		^EventStream(this,protoEvent);
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
	composeBinaryOp { arg operator, pattern;
		^Pbinop.new(operator, this, pattern)
	}
	reverseComposeBinaryOp { arg operator, pattern;
		^Pbinop.new(operator, pattern, this)
	}
	composeNAryOp { arg selector, argList;
		^this.notYetImplemented
	}
}

Pevent : Pattern {
	var <>pattern, <>event;
	*new { arg pattern, event;
		^super.newCopyArgs(pattern, event ? Event.protoEvent);
	}
	asStream {
		^EventStream(pattern.asStream, event);
	}
	play { arg clock;
		this.asStream.play(clock ? SystemClock)
	}
}


Pfunc : Pattern {
	var <>nextFunc; // Func is evaluated for each next state
	var <>resetFunc; // Func is evaluated for each next state
	*new { arg nextFunc, resetFunc;	
		^super.newCopyArgs(nextFunc, resetFunc)
	}
	asStream {
		^FuncStream.new(nextFunc, resetFunc)
	}
}

Prout : Pattern {
	var <>routineFunc;
	*new { arg routineFunc;
		^super.newCopyArgs(routineFunc)
	}
	asStream {
		^Routine.new(routineFunc)
	}
}

Pfuncn : Pattern {
	var <>func, <>repeats;
	*new { arg func, repeats = 1;
		^super.newCopyArgs(func, repeats)
	}
	asStream { 
		^Routine.new({ arg inval;
			repeats.value.do({
				inval = func.value(inval).yield;
			});
		});
	}
}	

// Punop and Pbinop are used to create patterns in response to math operations
Punop : Pattern {
	var <>operator, <>a;
	*new { arg operator, a;
		^super.newCopyArgs(operator, a)
	}
	asStream {
		var stream;
		stream = a.asStream;
		^UnaryOpStream.new(operator, stream);
	}
}

Pbinop : Pattern {
	var <>operator, <>a, <>b;
	*new { arg operator, a, b;
		^super.newCopyArgs(operator, a, b)
	}
	asStream {
		var streamA, streamB;
		streamA = a.asStream;
		streamB = b.asStream;
		^BinaryOpStream.new(operator, streamA, streamB);
	}
}


Pbind : Pattern {
	var <>patternpairs;
	*new { arg ... pairs;
		if (pairs.size.odd, { "Pbind should have even number of args.\n".error; this.halt });
		^super.new.patternpairs_(pairs)
	}
	asStream {
		var streampairs, endval;
		
		streampairs = patternpairs.copy;
		endval = streampairs.size - 1;
		forBy (1, endval, 2, { arg i;
			streampairs.put(i, streampairs.at(i).asStream);
		});
		
		^FuncStream.new({ arg inval;
			var sawNil = false;
			inval = inval.copy;
			forBy (0, endval, 2, { arg i;
				var name, stream, streamout;
				name = streampairs.at(i);
				stream = streampairs.at(i+1);
				
				streamout = stream.next(inval);
				
				if (streamout.isNil, {
					sawNil = true;
				},{
					if (name.isSequenceableCollection, {					
						streamout.do({ arg val, i;
							inval.put(name.at(i), val);
						});
					},{
						inval.put(name, streamout);
					});
				});
			});
			if (sawNil, { nil },{ 
				inval 
			});
		},{
			streampairs = patternpairs.copy;
			endval = streampairs.size - 1;
			forBy (1, endval, 2, { arg i;
				streampairs.put(i, streampairs.at(i).asStream);
			});
		});
	}
}

Pseries : Pattern {	// arithmetic series
	var <>start=0, <>step=1, <>length=inf;
	*new { arg start = 0, step = 1, length=inf;
		^super.newCopyArgs(start, step, length)
	}	
	asStream {
		var cur, counter = 0;
		cur = start;
		^FuncStream.new({
			var outval;
			if (counter >= length, {
				nil
			},{
				outval = cur;
				cur = cur + step;
				counter = counter + 1;
				outval
			});
		},{
			cur = start;
			counter = 0;
		});
	}
}
		
Pgeom : Pattern {	// geometric series
	var <>start=1.0, <>grow=1.0, <>length=inf;
	*new { arg start = 0, grow = 1, length=inf;
		^super.newCopyArgs(start, grow, length)
	}	
	asStream {
		var cur, counter = 0;
		cur = start;
		^FuncStream.new({
			var outval;
			if (counter >= length, {
				nil
			},{
				outval = cur;
				cur = cur * grow;
				counter = counter + 1;
				outval
			});
		},{
			cur = start;
			counter = 0;
		});
	}
}

Pbrown : Pattern {
	var <>lo, <>hi, <>step, <>length;
	*new { arg lo, hi, step, length=inf;
		^super.newCopyArgs(lo, hi, step, length)
	}	
	asStream {
		^Routine.new({
			var cur;
			cur = lo rrand: hi;
			length.do({
				cur = (cur + step.xrand2).fold(lo,hi);
				cur.yield;				
			});
		});
	}
}	

Pwhite : Pattern {
	var <>lo, <>hi, <>length;
	*new { arg lo, hi, length=inf;
		^super.newCopyArgs(lo, hi, length)
	}	
	asStream {
		^Routine.new({
			length.do({
				rrand(lo,hi).yield;				
			});
		});
	}
}	
				
Pstep2add : Pattern {
	var <>pattern1, <>pattern2;
	*new { arg pattern1, pattern2;
		^super.newCopyArgs(pattern1, pattern2)
	}	
	asStream {
		^Routine.new({ arg inval;
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
		})		
	}
}

Pstep3add : Pattern {
	var <>pattern1, <>pattern2, <>pattern3;
	*new { arg pattern1, pattern2, pattern3;
		^super.newCopyArgs(pattern1, pattern2, pattern3)
	}	
	asStream {
		^Routine.new({ arg inval;
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
		})		
	}
}


