
Bus {

	var <rate,<index,<numChannels,<server;
	
	*control { arg server,numChannels=1;
		var alloc;
		alloc = server.controlBusAllocator.alloc(numChannels);
		if(alloc.isNil,{
			error("BusAllocation: failed to get a control bus allocated."
				+ "numChannels:" + numChannels + "server:" + server);
			^nil
		});
		^this.new(\control,alloc,numChannels,server)
	}
	*audio { arg server,numChannels=1;
		var alloc;
		alloc = server.audioBusAllocator.alloc(numChannels);
		if(alloc.isNil,{
			error("BusAllocation: failed to get an audio bus allocated."
			+ "numChannels:" + numChannels + "server:" + server);
			^nil
		});
		^this.new(\audio,alloc,numChannels,server)
	}
	*alloc { arg rate,server,numChannels=1;
		^this.perform(rate ? \audio,server,numChannels);
	}

	*new { arg rate=\audio,index=0,numChannels=2,server;
		^super.newCopyArgs(rate,index,numChannels,server ?? {Server.local})
	}

	set { arg ... values; // shouldn't be larger than this.numChannels
		server.sendBundle(nil,(["/c_set"] 
			++ values.collect({ arg v,i; [index + i ,v] }).flat));
	}
	setMsg { arg ... values;
		^["/c_set"] 
			++ values.collect({ arg v,i; [index + i ,v] }).flat
	}
	
	setn { arg values;
		// could throw an error if values.size > numChannels
		server.sendBundle(nil,
			["/c_setn",index,values.size] ++ values);
	}
	fill { arg value,numChans;
		// could throw an error if numChans > numChannels
		server.sendBundle(nil,
			["/c_fill",index,numChans,value]);
	}
	// not yet implemented :
	// get, getn

	free {
		if(index.isNil,{ (this.asString + " has already been freed").warn; ^this });
		if(rate == \audio,{
			server.audioBusAllocator.free(index);
		},{
			server.controlBusAllocator.free(index);
		});
		index = nil;
		numChannels = nil;
	}

	// alternate syntaxes
	setAll { arg value;
		this.fill(value,numChannels);
	}
	value_ { arg value;
		this.fill(value,numChannels);
	}
	printOn { arg stream; stream << this.class.name << "(" <<* [server.name,rate,index,numChannels]  <<")" }
//	== { arg aBus;
//		^(aBus.class === this.class 
//		and: {aBus.index == index} and: {aBus.rate == rate} and: {aBus.server == server})
//	}
	isAudioOut { // audio interface
		^(rate === \audio and: {index <= server.options.firstPrivateBus})
	}
}

