Pindex : Pattern {
	var listPat, indexPat, repeats;
	*new { arg listPat, indexPat, repeats=1;
		^super.newCopyArgs(listPat, indexPat, repeats)
	}
	asStream {
		^Routine.new({ arg inval;
			var listStream, indexStream, list, index, item;
			listStream = listPat.asStream;
			repeats.do {
				list = listStream.next;
				if (list.isNil) { nil.alwaysYield };
				indexStream = indexPat.asStream;
				while {
					index = indexStream.next;
					index.notNil
				}{
					item = list.wrapAt(index);
					inval = item.embedInStream(inval);
				};
			}
		})
	}
}

ListPattern : Pattern {
	var <>list, <>repeats=1;
	
	*new { arg list, repeats=1;
		^super.new.list_(list).repeats_(repeats)
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
	asStream { 
		^Routine.new({ arg inval;
			var item, offsetValue;
			offsetValue = offset.value;
			if (inval.eventAt('reverse') == true, {
				repeats.value.do({ arg j;
					list.size.reverseDo({ arg i;
						item = list @@ (i + offsetValue);
						inval = item.embedInStream(inval);
					});
				});
			},{
				repeats.value.do({ arg j;
					list.size.do({ arg i;
						item = list @@ (i + offsetValue);
						inval = item.embedInStream(inval);
					});
				});
			});
		});
	}
	storeArgs { ^[ list, repeats, offset ] }
}

Pser : Pseq {
	asStream {
		^Routine.new({ arg inval;
			var item, offsetValue;
			
			offsetValue = offset.value;
			if (inval.eventAt('reverse') == true, {
				repeats.value.reverseDo({ arg i;
					item = list @@ (i + offsetValue);
					inval = item.embedInStream(inval);
				});
			},{
				repeats.value.do({ arg i;
					item = list @@ (i + offsetValue);
					inval = item.embedInStream(inval);
				});
			});
		});
	}
}	

Pshuf : ListPattern {
	asStream { 
		^Routine.new({ arg inval;
			var localList, item, stream;
			
			localList = list.copy.scramble;
			repeats.value.do({ arg j;
				localList.size.do({ arg i;
					item = localList @@ i;
					inval = item.embedInStream(inval);
				});
			});
		});
	}
}

Prand : ListPattern {
	asStream { 
		^Routine.new({ arg inval;
			var item;
			
			repeats.value.do({ arg i;
				item = list.at(list.size.rand);
				inval = item.embedInStream(inval);
			});
		});
	}
}		

Pxrand : ListPattern {
	asStream { 
		^Routine.new({ arg inval;			
			var item, index, size;
			index = list.size.rand;
			repeats.value.do({ arg i;
				size = list.size;
				index = (index + (size - 1).rand + 1) % size;
				item = list.at(index);
				inval = item.embedInStream(inval);
			});
		});
	}
}		

Pwrand : ListPattern {
	var <>weights;
	*new { arg list, weights, repeats=1;
		^super.new(list, repeats).weights_(weights)
	}
	asStream { 
		^Routine.new({ arg inval;
			var item;
			repeats.value.do({ arg i;
				item = list.at(weights.windex);
				inval = item.embedInStream(inval);
			});
		});
	}
	storeArgs { ^[ list, weights, repeats ] }
}		


Pfsm : ListPattern {
	asStream { 
		^Routine.new({ arg inval;
			var item, index=0, maxState;
			maxState = ((list.size - 1) div: 2) - 1;
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
		});
	}
}

Pdfsm : ListPattern {
	var >startState;
	*new { arg list, startState=0, repeats=1;
		^super.new( list, repeats ).startState_(startState)	}
	asStream {
		^Routine({ arg inval;
			var currState, sigStream, numStates;
			var sig, state, stream;
			numStates = list.size - 1;
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
		})
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
	asStream {
		var priorityQ, assn;
		
		priorityQ = PriorityQueue.new;
		
		^Routine.new({ arg inval;
		
			repeats.value.do({ arg j;
				var outval, stream, nexttime, now = 0.0;

				this.initStreams(priorityQ);
				
				// if first event not at time zero
				if (priorityQ.notEmpty and: { (nexttime = priorityQ.topPriority) > 0.0 }, {
					outval = inval.copy;
					outval.put(\freq, \rest);					
					outval.put(\delta, nexttime);
					
					inval = outval.yield;
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
							
							inval = outval.yield;
							now = nexttime;	
						},{
							priorityQ.clear;
						});		
					},{			
						// requeue stream
						priorityQ.put(now + outval.delta, stream);
						nexttime = priorityQ.topPriority;
						outval.put(\delta, nexttime - now);
						
						inval = outval.yield;
						now = nexttime;	
					});	
				});
			});
		});
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
	asStream { 
		^Routine.new({ arg inval;
			var item, index, indexStream;
			
			indexStream = which.asStream;
			while ({
				(index = indexStream.next).notNil;
			},{
				inval = list.wrapAt(index.asInteger).embedInStream(inval);
			});
		});
	}
	storeArgs { ^[ list, which ]  }
}

