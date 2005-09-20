Pstep : Pattern {
	var <>levelpattern, <>durpattern;

	*new { arg levelpattern, durpattern, curvepattern = \lin;
		^super.new
			.levelpattern_(levelpattern)
			.durpattern_(durpattern)
	}
	embedInStream { arg inval;
		var stream, val, dur;
		if (durpattern.notNil) {
			stream = Ptuple([levelpattern, durpattern]).asStream;
		} {
			stream = levelpattern.asStream
		};
		thisThread.endBeat = thisThread.endBeat ? thisThread.beats; 
		// endBeat > beats only if Pfindur ended something early
		thisThread.endBeat = thisThread.endBeat min: thisThread.beats;		
		
		loop {
			#val, dur = stream.next ?? {^inval};
			thisThread.endBeat = thisThread.endBeat + dur;
			while(
				{ thisThread.endBeat > thisThread.beats },
				{ inval = yield(val) }
			)
		};
	}	
}
Pseg : Pstep {
	var <>curvepattern;
	// this version uses Array-envAt to perform the interpolation between segment endpoints

	*new { arg levelpattern, durpattern, curvepattern = \lin;
		^super.new(levelpattern, durpattern)
			.curvepattern_(curvepattern)
	}
	embedInStream { arg inval;
		var stream, val, dur, curve;
		var evalArray,arrayOfEvalArrays;
		var startTime, curTime;
		// Array-envAt expects an Env in array form:
		//			start	lastseg	release	loop   end    dur   shape  curve
		evalArray = [	0,		1,		-99,		-99,	  1,     1,     0,     0];
		if (durpattern.notNil) {
			stream = Ptuple([levelpattern, durpattern, curvepattern]).asStream;
		} {
			stream = levelpattern.asStream
		};
		#val, dur, curve = stream.next ?? {^inval};
		thisThread.endBeat = thisThread.endBeat ? thisThread.beats; 
		thisThread.endBeat = thisThread.endBeat min: thisThread.beats;
		loop {
			evalArray[0] = val;
			evalArray[5] = dur;
			if (curve.isNumber) {
				evalArray[6] = 5;
				evalArray[7] = curve;
			} {
				evalArray[6] = Env.shapeNames[curve];
				evalArray[7] = 0
			};
			#val, dur, curve = stream.next ?? {^inval};
			evalArray[4] = val;
			
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
			};
		};
	}	
}

