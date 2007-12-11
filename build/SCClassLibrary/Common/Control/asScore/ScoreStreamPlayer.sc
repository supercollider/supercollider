
/* 
 ScoreStreamPlayer is a faux server that collects OSC commands from an EventStream into a Score.
 It implements the functionality of a TempoClock, so tempo changes and time based patterns within
 the event stream can be supported. 
 
 Patterns that play on multiple servers are not directly supported, but can be managed.
 For example, assuming the server is explicitly identified in the pattern:
 
 arrayOfScores = arrayOfServers.collect { | server |
 		Pchain(
 			Pbind(\freq, 
 				Pfunc({| ev |
 					if (ev[\server] == server) 
 						{ ev[\freq] }
 						{ \rest}
 				})
 			),
 			pattern
 		).asScore
 }

*/
ScoreStreamPlayer : Server {
	var <>beats, <>tempo;
	var <>bundleList, <>maxTime;
	var <>score;
	
	// bundling support added
	*new { ^super.new("record").latency_(0) }
	
	add { | beats, args|
		bundleList = bundleList.add([beats min: maxTime] ++ args)
	}
	sendBundle { arg time ... msgs;
		this.add(time * tempo + beats, msgs)
	}
	
 	listSendBundle { arg time, msgs;
		bundleList = this.add(time * tempo + beats, msgs)
	}


	sendRaw { arg rawArray;
		bundleList =this.add(beats, rawArray)
	}
	
	sendMsg { arg ... msg;
		this.add(beats, msg)
	}

	listSendMsg { arg msg;
		bundleList = this.add(beats, msg)
	}
	
	sendMsgSync { arg condition ... msgs;
		bundleList = this.add(beats, msgs)
	}
	
	sync { arg condition, msgs, latency; // array of bundles that cause async action
		bundleList = this.add(beats, msgs)
	}
	
	secs2beats { | seconds  |
		^(beats/tempo)
	}
	
	sched { | time, function |
		var oldbeats = beats;
		beats = time * tempo + beats;
		function.value;
		beats = oldbeats;
	}
	

	makeScore { | stream, duration = 1, event, timeOffset = 0|
		var ev;
		event = event ? Event.default;
		beats = timeOffset;
		tempo = 1;
		bundleList = [];
		maxTime = timeOffset + duration;
		Routine {
			thisThread.clock = this;
			while ({
				ev = stream.next(event.copy);		
				(maxTime >= beats) && ev.notNil
			},{ 
				ev[\server] = this;
				ev.play;
				beats = ev.delta * tempo + beats
			})
		}.next;
		^score = Score(bundleList.add([duration, [\c_set, 0, 0]]) );
	}
	
}
