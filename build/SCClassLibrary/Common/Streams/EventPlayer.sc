EventPlayer {
	playEvent { arg event;
		event.use({ ~finish.value; });
		event.postln;
	}
}

NotePlayer : EventPlayer {
	playOneEvent {
		var msg, id, dur, server;
		
		id = UniqueID.next;
		server = ~server ? Server.default;
		
		// create note on event
		msg = ["/s_new", ~instrument, id, ~group];
		~argNames.do({ arg name;
			msg = msg.add(name);
			msg = msg.add(currentEnvironment.at(name));
		});
		// send note on event
		server.sendBundle(~latency, msg);
		
		// if tempo changes this will be inaccurate.
		// another way must be found to do this. but for now..
		dur = ~dur / ~tempo;
		
		// send note off
		server.sendBundle(~latency + dur, ["/n_set", id, "gate", 0]);
	}
	playEvent { arg event;
		var freqs;
		event.use({
			~finish.value; // finish the event
			freqs = ~freq;
			if (freqs.isKindOf(Symbol), { nil },{
				if (freqs.isSequenceableCollection, {
					freqs.do({ arg freq;
						~freq = freq;
						this.playOneEvent;
					});
						
				},{	
					this.playOneEvent;
				});
			});
		});
	}
}

/*

delta time is in beats.
convert delta time to absolute time.
note off in beats


delta time in beats.
latency in seconds.

tempo = beats/second
wake up time = delta time/tempo - next latency




one pattern

prototype events:
poly note w/ dur
poly note w/ separarate on and off
mono note
tempo change
control bus change

Pevent(event)  
merge events.
	a) make a copy of the larger event and put the fields of the smaller event in it.
	b)if properties are not a dictionary then can append properties.
		make a dictionary only at the end.
	c) parent chaining
	


Ppar -> merge parallel lists.
*/

