PolyPlayer : EventPlayer {

	playEvent { arg event;
		
		event.use({
			var bundles, latency, noteOffTime, mapBundles, strum, mapArgs, server;
			~finish.value; // finish the event
			bundles = MultiChannelEventHatch.newSynthMsg;
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
					server.sendBundle(offset + noteOffTime.wrapAt(i), [15, item[2], \gate, 0])
				})
			}, {
				bundles.do({ arg item, i;
					var offset, id, dt;
					id = item[2].debug;
					offset = i * strum;
					dt = offset + latency.wrapAt(i);
					server.sendBundle(dt, item);
					server.sendBundle(dt, [14, id] ++ mapArgs); //n_map
					server.sendBundle(offset + noteOffTime.wrapAt(i), [15, id, \gate, 0])
				})
			});
		})
	}

}

