PolyPlayer : EventPlayer {

	playEvent { arg event;
		
		event.use({
			var bundles, latency, noteOffTime, mapBundles, strum;
			~finish.value; // finish the event
			bundles = MultiChannelEventHatch.newSynthMsg;
			noteOffTime = (~server.latency + ~lag + ~sustain).asArray;
			latency = (~server.latency + ~lag).asArray;
			strum = ~strum;
			bundles.do({ arg item, i;
				var offset;
				offset = i * strum;
				~server.sendBundle(offset + latency.wrapAt(i), item);
				~server.sendBundle(offset + noteOffTime.wrapAt(i), [15, item[2], \gate, 0])
			});
		})
	}

}

