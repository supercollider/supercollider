ScoreStreamPlayer : EventStreamPlayer {
	var absTime=0, <>score, <>server, <>maxTime;
	
	*new { arg stream, event, server;
		^super.new(stream).event_(event ? Event.default).score_(Score.new).server_(server ? Server.default);
	}

	read { arg duration=1.0, doReset = false, timeOffset = 0.0;
		var i =0, maxTime=0;
		if (stream.notNil) { "already reading".postln; ^this };
		if (doReset) { this.reset };
		maxTime = duration + timeOffset;
		absTime = timeOffset;
		stream = originalStream;
		
		event.put(\absTime, absTime);
		event.put(\score, score);
		event.put(\server, server);
		event.put(\type, (event.type ++ \_score).asSymbol);
//		this.debug(event[\type]);
		
		while({absTime<= maxTime and: stream.notNil}, {this.next(i);i=i+1;});
		score = score.add([absTime min: maxTime, [\c_set,0,0]]);
		stream = nil;
		^score;
	}
	
	
	next { arg inTime;
		var outEvent, nextTime;
		event.put(\absTime, absTime);
		outEvent = stream.next(event);
		if (outEvent.isNil) {
			stream = nextBeat = nil;
			^nil
		}{
			if (muteCount > 0) { outEvent.put(\freq, \rest) };
			
			if ((nextTime = outEvent.play).isNil) { stream = nil };
			absTime = absTime + nextTime;
			nextBeat = inTime + nextTime;	// inval is current logical beat
			
			^nextTime
		};
	}
}

