Pindex : Pattern {
	var listPat, indexPat, repeats;
	*new { arg listPat, indexPat, repeats=1;
		^super.newCopyArgs(listPat, indexPat, repeats)
	}
	embedInStream { arg inval;
		var indexStream, index, item;
		var listStream = listPat.asStream;
		repeats.do {
			var list = listStream.next;
			if (list.isNil) { ^inval };
			indexStream = indexPat.asStream;
			while {
				index = indexStream.next;
				index.notNil
			}{
				item = list.wrapAt(index);
				inval = item.embedInStream(inval);
			};
		};
		^inval;
	}
}

ListPattern : Pattern {
	var <>list, <>repeats=1;
	
	*new { arg list, repeats=1;
		if (list.size > 0) {
			^super.new.list_(list).repeats_(repeats)
		}{
			Error("ListPattern (" ++ this.name ++ ") requires a collection; received "
				++ list ++ ".").throw;
		}
	}
	copy {
		^super.copy.list_(list.copy)
	}
	storeArgs { ^[ list, repeats ] }
}

Pseq : ListPattern {
	var <>offset;
	*new { arg list, repeats=1, offset=0;
		^super.new(list, repeats).offset_(offset)
	}
	embedInStream {  arg inval;
		var item, offsetValue;
		offsetValue = offset.value;
		if (inval.eventAt('reverse') == true, {
			repeats.value.do({ arg j;
				list.size.reverseDo({ arg i;
					item = list.wrapAt(i + offsetValue);
					inval = item.embedInStream(inval);
				});
			});
		},{
			repeats.value.do({ arg j;
				list.size.do({ arg i;
					item = list.wrapAt(i + offsetValue);
					inval = item.embedInStream(inval);
				});
			});
		});
		^inval;
	}
	storeArgs { ^[ list, repeats, offset ] }
}

Pser : Pseq {
	embedInStream { arg inval;
		var item;
		var offsetValue = offset.value;
		if (inval.eventAt('reverse') == true, {
			repeats.value.reverseDo({ arg i;
				item = list.wrapAt(i + offsetValue);
				inval = item.embedInStream(inval);
			});
		},{
			repeats.value.do({ arg i;
				item = list.wrapAt(i + offsetValue);
				inval = item.embedInStream(inval);
			});
		});
		^inval;
	}
}	

Pshuf : ListPattern {
	embedInStream { arg inval;
		var item, stream;
		
		var localList = list.copy.scramble;
		repeats.value.do({ arg j;
			localList.size.do({ arg i;
				item = localList.wrapAt(i);
				inval = item.embedInStream(inval);
			});
		});
		^inval;
	}
}

Prand : ListPattern {
	embedInStream { arg inval;
		var item;
		
		repeats.value.do({ arg i;
			item = list.at(list.size.rand);
			inval = item.embedInStream(inval);
		});
		^inval;
	}
}		

Pxrand : ListPattern {
	embedInStream { arg inval;			
		var item, size;
		var index = list.size.rand;
		repeats.value.do({ arg i;
			size = list.size;
			index = (index + (size - 1).rand + 1) % size;
			item = list.at(index);
			inval = item.embedInStream(inval);
		});
		^inval;
	}
}		

Pwrand : ListPattern {
	var <>weights;
	*new { arg list, weights, repeats=1;
		^super.new(list, repeats).weights_(weights)
	}
	embedInStream {  arg inval;
		var item;
		repeats.value.do({ arg i;
			item = list.at(weights.windex);
			inval = item.embedInStream(inval);
		});
		^inval
	}
	storeArgs { ^[ list, weights, repeats ] }
}		


Pfsm : ListPattern {
	embedInStream {  arg inval;
		var item, index=0;
		var maxState = ((list.size - 1) div: 2) - 1;
		repeats.value.do({
			index = 0;
			while({
				index = list.at(index).choose.clip(0, maxState) * 2 + 2;
				item = list.at(index - 1);
				item.notNil
			},{
				inval = item.embedInStream(inval);
			});
		});
		^inval;
	}
}

Pdfsm : ListPattern {
	var >startState;
	*new { arg list, startState=0, repeats=1;
		^super.new( list, repeats ).startState_(startState)	}
	embedInStream { arg inval;
		var currState, sigStream;
		var sig, state, stream;
		var numStates = list.size - 1;
		repeats.do({
			
			currState = startState;
			sigStream = list[0].asStream;
			
			while({
				sig = sigStream.next;
				state = list[currState + 1];
				if( sig.isNil, { false }, {
					if( state.includesKey(sig), {
						#currState, stream = state[sig];
					}, {
						#currState, stream = state[\default];
					});
					currState.notNil and: {currState < numStates};
				})
			}, {
				inval = stream.embedInStream(inval);
			})
		});
		^inval;
	}
}
	
