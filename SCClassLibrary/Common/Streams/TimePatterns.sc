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

		repeats.value(inval).do { | i |
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

	*new { arg levels, durs = 1, curves = \lin,  repeats = 1 ;
		^super.new(levels, durs, repeats).curves_(curves)
	}
	embedInStream { arg inval;
		var valStream, durStream, curveStream, startVal, val, dur, curve;
		var env;
		var startTime, curTime;
		repeats.value(inval).do {
			valStream = list.asStream;
			durStream = durs.asStream;
			curveStream = curves.asStream;
			val = valStream.next(inval) ?? {^inval};
			thisThread.endBeat = thisThread.endBeat ? thisThread.beats min: thisThread.beats;
			while {
				startVal = val;
				val = valStream.next(inval);
				dur = durStream.next(inval);
				curve = curveStream.next(inval);

				val.notNil and: { dur.notNil and: { curve.notNil } }
			} {
				startTime = thisThread.endBeat;
				thisThread.endBeat = thisThread.endBeat + dur;
				if (startVal.isArray) {
					env = [startVal,val, dur, curve].flop.collect { | args |
						Env([args[0], args[1]], [args[2]], args[3]) };
					while { thisThread.endBeat > curTime = thisThread.beats } {
						inval = yield(env.collect{ | e | e.at(curTime - startTime)})
					}
				} {
					env = Env([startVal, val], [dur], curve);
					while { thisThread.endBeat > curTime = thisThread.beats } {
						inval = yield(env.at(curTime - startTime))
					}
				}
			}
		}
	}
	storeArgs {
		^[list, durs, curves, repeats]
	}
}
