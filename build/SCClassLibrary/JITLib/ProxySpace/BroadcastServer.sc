
BroadcastServer : Server {

	var <>localServer, <>allAddr;
	
	
	*newFrom { arg localServer, allAddr;
		^super.new
				.localServer_(localServer)
				.ninit(localServer.name.asString ++ "_broadcast")
				.allAddr_(allAddr)
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
	
	addr { ^allAddr } //support NodeWatcher.register
	
	makeWindow {} //doesn't make sense, use Router for gui
	
	newNodeWatcher {
		nodeWatcher = NodeWatcher.new(allAddr);
		nodeWatcher.start;
		
	}
	boot {
		nodeWatcher.start;
		this.notify;
		
	}
	quit {
		nodeWatcher.stop;
	}
	
	serverRunning { ^true } //assume that.
	
	at { arg index;
		^allAddr.clipAt(index)
	}
	
	wrapAt { arg index;
		^allAddr.wrapAt(index)
	}
	
	do { arg function;
		^allAddr.do(function)
	}
	
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
	
	
	
		
	//use local allocators

	nodeAllocator { ^localServer.nodeAllocator }
	staticNodeAllocator { ^localServer.staticNodeAllocator }
	controlBusAllocator { ^localServer.controlBusAllocator }
	audioBusAllocator { ^localServer.audioBusAllocator }
	bufferAllocator { ^localServer.bufferAllocator }
	
	nextNodeID { ^localServer.nodeAllocator.alloc }
	//nextStaticNodeID { ^localServer.staticNodeAllocator.alloc }
	nextSharedNodeID { ^localServer.nextSharedNodeID }
}


DispatchServer : BroadcastServer {
	var <>latencies;
	
	*newFrom { arg localServer, allAddr, latencies;
		^super.newFrom(localServer, allAddr).latencies_(latencies ? #[])
	}
	
	sendMsg { arg ... args;
		allAddr.do({ arg addr, i; 
				if(latencies.clipAt(i).notNil, {
					addr.sendBundle(nil, args) 
				});
		})
	}
	
	sendBundle { arg time ... messages;
		allAddr.do({ arg addr, i; 
				var time; 
				time = latencies.clipAt(i);
				if(time.notNil, {
					addr.performList(\sendBundle, time, messages);
				});
		})
	}
	sendRaw { arg rawArray;
		allAddr.do({ arg addr, i; 
				if(latencies.clipAt(i).notNil, {
					 addr.sendRaw(rawArray) 
				});
		})
	}
	
	listSendMsg { arg msg;
		allAddr.do({ arg addr, i; 
				if(latencies.clipAt(i).notNil, {
					 addr.sendBundle(nil,msg)
				});
		})
	}
	
 	listSendBundle { arg time,bundle;
 		allAddr.do({ arg addr, i; 
				var time; 
				time = latencies.clipAt(i);
				if(time.notNil, {
					addr.performList(\sendBundle, [time] ++ bundle)
				});
		})
 		
	}


}

Router : Server {
	
	var <broadcast, <sharedNodeIDAllocator;
	var <clientNumber=0; //for multi user dungeons
	
	*new { arg name, addr, options, clientNumber=0, allAddr, latencies;
		^super.new(name, addr, options, clientNumber).initBroadcast(allAddr, latencies)
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
	
	at { arg index;
		^broadcast.allAddr.at(index)
	}
	
	wrapAt { arg index;
		^broadcast.allAddr.wrapAt(index)
	}
	
	do { arg function;
		broadcast.allAddr.do(function)
	}
	
	//isLocal { ^false }
	
	initBroadcast { arg addresses, latencies;
		if(latencies.isNil, {
			broadcast = BroadcastServer.newFrom(this, addresses);
		}, {
			broadcast = DispatchServer.newFrom(this, addresses, latencies);
		});
	}
	
	newAllocators {
		var nodeIdOffset, n;
		n = options.maxNodes;
		nodeIdOffset = 1000 + (clientID * n);
		nodeAllocator = LRUNumberAllocator(nodeIdOffset, nodeIdOffset + n);
		sharedNodeIDAllocator = RingNumberAllocator(16, 800);
		
		controlBusAllocator = PowerOfTwoAllocator(options.numControlBusChannels);
		audioBusAllocator = PowerOfTwoAllocator(options.numAudioBusChannels, 
		options.numInputBusChannels + options.numOutputBusChannels);
		bufferAllocator = PowerOfTwoAllocator(options.numBuffers);
	}
	

}

