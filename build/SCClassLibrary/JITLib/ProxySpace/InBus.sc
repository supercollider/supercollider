InBus {
	
	*ar { arg bus, numChannels=2;
		^this.getOutput(bus.asBus, 'audio', numChannels);
	}
	
	*kr { arg bus, numChannels=1;
		^this.getOutput(bus.asBus, 'control', numChannels);
	}
	
	*getOutput { arg bus, argRate, numChannels;
		var rate, n, out, index, indices;
		rate = bus.rate;
		index = bus.index;
		n = bus.numChannels;
		if(n >= numChannels, {
			out = if(rate === 'audio', {
				InFeedback.ar(index, numChannels)
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