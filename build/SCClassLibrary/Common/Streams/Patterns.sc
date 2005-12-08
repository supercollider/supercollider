
Pattern : AbstractFunction {
	
	
	// concatenate Patterns
	++ { arg aPattern;
		^Pseq.new([this, aPattern])
	}
	// compose Patterns
	<> { arg aPattern;
		^Pchain(this, aPattern)
	}
	
	play { arg clock, protoEvent, quant=1.0;
		^this.asEventStreamPlayer(protoEvent).play(clock, false, quant)
	}

	asStream { ^Routine({ arg inval; this.embedInStream(inval) }) }
	iter { ^this.asStream }

	asEventStreamPlayer { arg protoEvent;
		^EventStreamPlayer(this.asStream, protoEvent);
	}
	embedInStream { arg inval;
		^this.asStream.embedInStream(inval);
	}
	do { arg function;
		this.asStream.do(function)
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
	
	trace { arg key, printStream, prefix=""; ^Ptrace(this, key, printStream, prefix) }

	
	//////////////////////
	
	mtranspose { arg n; ^Paddp(\mtranspose, n, this) }
	ctranspose { arg n; ^Paddp(\ctranspose, n, this) }
	gtranspose { arg n; ^Paddp(\gtranspose, n, this) }
	detune { arg n; ^Paddp(\detune, n, this) }

	scaleDur { arg x; ^Pmulp(\dur, x, this) }
	addDur { arg x; ^Paddp(\dur, x, this) }
	stretch { arg x; ^Pmulp(\stretch, x, this) }
	lag { arg t; ^Plag(t, this) }

	legato { arg x; ^Pmulp(\legato, x, this) }

	db { arg db; ^Paddp(\db, db, this) }
	
	clump { arg n; ^Pclump(n, this) }
	flatten { arg n = 1; ^Pflatten(n, this) }
	repeat { arg n=inf; ^Pn(this, n) }
	keep { arg n; ^Pfin(n, this) }
	drop { arg n; ^Pdrop(n, this) }
	stutter { arg n; ^Pstutter(n, this) }
	finDur { arg dur, tolerance = 0.001; ^Pfindur(dur, this, tolerance) }
	fin { arg n; ^Pfin(n, this) }
	
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
		var stream = a.asStream;
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
		var streamA = a.asStream;
		var streamB = b.asStream;
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
		var streamA = a.asStream;
		var streamlist = arglist.collect({ arg item; item.asStream });
		^NAryOpStream.new(operator, streamA, streamlist);
	}
}

Pchain : Pattern {
	var <>patterns;
	*new { arg ... patterns;
		^super.newCopyArgs(patterns);
	}
	<> { arg aPattern;
		var list;
		list = patterns.copy.add(aPattern);
		^this.class.new(*list)
	}
	embedInStream { arg inval;
		var streams, inevent;
		streams = patterns.collect(_.asStream);
		loop {
			streams.reverseDo { |str|
				inevent = str.next(inval);
				if(inevent.isNil) { ^inval };
				inval = inevent;
			};
			inval = yield(inevent);
			if (inval.isNil) {
				streams.reverseDo { |str|
					str.next(inval);
				};
				^nil.yield;
			};			
		};
	}
	storeOn { arg stream;
			stream << "("; 
			patterns.do { |item,i|  if(i != 0) { stream << " <> " }; stream <<< item; }; 
			stream << ")"
	}
}