//Ppar : ListPattern {
//	initStreams { arg priorityQ;
//		list.do({ arg pattern, i; 
//			priorityQ.put(0.0, pattern.asStream);
//		});
//	}
//	asStream {
//		^Routine({ arg inval;
//			var count = 0, join, cond;
//			join = list.size;
//			cond = Condition({ count >= join });
//			list.do({ arg func; 
//				Routine({ arg time;
//					inval.
//					pattern.embedInStream(inval.copy);
//					count = count + 1;
//					cond.signal;
//				}).play;
//			});
//			cond.wait;
//		});
//	}
//}

Ppar : ListPattern {
	initStreams { arg priorityQ;
		list.do({ arg pattern, i; 
			priorityQ.put(0.0, pattern.asStream);
		});
	}
		
// tests for nil events and relays them to all child streams
// nil events come from Pfin, Pfindur, and PatternConductor 
// to force streams to end early
	embedInStream { arg inval;
		var assn;
		var priorityQ = PriorityQueue.new;
	
		repeats.value.do({ arg j;
			var outval, stream, nexttime, now = 0.0;

			this.initStreams(priorityQ);
			
			// if first event not at time zero
			if (priorityQ.notEmpty and: { (nexttime = priorityQ.topPriority) > 0.0 }, {
				outval = inval.copy;
				outval.put(\freq, \rest);					
				outval.put(\delta, nexttime);
				
				inval = outval.yield 
					?? {while {priorityQ.notEmpty} { priorityQ.pop.next(nil) } };
				now = nexttime;	
			});
			
			while({
				priorityQ.notEmpty
			},{
				stream = priorityQ.pop;
				outval = stream.next(inval);
				if (outval.isNil, {
					nexttime = priorityQ.topPriority;
					if (nexttime.notNil, {
						// that child stream ended, so rest until next one
						outval = inval.copy;
						outval.put(\freq, \rest);					
						outval.put(\delta, nexttime - now);
						
						inval = outval.yield 
							?? {while {priorityQ.notEmpty} { priorityQ.pop.next(nil) } };
						now = nexttime;	
					},{
						priorityQ.clear;
					});		
				},{			
					// requeue stream
					priorityQ.put(now + outval.delta, stream);
					nexttime = priorityQ.topPriority;
					outval.put(\delta, nexttime - now);
					
					inval = outval.yield 
						?? {while {priorityQ.notEmpty} { priorityQ.pop.next(nil) } };
					now = nexttime;	
				});	
			});
		});
		^inval;
	}
}	

Ptpar : Ppar {
	initStreams { arg priorityQ;
		forBy(0, list.size-1, 2, { arg i;
			priorityQ.put(list.at(i).value, list.at(i+1).asStream);
		});
	}
}

Pswitch : Pattern {
	var <>list, <>which=0;
	*new { arg list, which=0;
		^super.new.list_(list).which_(which)
	}
	embedInStream {  arg inval;
		var item, index;
		
		var indexStream = which.asStream;
		while ({
			(index = indexStream.next).notNil;
		},{
			inval = list.wrapAt(index.asInteger).embedInStream(inval);
		});
		^inval;
	}
	storeArgs { ^[ list, which ]  }
}

Pswitch1 : Pswitch {	
	embedInStream { arg inval;
		var index, outval;
		
		var streamList = list.collect({ arg pattern; pattern.asStream; });
		var indexStream = which.asStream;
		loop {
			if ((index = indexStream.next).isNil) { ^inval };
			outval = streamList.wrapAt(index.asInteger).next(inval);
			if (outval.isNil) { ^inval };
			inval = outval.yield;
		};
	}
}	


//Pswitch1 : Pattern {	
//	var <>list, <>which=0;
//	*new { arg list, which=0;
//		^super.new.list_(list).which_(which)
//	}
//	
//	asStream { 
//		var streamList, indexStream;
//		
//		streamList = list.collect({ arg pattern; pattern.asStream; });
//		indexStream = which.asStream;
//		
//		^FuncStream.new({ arg inval;
//			var index;
//			if ((index = indexStream.next).notNil, {
//				streamList.wrapAt(index.asInteger).next(inval);
//			});
//		},{
//			streamList.do({ arg stream; stream.reset; });
//		});
//	}
//	storeArgs { ^[ list, which ]  }
//}	

