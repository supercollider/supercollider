
BroadcastServer : Server {

	var <>localServer, <>allAddr;
	
	*newFrom { arg localServer, allAddr;
		^super.new(localServer.name.asString ++ "_broadcast")
				.localServer_(localServer)
				.allAddr_(allAddr.add(localServer.addr))
	}
		
	sendMsg { arg ... args;
		allAddr.do({ arg addr; addr.sendBundle(nil, args) });
	}
	sendBundle { arg time ... messages;
		allAddr.do({ arg addr; addr.performList(\sendBundle, time, messages); });
	}
	sendRaw { arg rawArray;
		allAddr.do({ arg addr; addr.sendRaw(rawArray) })
	}
	
	listSendMsg { arg msg;
		allAddr.do({ arg addr; addr.sendBundle(nil,msg) });
	}
 	listSendBundle { arg time,bundle;
 		allAddr.do({ arg addr; addr.performList(\sendBundle, [time ? this.latency] ++ bundle) })
	}
		
	status {
		allAddr.do({ arg addr; addr.sendMsg("/status") });
	}
	notify { arg flag=true;
		allAddr.do({ arg addr; addr.sendMsg("/notify", flag.binaryValue) });
	}
	
	serverRunning {
		^true //for now assume it is
	}
	
	//use local allocators

	nodeAllocator { ^localServer.nodeAllocator }
	staticNodeAllocator { ^localServer.staticNodeAllocator }
	controlBusAllocator { ^localServer.controlBusAllocator }
	audioBusAllocator { ^localServer.audioBusAllocator }
	bufferAllocator { ^localServer.bufferAllocator }
	
	nextNodeID { ^localServer.nodeAllocator.alloc }
	nextStaticNodeID { ^localServer.staticNodeAllocator.alloc }
}


Router : Server {
	
	var <broadcast;
	var <clientNumber=0; //for multi user dungeons
	
	*new { arg name, addr, options, clientNumber=0, allAddr;
		^super.new(name, addr, options)
			.setID(clientNumber)
			.newAllocatorsForThisClient //move up later maybe
			.initBroadcast(allAddr)
	}
	
	
	initBroadcast { arg addresses;
		broadcast = BroadcastServer.newFrom(this, addresses);
	}
	
	setID { arg id;
		clientNumber = id;
	}
	
	newAllocators {}
	
	newAllocatorsForThisClient { 
		var startID;
		startID = clientNumber * (options.maxNodes * 2) + 1;
		nodeAllocator = RingNumberAllocator(startID, startID + options.maxNodes);
		staticNodeAllocator = RingNumberAllocator(startID + options.maxNodes, 
										startID + 1 + (2*options.maxNodes));
		
		controlBusAllocator = PowerOfTwoAllocator(options.numControlBusChannels);
		audioBusAllocator = PowerOfTwoAllocator(options.numAudioBusChannels, 
		options.numInputBusChannels + options.numOutputBusChannels);
		bufferAllocator = PowerOfTwoAllocator(options.numBuffers);
	}


}

