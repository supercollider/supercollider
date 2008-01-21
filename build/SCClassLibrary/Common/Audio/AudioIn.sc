
AudioIn  {

	*ar { arg channel = 1, mul=1.0, add=0.0;
		var chanOffset;
		chanOffset = NumOutputBuses.ir - 1;
		if(channel.isArray.not,{
			^In.ar(chanOffset + channel, 1).madd(mul,add)
		});
		
		// check to see if channels array is consecutive [n,n+1,n+2...]
		if(channel.every({arg item, i; 
				(i==0) or: {item == (channel.at(i-1)+1)}
			}),{
			^In.ar(chanOffset + channel.first, channel.size).madd(mul,add)
		},{
			// allow In to multi channel expand
			^In.ar(chanOffset + channel).madd(mul,add)
		})
	}
}

SoundIn  {

	*ar { arg bus = 0, mul=1.0, add=0.0;
		var chanOffset;
		chanOffset = NumOutputBuses.ir;
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
}