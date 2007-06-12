
BroadcastServer : Server {

	var <>localServer, <>allAddr;
	
	
	*for { arg localServer, allAddr; //local server is a router
		^super.new
				.localServer_(localServer)
				.ninit(localServer.name.asString ++ "_broadcast")
				.allAddr_(allAddr)
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
	
	
	boot { this.notify; this.initTree; }
	quit {}
	
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
		allAddr.do({ arg addr; addr.sendBundle(time, *messages); });
	}
	sendRaw { arg rawArray;
		allAddr.do({ arg addr; addr.sendRaw(rawArray) })
	}
	
	listSendMsg { arg msg;
		allAddr.do({ arg addr; addr.sendBundle(nil,msg) });
	}
 	listSendBundle { arg time,bundle;
 		allAddr.do({ arg addr; addr.sendBundle(time ? this.latency, *bundle) })
	}
		
	status {
		allAddr.do({ arg addr; addr.sendMsg("/status") });
	}
	notify { arg flag=true;
		notified = true;
		allAddr.do({ arg addr; addr.sendMsg("/notify", flag.binaryValue) });
	}
	
	// sync
	sendMsgSync { arg condition ... args;
		var cmdName, count;
		if (condition.isNil) { condition = Condition.new };
		condition.test = false;
		count = allAddr.size;
		cmdName = args[0].asString;
		if (cmdName[0] != $/) { cmdName = cmdName.insert(0, $/) };
		allAddr.do { arg addr;
			var resp;
			resp = OSCresponderNode(addr, "/done", {|time, resp, msg|
			if (msg[1].asString == cmdName) {
				resp.remove;
				count = count - 1;
				if(count == 0) {
					condition.test = true;
					condition.signal;
				};			
			};
			}).add;
		
			addr.sendBundle(nil, args);
		};
		condition.wait;
	}
	
	sync { arg condition, bundles, latency; // array of bundles that cause async action
		var count;
		if (condition.isNil) { condition = Condition.new };
		count = allAddr.size;
		condition.test = false;
		allAddr.do { arg addr;
			var resp, id;
			id = UniqueID.next;
			resp = OSCresponderNode(addr, "/synced", {|time, resp, msg|
			if (msg[1] == id) {
				resp.remove;
				count = count - 1;
				if(count == 0) {
					condition.test = true;
					condition.signal;
				};
			};
			}).add;
			if(bundles.isNil) {
				addr.sendBundle(latency, ["/sync", id]);
			} {
				addr.sendBundle(latency, *(bundles ++ [["/sync", id]]));
			};
		};
		condition.wait;
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
		// alloc shared busses.
		// maybe pre allocate 26 + 8 audio busses
		// and 1000 kr busses?
		
	}
	serverRunning { ^true } //assume that.
	
	//waitForBoot {Êarg func; func.value }
	//stopAliveThread { }
	//startAliveThread { }

}

