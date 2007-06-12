
BroadcastServer : Server {

	var <>localServer, <>allAddr;
	
	
	*newFrom { arg localServer, allAddr;
		^super.new
				.localServer_(localServer)
				.ninit(localServer.name.asString ++ "_broadcast")
				.allAddr_(allAddr.add(localServer.addr))
				.newNodeWatcher
				
				
				 
	}
	
	////we don't need much here, this class mainly wraps the messages
	ninit { arg argName;
		name = argName; 
		addr = localServer.addr;
		options = localServer.options;
		named.put(name, this);
	}
	init {}
	
	newNodeWatcher {
		nodeWatcher = BasicNodeWatcher.new(allAddr);
		//nodeWatcher.start;
		
	}
	boot {
		nodeWatcher.start;
		
	}
	quit {
		nodeWatcher.stop;
	}
	serverRunning { ^true }
	
	
	///message forwarding
		
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
		notified = true;
		allAddr.do({ arg addr; addr.sendMsg("/notify", flag.binaryValue) });
	}
	
	nodeIsPlaying_ { arg nodeID;
		nodeWatcher.nodes.add(nodeID);
	}
	
	
		
	//use local allocators

	nodeAllocator { ^localServer.nodeAllocator }
	staticNodeAllocator { ^localServer.staticNodeAllocator }
	controlBusAllocator { ^localServer.controlBusAllocator }
	audioBusAllocator { ^localServer.audioBusAllocator }
	bufferAllocator { ^localServer.bufferAllocator }
	
	nextNodeID { ^localServer.nodeAllocator.alloc }
	nextStaticNodeID { ^localServer.staticNodeAllocator.alloc }
	nextSharedNodeID { ^localServer.nextSharedNodeID }
}


Router : Server {
	
	var <broadcast, <sharedNodeIDAllocator;
	var <clientNumber=0; //for multi user dungeons
	
	*new { arg name, addr, options, clientNumber=0, allAddr;
		^super.new(name, addr, options)
			.setID(clientNumber)
			.newAllocatorsForThisClient //move up later maybe
			.initBroadcast(allAddr)
	}
	nextSharedNodeID {
		^sharedNodeIDAllocator.alloc
	}
	boot {
		super.boot;
		broadcast.boot;
	}
	quit {
		super.quit;
		broadcast.quit;
	}
	
	//isLocal { ^false }
	
	initBroadcast { arg addresses;
		broadcast = BroadcastServer.newFrom(this, addresses);
	}
	
	
	setID { arg id;
		clientNumber = id;
	}
	
	newAllocators {}
	
	newAllocatorsForThisClient { 
		var startID;
		startID = clientNumber * (options.maxNodes * 2) + options.maxNodes  + 1000;
		//this one is same for all
		sharedNodeIDAllocator = RingNumberAllocator(1000, 1000 + options.maxNodes);
		
		nodeAllocator = RingNumberAllocator(startID  + 1 + options.maxNodes, 
										startID + (2*options.maxNodes));
		staticNodeAllocator = RingNumberAllocator(startID + 1 + (2*options.maxNodes), 
										startID +  (3*options.maxNodes));
		controlBusAllocator = PowerOfTwoAllocator(options.numControlBusChannels);
		audioBusAllocator = PowerOfTwoAllocator(options.numAudioBusChannels, 
		options.numInputBusChannels + options.numOutputBusChannels);
		bufferAllocator = PowerOfTwoAllocator(options.numBuffers);
	}
	
	nodeIsPlaying_ { arg nodeID;
		nodeWatcher.nodes.add(nodeID);
	}
	

}

