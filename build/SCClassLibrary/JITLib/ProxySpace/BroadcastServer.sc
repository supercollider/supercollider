
BroadcastServer : Server {

	var <>localServer, <>allAddr;
	
	
	*for { arg localServer, allAddr; //local server is a router
		^super.new
				.localServer_(localServer)
				.ninit(localServer.name.asString ++ "_broadcast")
				.allAddr_(allAddr)
				.newNodeWatcher
				
				
				 
	}
	// doesn't work currently
	autoConfigure { arg getAnyApplication=false;
		var addr;
		addr = Set.new;
		OSCService.knownServices.do({ arg item;
			if(getAnyApplication || (item.name == "SuperCollider"), {
				addr = addr.add(NetAddr(item.hostname, item.port));
				[item.name, item.hostname, item.port].postln;
			});
		});
		allAddr = Array.newFrom(addr)
			.sort({ arg a, b; (a.hostname+a.port) < (a.hostname+a.port) });
	}
	
	////we don't need much here, this class wraps the messages
	ninit { arg argName;
		name = argName; 
		addr = localServer.addr;
		options = localServer.options;
		isLocal = false;
		named.put(name, this);
	}
	
	init {}
	
	addr { ^allAddr } //support NodeWatcher.register
	
	makeWindow {} //doesn't make sense, use Router for gui
	
	newNodeWatcher {
		nodeWatcher = NodeWatcher.new(this);
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
	waitForBoot {Êarg func; func.value }
	
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
	nextSharedNodeID { ^localServer.nextSharedNodeID }
}



// handles node id allocation etc.

Router : Server {
	
	var <broadcast, <sharedNodeIDAllocator;
	
	
	addr_ { arg list, latencies; //change this args.
		broadcast = BroadcastServer.for(this, list);
	}
	
	autoConfigure { arg getAnyApplication=false;
		broadcast.autoConfigure(getAnyApplication);
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
	
	nextSharedNodeID {
		^sharedNodeIDAllocator.alloc
	}
	
	newAllocators {
	
		super.newAllocators;
		sharedNodeIDAllocator = RingNumberAllocator(128, 800);
		
	}
	serverRunning { ^true } //assume that.
	waitForBoot {Êarg func; func.value }
	stopAliveThread { }
	startAliveThread { }

}

