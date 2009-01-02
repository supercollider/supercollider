
Pstep : Pattern {
	var <>list, <>durs,  <>repeats;

	*new { arg levels, durs = 1, repeats = 1;
		^super.newCopyArgs(levels, durs, repeats).init
	}
	init { if (list.isKindOf(Collection)) { list = Pseq(list); } }
	
	embedInStream { arg inval;
		var stream, val,dur;
		thisThread.endBeat = thisThread.endBeat ? thisThread.beats; 
		// endBeat > beats only if Pfindur ended something early
		thisThread.endBeat = thisThread.endBeat min: thisThread.beats;		
		
		repeats.do { | i |
			stream = Ptuple([list, durs]).asStream;
			while (			
				{ #val, dur = stream.next(inval) ? [nil, nil];
					val.notNil;
				}, 
				{
				thisThread.endBeat = thisThread.endBeat + dur;
					while(
						{ thisThread.endBeat > thisThread.beats },
						{ inval = val.embedInStream(inval) }
					)
				});
		};
		^inval;
	}
	storeArgs {
		^[list, durs, repeats]
	}	
}

Pseg : Pstep {
	var <>curves;
	// this version uses Array-envAt to perform the interpolation between segment endpoints

	*new { arg levels, durs = 1, curves = \lin,  repeats = 1 ;
		^super.new(levels, durs, repeats)
			.curves_(curves)
	}
	embedInStream { arg inval;
		var stream, streamVals, val, dur, curve;
		var evalArray,arrayOfEvalArrays;
		var startTime, curTime;
		// Array-envAt expects an Env in array form:
		//			start	lastseg	release	loop   end    dur   shape  curve
		evalArray = [0,		1,		-99,		-99,	  1,     1,     0,     0];
		repeats.do {
			if (durs.notNil) {
				stream = Ptuple([list, durs, curves]).asStream;
			} {
				stream = list.asStream
			};
			stream = Ptuple([list, durs, curves]).asStream;
			#val, dur, curve = stream.next(inval) ?? {^inval};
			thisThread.endBeat = thisThread.endBeat ? thisThread.beats min: thisThread.beats;
			while ({
				evalArray[0] = val;
				evalArray[5] = dur;
				if (curve.isNumber) {
					evalArray[6] = 5;
					evalArray[7] = curve;
				} {
					evalArray[6] = Env.shapeNames[curve];
					evalArray[7] = 0
				};
				(streamVals = stream.next(inval)).notNil
			}, {
				#val, dur, curve = streamVals;
				evalArray[4] = val;
				evalArray[7] = val;
				
				startTime = thisThread.endBeat;
				thisThread.endBeat = thisThread.endBeat + evalArray[5];
				if (val.isArray) {
					arrayOfEvalArrays = evalArray.flop;
					while(
						{ thisThread.endBeat > curTime = thisThread.beats },
						{ inval = yield(arrayOfEvalArrays.collect({ | a | 
								a.envAt(curTime - startTime)}) 
							) 
						})
				} {
					while(
						{ thisThread.endBeat > curTime = thisThread.beats },
						{ inval = yield(evalArray.envAt(curTime - startTime) ) }
					)
				}
			})
		}
	}
	storeArgs {
		^[list, durs, curves, repeats]
	}	
}
