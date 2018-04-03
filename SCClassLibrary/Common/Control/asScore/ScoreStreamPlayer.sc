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

	*new {
		^super.new("record").initScoreStreamPlayer
	}

	initScoreStreamPlayer {
		this.latency_(0);
		Server.all.remove(this); // we do not want to be part of the server list
	}

	beats2secs { | beats | ^beats }
	secs2beats { | beats | ^beats }

	add { | beats, args |
		beats = beats min: maxTime;
		if(beats.isArray) {
			beats.do { |each, i|
				// because of the way events multichannel expand their values
				// before calling ~schedBundle, the args are already flopped.
				// so in this case we just select one for each beat value
				bundleList = bundleList.add([each, args.wrapAt(i)])
			}
		} {
			bundleList = bundleList.add([beats] ++ args)
		}
	}

	prepareEvent { | event |
		event = event.copy;
		event.use({
			~schedBundle = { | lag, offset, server ... bundle |
				this.add(offset * tempo + lag + beats, bundle)
			};
			~schedBundleArray = { | lag, offset, server, bundle |
				this.add(offset * tempo + lag + beats, bundle)
			};
		});
		^event
	}

	makeScore { | stream, duration = 1, event, timeOffset = 0 |
		var ev, startTime, proto;
		proto = (
			server: this,

			schedBundle: { | lag, offset, server ... bundle |
				this.add(offset * tempo + lag + beats, bundle)
			},
			schedBundleArray: { | lag, offset, server, bundle |
				this.add(offset * tempo + lag + beats, bundle)
			}
		);

		event = event ?? { Event.default };
		event = event.copy.putAll(proto);
		beats = timeOffset;
		tempo = 1;
		bundleList = [];
		maxTime = timeOffset + duration;

		// call from a routine so that the stream has this as clock
		Routine {
			thisThread.clock = this;
			while {
				thisThread.beats = beats;
				ev = stream.next(event.copy);
				(maxTime >= beats) and: { ev.notNil }
			} {
				ev.putAll(proto);
				ev.play;
				beats = ev.delta * tempo + beats
			}
		}.next;

		bundleList = bundleList.sort { | a, b | b[0] >= a[0] };

		if((startTime = bundleList[0][0]) < 0) {
			timeOffset = timeOffset - startTime;
		};

//		bundleList.do { | b | b[0] = b[0] + timeOffset }

		^Score(bundleList.add([duration + timeOffset, [\c_set, 0, 0]]))
	}

}
