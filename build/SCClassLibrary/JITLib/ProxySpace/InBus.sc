InBus {
	
	*ar { arg bus, numChannels=2, offset=0; //offset is your own responsibility
		^this.getOutput(bus.asBus, 'audio', numChannels, offset);
	}
	
	*kr { arg bus, numChannels=1, offset=0;
		^this.getOutput(bus.asBus, 'control', numChannels, offset);
	}
	
	*getOutput { arg bus, argRate, numChannels, offset=0;
		var rate, n, out, index, indices;
		rate = bus.rate;
		index = bus.index + offset;
		n = bus.numChannels;
		if(n >= numChannels, {
			out = if(rate === 'audio', {
				InFeedback.ar(index.min(n + bus.index), numChannels)
			}, {
				In.kr(index, numChannels)
			})
		}, {
			indices = Array.fill(numChannels, { arg i; index + (i % n) });
			out = if(rate === 'audio', {
				InFeedback.ar(indices, 1);
			}, {
				In.kr(indices, 1)
			})
		});
		//vice versa is not a problem so much (mainly with triggers). there is no A2K ugen
		^if((argRate === 'audio') and: { rate === 'control' }, {
			out = K2A.ar(out);
		}, { 
			out 
		});
	}

}

