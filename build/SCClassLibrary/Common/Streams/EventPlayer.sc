EventPlayer {
	playEvent { arg event;
		event.use({ ~finish.value; });
		event.postln;
	}
}

NotePlayer : EventPlayer {
	playOneEvent { arg lag, dur, freq;
		var msg, server, id;
		
		server = ~server;
		id = server.nextNodeID;
		
		msg = ~msgFunc.value(id, freq);
		
		//send the note on bundle
		server.sendBundle(lag, msg); 
				
		// send note off bundle. 
		server.sendBundle(lag + dur, [15, id, \gate, 0]); //15 == n_set
	}
	playEvent { arg event;
		var freqs, lag, dur, strum;
		event.use({
			~finish.value; // finish the event
			// if tempo changes this will be inaccurate.
			// another way must be found to do this. but for now..
			dur = ~dur / ~tempo;
			lag = ~lag + ~server.latency;
			freqs = ~freq;
			strum = ~strum;
			if (freqs.isKindOf(Symbol), { nil },{
				if (freqs.isSequenceableCollection, {
					freqs.do({ arg freq, i;
						this.playOneEvent(i * strum + lag, dur, freq);
					});
						
				},{	
					this.playOneEvent(lag, dur, freqs);
				});
			});
		});
	}
}

NoteDurPlayer : NotePlayer {
	playOneEvent { arg lag, dur, freq;
		var msg, server, id;
		
		server = ~server;
		id = server.nextNodeID;
		
		msg = ~msgFunc.value(id, freq);
		
		//send the note on bundle
		server.sendBundle(lag, msg); 
		
		// no note off. note assumed to have a fixed duration.
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

