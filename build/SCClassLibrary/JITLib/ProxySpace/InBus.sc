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
		out = if(offset.isKindOf(UGen)) {
					if(rate === 'audio')
						{ XInFeedback.ar(index, numChannels) }
						{ XIn.kr(index, numChannels) }
			} {
					if(rate === 'audio')
						{ InFeedback.ar(index, numChannels) }
						{ In.kr(index, numChannels) }
			};
		
		// ar -> kr is not a problem so much (mainly with triggers). there is no A2K ugen
		^if((argRate === 'audio') and: { rate === 'control' }) { K2A.ar(out) } { out };
	
	}

}

// crossfde In by AdC and JRh

XIn { 
	*ar { arg which, n;
		^XFade2.ar(
			In.ar(which.round(2), n), 
			In.ar(which.trunc(2) + 1, n), 
			(which * 2 - 1).fold2(1)
		);
	}
	*kr { arg which, n;
		^XFade2.kr(
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

