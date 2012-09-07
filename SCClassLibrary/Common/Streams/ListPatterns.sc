Pindex : Pattern {
	var listPat, indexPat, repeats;
	*new { arg listPat, indexPat, repeats=1;
		^super.newCopyArgs(listPat, indexPat, repeats)
	}
	storeArgs { ^[listPat,indexPat,repeats] }
	embedInStream { arg inval;
		var indexStream, index, item, itemCount;
		var listStream = listPat.asStream;
		repeats.value(inval).do {
			var list = listStream.next(inval);
			if (list.isNil) { ^inval };
			indexStream = indexPat.asStream;
			itemCount = 0;
			while {
				index = indexStream.next(inval);
				index.notNil
			}{
				itemCount = itemCount + 1;
				item = list.wrapAt(index);
				inval = item.embedInStream(inval);
			};
			if(itemCount == 0) { ^inval }
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
			Error("ListPattern (" ++ this.name ++ ") requires a non-empty collection; received "
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
		offsetValue = offset.value(inval);
		if (inval.eventAt('reverse') == true, {
			repeats.value(inval).do({ arg j;
				list.size.reverseDo({ arg i;
					item = list.wrapAt(i + offsetValue);
					inval = item.embedInStream(inval);
				});
			});
		},{
			repeats.value(inval).do({ arg j;
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
		var offsetValue = offset.value(inval);
		if (inval.eventAt('reverse') == true, {
			repeats.value(inval).reverseDo({ arg i;
				item = list.wrapAt(i + offsetValue);
				inval = item.embedInStream(inval);
			});
		},{
			repeats.value(inval).do({ arg i;
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

		repeats.value(inval).do({ arg j;
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

		repeats.value(inval).do({ arg i;
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
		repeats.value(inval).do({ arg i;
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
		var item, wVal;
		var wStr = weights.asStream;
		repeats.value(inval).do({ arg i;
			wVal = wStr.next(inval);
			if(wVal.isNil) { ^inval };
			item = list.at(wVal.windex);
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
		repeats.value(inval).do({
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
	var <>startState;
	*new { arg list, startState=0, repeats=1;
		^super.new( list, repeats ).startState_(startState)
	}

	embedInStream { arg inval;
		var currState, sigStream;
		var sig, state, stream;
		var numStates = list.size - 1;
		repeats.value(inval).do({

			currState = startState;
			sigStream = list[0].asStream;

			while({
				sig = sigStream.next(inval);
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

Pswitch : Pattern {
	var <>list, <>which=0;
	*new { arg list, which=0;
		^super.new.list_(list).which_(which)
	}
	embedInStream {  arg inval;
		var item, index;

		var indexStream = which.asStream;
		while ({
			(index = indexStream.next(inval)).notNil;
		},{
			inval = list.wrapAt(index.asInteger).embedInStream(inval);
		});
		^inval;
	}
	storeArgs { ^[ list, which ]  }
}

Pswitch1 : Pswitch {
	embedInStream { arg inval;
		var cleanup = EventStreamCleanup.new;
		var index, outval;
		var streamList = list.collect({ arg pattern; pattern.asStream; });
		var indexStream = which.asStream;

		loop {
			if ((index = indexStream.next(inval)).isNil) { ^cleanup.exit(inval) };
			outval = streamList.wrapAt(index.asInteger).next(inval);
			if (outval.isNil) { ^cleanup.exit(inval) };
			cleanup.update(outval);
			inval = outval.yield;
		};
	}
}

Ptuple : ListPattern {
	embedInStream {  arg inval;
		var item, streams, tuple, outval;

		repeats.value(inval).do({ arg j;
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
		var offsetValue = offset.value(inval);

		if (inval.eventAt('reverse') == true, {
			repeats.value(inval).do({ arg j;
				list.size.reverseDo({ arg i;
					item = list.wrapAt(i + offsetValue);
					if (item.isSequenceableCollection, {
						item = item.wrapAt(j);
					});
					inval = item.embedInStream(inval);
				});
			});
		},{
			repeats.value(inval).do({ arg j;
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
		var offsetValue = offset.value(inval);
		var localRepeats = repeats.value(inval);

		index = repeat = 0;
		while { (repeat < localRepeats) and: { consecutiveNils < list.size } } {
			if(inval.eventAt(\reverse) == true) {
				item = streamList.wrapAt(offsetValue - index - 1);
			} {
				item = streamList.wrapAt(offsetValue + index);
			};
			if((item = item.next(inval)).notNil) {
				consecutiveNils = 0;
				inval = item.embedInStream(inval);
			} {
				consecutiveNils = consecutiveNils + 1;
			};
			if((index = index + 1) == list.size) {
				index = 0;
				repeat = repeat + 1;
			};
		};
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

    var <>len, <>step, <>start, <>wrapAtEnd;
    *new { arg list, repeats = 1, len = 3, step = 1, start = 0, wrapAtEnd = true;
        ^super.new(list, repeats).len_(len).step_(step).start_(start)
			.wrapAtEnd_(wrapAtEnd);
    }
    embedInStream { arg inval;
    	var item;
    	var pos = start;
    	var stepStr = step.asStream, stepVal;
    	var lengthStr = len.asStream, lengthVal;

	   	repeats.value(inval).do {
			lengthVal = lengthStr.next(inval);
	    		if(lengthVal.isNil) { ^inval };
			if(wrapAtEnd) {
				lengthVal.do { |j|
					item = list.wrapAt(pos + j);
					inval = item.embedInStream(inval);
				}

			} {
				lengthVal.do { |j|
					item = list.at(pos + j);
					if(item.notNil) {
						inval = item.embedInStream(inval);
					} {
						^inval
					};
				}
			};
    		stepVal = stepStr.next(inval);
    		if(stepVal.isNil) { ^inval };
    		pos = pos + stepVal;
		};

	     ^inval;
    }
}

Pwalk : ListPattern {
		// random walk pattern - hjh - jamshark70@gmail.com

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


	embedInStream { arg inval;
		var	step;
		var index = startPos;
		var stepStream = stepPattern.asStream;
		var directionStream = directionPattern.asStream;
		// 1 = use steps as is; -1 = reverse direction
		var direction = directionStream.next(inval) ? 1;		// start with first value

		while({
			// get step, stop when nil
			(step = stepStream.next(inval)).notNil
		},{
			inval = list[index].embedInStream(inval);  // get value/stream out
			step = step * direction;	// apply direction
				// if next thing will be out of bounds
			if(((index + step) < 0) or: { (index + step) >= list.size }, {
				direction = directionStream.next(inval) ? 1;  // next direction, or 1
				step = step.abs * direction.sign;  // apply to this step
			});
			index = (index + step) % list.size;
		});

		^inval;
	}
}
