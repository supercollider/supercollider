EventPlayer {
	playEvent { arg event;
		event.use({ ~finish.value; });
		event.postln;
	}
}

NotePlayer : EventPlayer {
	playOneEvent { arg desc, lag, msgArgs;
		var server, id;
//		var ttbeats, ttseconds;
		
		server = ~server;
		id = server.nextNodeID;
		
		//send the note on bundle
		server.sendBundle(lag, [9, desc.name, id, 0, ~group] ++ msgArgs); 
//		ttbeats = thisThread.beats;
//		ttseconds = thisThread.seconds;
				
		if (desc.hasGate) {
			// send note off bundle.
			~tempoclock.sched(~sustain, { 
	//			if (thisThread.seconds <= ttseconds) {
	//				[\ooo, ttbeats, ttseconds, thisThread.beats, thisThread.seconds, dur].postln;
	//				TempoClock.default.prDump;
	//			};
				server.sendBundle(lag, [15, id, \gate, 0]); //15 == n_set
			});
		};
	}
	playEvent { arg event;
		var freqs, lag, dur, strum, sustain, desc, bndl;
		event.use({
			~finish.value; // finish the event
			lag = ~lag + ~server.latency;
			freqs = ~freq;
			strum = ~strum;
			sustain = ~sustain;

			if (~tempo.notNil) { ~tempoclock.tempo = ~tempo };
	
			desc = ~synthLib.synthDescs[~instrument];
			if (desc.isNil) { error("instrument " ++ ~instrument ++ " not found."); ^nil };
						
			if (freqs.isKindOf(Symbol), { nil },{

				bndl = desc.msgFunc.value.flop;
				bndl.do {|msgArgs, i|
					this.playOneEvent(desc, i * strum + lag, msgArgs);
				};
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

