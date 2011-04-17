SoundIn  {

	*ar { arg bus = 0, mul=1.0, add=0.0;
		var chanOffset;
		chanOffset = this.channelOffset;
		if(bus.isArray.not,{
			^In.ar(chanOffset + bus, 1).madd(mul,add)
		});

		// check to see if channels array is consecutive [n,n+1,n+2...]
		if(bus.every({arg item, i;
				(i==0) or: {item == (bus.at(i-1)+1)}
			}),{
			^In.ar(chanOffset + bus.first, bus.size).madd(mul,add)
		},{
			// allow In to multi channel expand
			^In.ar(chanOffset + bus).madd(mul,add)
		})
	}

	*channelOffset {
		^NumOutputBuses.ir
	}
}

// backward compatible version. Only difference: starts counting from channel 1

AudioIn : SoundIn  {
	*ar { arg channel = 0, mul=1.0, add=0.0;
		^super.ar(channel, mul, add)
	}
	*channelOffset {
		^NumOutputBuses.ir - 1
	}
}
