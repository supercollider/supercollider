
// decrepated.
/*
PolyPlayer : EventPlayer {

	
	
	playEvent { arg event;
		
		event.use({
			var bundles, latency, noteOffTime, mapBundles, strum, mapArgs, server, canRelease;
			
			~finish.value; // finish the event
			// info = SynthDefInfo.at(~instrument);
			// ~argNames = info.argNames ? #[\out, \freq, \amp, \pan, \sustain];
			~argNames = ~argNames ? #[\out, \freq, \amp, \pan, \sustain];

			// canRelease = info.canReleaseSynth;
			canRelease = ~canRelease ? true;
			
			bundles = this.synthMsg;
			
			mapArgs = ~mapArgs;
			server = ~server;
			latency = server.latency;
			noteOffTime = (latency + ~lag + ~sustain).asArray;
			latency = (latency + ~lag).asArray;
			strum = ~strum;
			if(mapArgs.isNil, {
				bundles.do({ arg item, i;
					var offset;
					offset = i * strum;
					server.sendBundle(offset + latency.wrapAt(i), item);
					if(canRelease, {
						server.sendBundle(offset + noteOffTime.wrapAt(i), [15, item[2], \gate, 0])
					})
				})
			}, {
				bundles.do({ arg item, i;
					var offset, id, dt;
					id = item[2].debug;
					offset = i * strum;
					dt = offset + latency.wrapAt(i);
					server.sendBundle(dt, item);
					server.sendBundle(dt, [14, id] ++ mapArgs); //n_map
					if(canRelease, {
						server.sendBundle(offset + noteOffTime.wrapAt(i), [15, id, \gate, 0])
					});
				})
			});
		})
	}
	
	synthMsg {
		var id, messages, instr;
		messages = ~argNames.makeEnvirValPairs; // use synthDec scheme later
		//messages = ~msgFunc.value.at(0); // first is synth bundle
		messages = messages.multiChannelExpand;
		instr = ~instrument.asArray;
		^messages.collect({ arg item, i;
			["/s_new", instr.wrapAt(i), ~server.nextNodeID, 0, ~group ? 0] ++ item 
		})
	}

}
*/

MapNotePlayer : NotePlayer {
	playOneEvent { arg desc, lag, msgArgs;
		var server, id, mapping;
		server = ~server;
		id = server.nextNodeID;
		mapping = ~mapping;
		if(mapping.notNil) { mapping = [14, id] ++ mapping };
		//send the note on bundle
		server.sendBundle(lag, [9, desc.name, id, ~addAction ? 1, ~group] ++ msgArgs, mapping); 
		
		if (desc.hasGate) {
			// send note off bundle.
			~tempoclock.sched(~sustain, { 
			server.sendBundle(lag, [15, id, \gate, 0]); //15 == n_set
			});
		};
	}


}

PolyPlayer : MapNotePlayer {}