Pswitch1 : Pswitch {	
	asStream { 
		var streamList, indexStream;
		
		streamList = list.collect({ arg pattern; pattern.asStream; });
		indexStream = which.asStream;
		
		^FuncStream.new({ arg inval;
			var index;
			if ((index = indexStream.next).notNil, {
				streamList.wrapAt(index.asInteger).next(inval);
			});
		},{
			streamList.do({ arg stream; stream.reset; });
		});
	}
}	

Ptuple : ListPattern {
	asStream { 
		^Routine.new({ arg inval;
									
			repeats.value.do({ arg j;
				var item, streams, tuple, outval, sawNil = false;
				
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
		});
	}
}

Place : Pseq {
	asStream { 
		^Routine.new({ arg inval;
			var item, offsetValue;
			
			offsetValue = offset.value;
			if (inval.eventAt('reverse') == true, {
				repeats.value.do({ arg j;
					list.size.reverseDo({ arg i;
						item = list @@ (i + offsetValue);
						if (item.isSequenceableCollection, {
							item = item @@ j;
						});
						inval = item.embedInStream(inval);
					});
				});
			},{
				repeats.value.do({ arg j;
					list.size.do({ arg i;
						item = list @@ (i + offsetValue);
						if (item.isSequenceableCollection, {
							item = item @@ j;
						});
						inval = item.embedInStream(inval);
					});
				});
			});
		});
	}
}

// similar to Place, but the list is an array of Patterns or Streams
Ppatlace : Pseq {
	asStream {
		^Routine({ |inval|
			var	streamList, consecutiveNils = 0, index, repeat, offsetValue, item;
			streamList = list.collect({ |item| item.asStream });
			offsetValue = offset.value;
			index = repeat = 0;
			{ (repeat < repeats) and: { consecutiveNils < list.size } }.while({
				(inval.eventAt(\reverse) == true).if({
					item = streamList @@ (offsetValue - index - 1);
				}, {
					item = streamList @@ (offsetValue + index);
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
			nil.alwaysYield;
		});
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
    asStream {
        ^Routine({ arg inval;
            var pos, item;
            pos = start;
            repeats.do({
                len.do({ arg j;
                    item = list.wrapAt(pos + j);
                    inval = item.embedInStream(inval);
                });
                pos = pos + step;
            });
        });
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
	
	asStream {
		^Routine({ arg inval;
			var	index, step, stepStream, directionStream,
				direction;	// 1 = use steps as is; -1 = reverse direction
			
			index = startPos;
			stepStream = stepPattern.asStream;
			directionStream = directionPattern.asStream;
			direction = directionStream.next ? 1;	// start with first value

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

			nil.yield;
		});
	}
}