Ptuple : ListPattern {
	embedInStream {  arg inval;
		var item, streams, tuple, outval;
					
		repeats.value.do({ arg j;
			var sawNil = false;
			streams = list.collect({ arg item; item.asStream });
							
			while ({
				tuple = Array.new(streams.size);
				streams.do({ arg stream; 
					outval = stream.next(inval);
					if (outval.isNil, { sawNil = true; });
					tuple.add(outval);
				});
				sawNil.not
			},{
				inval = yield(tuple);
			});
		});
		^inval;
	}
}

Place : Pseq {
	embedInStream {  arg inval;
		var item;
		
		var offsetValue = offset.value;
		if (inval.eventAt('reverse') == true, {
			repeats.value.do({ arg j;
				list.size.reverseDo({ arg i;
					item = list.wrapAt(i + offsetValue);
					if (item.isSequenceableCollection, {
						item = item.wrapAt(j);
					});
					inval = item.embedInStream(inval);
				});
			});
		},{
			repeats.value.do({ arg j;
				list.size.do({ arg i;
					item = list.wrapAt(i + offsetValue);
					if (item.isSequenceableCollection, {
						item = item.wrapAt(j);
					});
					inval = item.embedInStream(inval);
				});
			});
		});
		^inval;
	}
}

// similar to Place, but the list is an array of Patterns or Streams
Ppatlace : Pseq {
	embedInStream { |inval|
		var	consecutiveNils = 0, index, repeat, item;
		var streamList = list.collect({ |item| item.asStream });
		var offsetValue = offset.value;
		index = repeat = 0;
		{ (repeat < repeats) and: { consecutiveNils < list.size } }.while({
			(inval.eventAt(\reverse) == true).if({
				item = streamList.wrapAt(offsetValue - index - 1);
			}, {
				item = streamList.wrapAt(offsetValue + index);
			});
			(item = item.next).notNil.if({
				consecutiveNils = 0;
				inval = item.embedInStream(inval);
			}, {
				consecutiveNils = consecutiveNils + 1;
			});
			((index = index + 1) == list.size).if({
				index = 0;
				repeat = repeat + 1;
			});
		});
		^inval;
	}
}

Pslide : ListPattern {
    // 'repeats' is the number of segments.
    // 'len' is the length of each segment.
    // 'step' is how far to step the start of each segment from previous.
    // 'start' is what index to start at.
    // indexing wraps around if goes past beginning or end.
    // step can be negative.

    var <>len, <>step, <>start;
    *new { arg list, repeats = 1, len = 3, step = 1, start = 0;
        ^super.new(list, repeats).len_(len).step_(step).start_(start)
    }
    embedInStream { arg inval;
	    	var item;
	    	var pos = start; 
	    	repeats.do({
	    		len.do({ arg j;
	    			item = list.wrapAt(pos + j);
	    			inval = item.embedInStream(inval);
	    		});
	    		pos = pos + step;
	    	});
	     ^inval;  		
    }
}

Pwalk : ListPattern {
		// random walk pattern - hjh - jamshark70@yahoo.com
	
	var	<>startPos,	// starting index
		<>stepPattern,	// pattern for steps
		<>directionPattern;	// pattern should return a stream of:
							// 1 to move in direction of stepPattern
							// -1 to reverse the direction of stepPattern
							// a new direction will be chosen when the walker
							// reaches a boundary
	
	*new { arg list, stepPattern, directionPattern = 1, startPos = 0;
		^super.new(list).startPos_(startPos)
			.stepPattern_(stepPattern ?? { Prand([-1, 1], inf) })
			.directionPattern_(directionPattern ? 1);
	}
	
	storeArgs { ^[list, stepPattern, directionPattern, startPos] }
	
	copy {
		^super.copy.list_(list.copy).stepPattern_(stepPattern)
			.directionPattern_(directionPattern);
	}
	
	embedInStream { arg inval;
		var	step;	
		var index = startPos;
		var stepStream = stepPattern.asStream;
		var directionStream = directionPattern.asStream;
		// 1 = use steps as is; -1 = reverse direction
		var direction = directionStream.next ? 1;	// start with first value

		{ (step = stepStream.next).notNil }.while({  // get step, stop when nil
			inval = list[index].embedInStream(inval);  // pop value/stream out
			step = step * direction;	// apply direction
				// if next thing will be out of bounds
			(((index + step) < 0) or: ((index + step) >= list.size)).if({
				direction = directionStream.next ? 1;  // next direction, or 1
				step = step.abs * direction.sign;  // apply to this step
			});
			index = (index + step) % list.size;
		});

		^inval;
	}
}
