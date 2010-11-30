//RenderNotePlayer : NotePlayer {
//	var <>maxTime, <>score, <time=0;
//
//	*new { arg maxTime = 60;
//		^super.newCopyArgs(maxTime, Score.new)
//	}
//
//	playOneEvent { arg desc, lag, msgArgs;
//		var server, id, bundle, noteOffTime;
//
//		server = ~server;
//		id = server.nextNodeID;
//		bundle = [time + lag, [9, desc.name, id, ~addAction, ~group] ++ msgArgs];
//		score = score.add(bundle);
//		if (desc.hasGate) {
//			noteOffTime = time + lag  + ~sustain;
//			// noteOffTime = min(maxTime, noteOffTime);
//			bundle = [noteOffTime, [15, id, \gate, 0]]; //15 == n_set
//			score.add(bundle);
//		};
//		time = time + ~dur;
//	}
//
//	read { arg stream;
//		var val;
//		while {
//			val = stream.next(Event.default);
//			val.notNil and: { time < maxTime };
//		} {
//			this.playEvent(val);
//		};
//
//	}
//
//}