Pevent : Pattern {
	var <>pattern, <>event;
	
	*new { arg pattern, event;
		^super.newCopyArgs(pattern, event);
	}
	storeArgs { ^[pattern, event] }
	embedInStream { arg inval;
		var outval;
		var stream = pattern.asStream;
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
		var event;
		var sawNil = false;		
		var streampairs = patternpairs.copy;
		var endval = streampairs.size - 1;

		forBy (1, endval, 2) { arg i;
			streampairs.put(i, streampairs[i].asStream);
		};

		loop {
			if (inevent.isNil) { ^nil.yield };
			event = inevent.copy;
			forBy (0, endval, 2) { arg i;
				var name = streampairs[i];
				var stream = streampairs[i+1];		
				var streamout = stream.next(event);
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

Pmono : Pattern {
	var <>synthName, <>patternpairs;
	*new { arg name ... pairs;
		if (pairs.size.odd, { Error("Pmono should have even number of args.\n").throw; });
		^super.newCopyArgs(name, pairs)
	}
	
	embedInStream { | inevent |
		var id, server;
		var event;
		var streamout, name;
		var patMadeSynth;
		var streampairs = patternpairs.copy;
		var endval = patternpairs.size - 1;
		var synthLib, desc, msgFunc, hasGate;
		forBy (1, endval, 2) { | i | streampairs[i] = patternpairs[i].asStream };

		inevent ?? { ^nil.yield };

		event = inevent.copy;
		synthLib = event[\synthLib] ?? { SynthDescLib.global };
		desc = synthLib.synthDescs[synthName.asSymbol];			if (desc.notNil) { 
			msgFunc = desc.msgFunc;
			hasGate = desc.hasGate;
		}{
			msgFunc = event[\defaultMsgFunc];
			hasGate = true;
		};
		
		if (event[\id].notNil) {
			event[\type] = \monoSet;	
			patMadeSynth = false;
		} {
			event[\type] = \monoNote; 
			event[\instrument] = synthName;
			event[\updatePmono] = { | argID, argServer | 
				 id = argID;
				 server = argServer;
				 patMadeSynth = true; 
			};
		}; 
		loop {
			event[\msgFunc] = msgFunc;
			forBy (0, endval, 2) { | i |
				name = streampairs[i];
				streamout = streampairs[i+1].next(event);
				streamout ?? { 					// exit on nil field
					if (patMadeSynth) { 			// end synth if made by Pmono
						event[\type] = \monoOff;
						event[\hasGate] = hasGate;
						event.play; 
					};
					^inevent
				 };
				if (name.isSequenceableCollection) {
					name.do { | n, i | event[n] = streamout[i] };
				}{
					event[name] = streamout;
				};			
			};
			inevent = event.yield;			
			if (inevent.isNil) {					// pattern is ending early
				if ( patMadeSynth) {				// from pfin, pfindur, PatternConductor
					event.use {
						~finish.value;
						~eventTypes[\off].value(~server ?? { Server.default });
					};
					^nil.yield
				} {
					^nil.yield
				}
			};			
			event = inevent.copy;
			if (patMadeSynth) {
				event[\server] = server;
				event[\id] = id;
			};
			event[\type] = \monoSet;
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
		var outval, counter = 0;
		var cur = start;
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
		var outval, counter = 0;
		var cur = start;
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
	embedInStream { arg inval;
		var cur = lo rrand: hi;
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
	embedInStream { arg inval;
		length.do({
			inval = rrand(lo,hi).yield;				
		});
		^inval;
	}
}

Pprob : Pattern {
	var <>hi, <>lo, <>length, <tableSize, <distribution, <table;
	
	*new { arg distribution, lo=0.0, hi=1.0, length=inf, tableSize;
		^super.newCopyArgs(hi, lo, length, tableSize).distribution_(distribution);
	}
	distribution_ { arg list;
		var n = tableSize ?? { max(64, list.size) }; // resample, if too small
		distribution = list;
		table = distribution.asRandomTable(n);
	}
	tableSize_ { arg n;
		tableSize = n;
		this.distribution_(distribution);
	}
	embedInStream { arg inval;
		length.value.do {
			inval = ((table.tableRand * (hi - lo)) + lo).yield;		};
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
		while {
			(val1 = stream1.next(inval)).notNil;
		}{
			stream2 = pattern2.asStream;
			while {
				(val2 = stream2.next(inval)).notNil;
			}{
				inval = yield( val1 + val2 );
			};
		};
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
		while {
			(val1 = stream1.next(inval)).notNil;
		}{
			stream2 = pattern2.asStream;
			while {
				(val2 = stream2.next(inval)).notNil;
			}{
				stream3 = pattern3.asStream;
				while {
					(val3 = stream3.next(inval)).notNil;
				}{
					inval = yield( val1 + val2 + val3 );
				};
			};
		};
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
		var size = patterns.size;
		var max = size - 1;
		var streams = Array.newClear(size);
		var vals = Array.newClear(size);
		var f = { arg inval, level=0;
				var val;
				streams[level] = patterns[level].asStream;
				while{
					vals[level] = val = streams[level].next(inval);
					val.notNil;
				}{
					if(level < max) {
						f.value(inval, level + 1)
					}{
						inval = yield(function.value(vals));
					}
				};
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
	embedInStream { arg inval;
		var start = thisThread.beats;
		repeats.do { inval = (thisThread.beats - start).yield };
		^inval
	}
}

