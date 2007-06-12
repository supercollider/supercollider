InBus {
	
	*ar { arg bus, numChannels=2, offset=0;
		^this.getOutput(bus.asBus, 'audio', numChannels, offset);
	}
	
	*kr { arg bus, numChannels=1, offset=0;
		^this.getOutput(bus.asBus, 'control', numChannels, offset);
	}
	
	*getOutput { arg bus, argRate, numChannels, offset=0;
		var rate, n, out, startIndex, index;
		rate = bus.rate;
		startIndex = bus.index + offset;
		n = bus.numChannels;
		if(n >= numChannels) {
			index = startIndex.min(n + bus.index);
		} {
			index = Array.fill(numChannels, { arg i; startIndex + (i % n) });
			numChannels = 1;
		};
		out = if(offset.isInteger) {
					if(rate === 'audio')
						{ InFeedback.ar(index, numChannels) }
						{ In.kr(index, numChannels) }
			} {
					if(rate === 'audio')
						{ XInFeedback.ar(index, numChannels) }
						{ XIn.kr(index, numChannels) }
			};
		
		// ar -> kr is not a problem so much (mainly with triggers). there is no A2K ugen
		^if((argRate === 'audio') and: { rate === 'control' }) { K2A.ar(out) } { out };
	
	}

}

// crossfde In by AdC and JRh

XIn { 
	*ar { arg which, n;
		^XFade2.ar( // use equal power crossfading for audio rate
			In.ar(which.round(2), n), 
			In.ar(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
	*kr { arg which, n;
		^LinXFade2.kr( // use linear crossfading for control rate
			In.kr(which.round(2), n), 
			In.kr(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
	
}

XInFeedback { 
	*ar { arg which, n;
		^XFade2.ar(
			InFeedback.ar(which.round(2), n), 
			InFeedback.ar(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
}

// listens on a fixed index (or several)
// plays out to another.

Monitor {
	
	var <group, <vol=1.0, <out=0;
	
	play { arg fromIndex, fromNumChannels=2, toIndex=0, toNumChannels, 
			target, multi=false, volume, fadeTime=0.02;
			
		var server, inGroup, numChannels, bundle, divider;
		inGroup = target.asGroup;
		server = inGroup.server;
		bundle = List.new;
		
		toNumChannels = toNumChannels ? fromNumChannels;
		numChannels = max(fromNumChannels, toNumChannels);
		vol = volume ? vol;

		if(group.isPlaying.not) { 
				group = Group.basicNew(inGroup.server);
				bundle.add(group.newMsg(inGroup, \addToTail));
				NodeWatcher.register(group);
				
		} {
				if(group.group.nodeID != inGroup.nodeID) {
					if(group.server !== group.group.server) 
						{ÊError("group not on the same server").throw }; 
						bundle.add(["/g_tail", inGroup.nodeID, group.nodeID]);
						group.group = inGroup;
				};
				if(multi.not) { bundle.add(["/n_set", group.nodeID, "gate", 0.0]) }
		};
		
		if(multi) {Êout = out.asCollection.add(toIndex) } {Êout = toIndex };
		divider = if(toNumChannels.even and: { fromNumChannels.even }, 2, 1);
		(numChannels div: divider).do { arg i;
			bundle.add([9, "system_link_audio_"++divider, 
					server.nextNodeID, 1, group.nodeID,
					\out, toIndex + (i * divider  % toNumChannels), 
					\in, fromIndex + (i * divider % fromNumChannels),
					\vol, vol
					]);
		};
		bundle.add(["/n_set", group.nodeID, "fadeTime", fadeTime]);
		server.listSendBundle(server.latency, bundle);
	}
	
	isPlaying {Ê^group.isPlaying }
	
	stop { arg fadeTime;
		if(group.isPlaying) {
			group.release(fadeTime);
			SystemClock.sched(fadeTime, { 
				group.free;
				group = nil;
				out = nil;
			}); 
		}
	}
	
	vol_ { arg invol;
		vol = invol ? vol;
		group.set(\vol, vol);
	}
	
	out_ { arg inval;
		out = inval;
		group.set(\out, inval);
	}


}
