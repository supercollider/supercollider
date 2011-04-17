/*
This file defines Group and Synth event types similar to the Group and Synth classes.
They follow the conventions of patterns, using integers (0,1,2,3) for addActions and
integer nodeID's as targets.

For example,

	g = (type: \Group, id: 2);
	g.play;
	a = (type: \Synth, group: 2)
	a.play;

Would start a group with nodeID = 2 and play a synth within that group.  If the event id is left unspecified, it is automatically allocated when the event is played.

Caution, Event-play returns a time value, so an expression of the form

	a = (type: \Group, id: 1).play

will assign the default duration of 1 to the variable a, not the group event!

The interface to these events is a hybrid of the Pattern and Node control interfaces consisting of
	play, stop, pause, resume
	release, set, map, before, after, headOf, tailOf

event[\id] can be an array. In this case, a set of nodes corresponding to each element of the array will be created and the order of the id's will be the order of the nodes running on the server.  (In other word, the nodes are created in reverse order when event[\addAction] is 0 or 2.

With the exception of ~server, ~latency, and ~instrument any key in the event can have an array as a value and the standard rules of multi-channel expansion will be followed.

These event types can be used in patterns, but are probably more useful for more static use
in system configuration or to control individual synths.

Here is a simple example of its use:
(
	g = (type: \Group, id: [2,3,4,5,6], group: 0, addAction: 1);  // define a multiple Group event
	g.play;										          // play it
	h = g.split;                                                 // split it into individual groups

	b = (type:\Synth, freq: [500,510], group: [2,3]);           // make a Synth event
	b.play;
	c = b.split;

	b.set(\freq,[1000,1200])

	c[0].set(\freq,700);
	c[1].set(\freq,400);

	h[0].release;
	h[1].release;

	g.stop;
)
*/

+Event {
	*checkIDs { | id, server |
		if (id.isNil) {^nil};
		if (id.asArray.first < (if(server.notNil) { server.options.initialNodeID } { 1000 })) { ^id};
		^nil
	}

	asEventStreamPlayer {}

	sendOSC { | msg |
		if (this[\isPlaying]) {
			this[\server].sendBundle(this[\latency],  *(msg.flop) )
		}
	}
	set { | ... args |
//		this.sendOSC([15, this[\id]] ++ (args.asOSCArgArray));
		args = ([15, this[\id]] ++ args).flop.asOSCArgBundle;
		if (this[\isPlaying]) {
			this[\server].sendBundle( this[\latency],  *args )
		}
	}

	stop { this.use { ~stopServerNode.value }  }
	pause { this.use { ~pauseServerNode.value }  }
	resume { this.use { ~resumeServerNode.value }  }
	release { |releaseTime| this.use { ~releaseServerNode.value(releaseTime) }  }
	free { this.use { ~freeServerNode.value }  }

	synth {
		this.parent = Event.parentEvents[\synthEvent];
	}
	group {
		this.parent = Event.parentEvents[\groupEvent];
	}

	split { | key = \id |
		var event;
		if (this[\isPlaying] == true) {
			^this[key].asArray.collect { |keyVal|
				event = this.copy.put(key, keyVal);
				NodeWatcher.register(event);
				event;
			}
		} {
			^this[key].asArray.collect { |keyVal|
				this.copy.put(key, keyVal)
			}
		}
	}
	nodeID { ^(this[\id].asArray[0]) }

	asGroup {
		var type = this[\type];
		if(type == \Group) { ^this };
		^this[\group].asGroup
	}
}
