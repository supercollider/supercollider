
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
	*alloc { arg rate=\audio,server,numChannels=1;
		^this.perform(rate,server,numChannels);
	}

	*new { arg rate=\audio,index=0,numChannels=2,server;
		^super.newCopyArgs(rate,index,numChannels,server ?? {Server.local})
	}

	setAll { arg value;
		server.sendBundle(server.latency,
			["/c_fill",index,this.numChannels,value]);
	}
	set { arg ... values; // shouldn't be larger than this.numChannels
		server.sendBundle(server.latency,["/c_set"] 
			++ values.collect({ arg v,i; [index + i ,v] }).flat);
	}
	setList { arg values; // shouldn't be larger than this.numChannels
		server.sendBundle(server.latency,["/c_set"] 
			++ values.collect({ arg v,i; [index + i ,v] }).flat);
	}
	
	value_ { arg value;
		this.setAll(value);
	}
	
	setn { arg values;
		// could throw an error if values.size > numChannels
		server.sendBundle(server.latency,
			["/c_setn",index,values.size,values]);
	}
/*
	// not yet implemented on the server:
	// get, getn
*/

	free {
		// it should be an error to try and free a non-private bus ?
		if(rate == \audio,{
			server.audioBusAllocator.free(index);
		},{
			server.controlBusAllocator.free(index);
		});
		index = nil;
		numChannels = nil;
	}

}

