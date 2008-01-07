
/* 
 ScoreStreamPlayer collects OSC commands from an EventStream into a Score. It is derived from server to provide allocation of nodeIDs, bufums, etc.

  It implements the functionality of a TempoClock, to support tempo changes and time based patterns. 
 
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
	
	*new { ^super.new("record").latency_(0) }
	
	beats2secs { | beats | ^beats }
	secs2beats { | beats | ^beats }
	
	add { | beats, args|
		bundleList = bundleList.add([beats min: maxTime] ++ args)
	}
	
	prepareEvent { | event |
		event = event.copy;
		event.use({
			~schedBundle = { | time, server ...bundle | 
				this.add(time * tempo + beats, bundle)
			};
			~schedBundleArray = { | time, server, bundle | 
				this.add(time * tempo + beats, bundle)
			};
		});
		^event;
	}
			
	makeScore { | stream, duration = 1, event, timeOffset = 0|
		var ev, startTime;
		event = this.prepareEvent(event ? Event.default);
			
		beats = timeOffset;
		tempo = 1;
		bundleList = [];
		maxTime = timeOffset + duration;
		Routine {
			thisThread.clock = this;
			while ({
				thisThread.beats = beats;
				ev = stream.next(event.copy);		
				(maxTime >= beats) && ev.notNil
			},{ 
				ev[\server] = this;
				ev.play;
				beats = ev.delta * tempo + beats
			})
		}.next;
		bundleList = bundleList.sort({ | a, b | b[0] >= a[0] });
		bundleList[0][0].postln;
		if ((startTime = bundleList[0][0]) < 0 ) {
			timeOffset = timeOffset - startTime;
		};
		bundleList.do { | b | b[0] = b[0] + timeOffset }
		
		^Score(bundleList.add([duration, [\c_set, 0, 0]]) );
	}
	
}
