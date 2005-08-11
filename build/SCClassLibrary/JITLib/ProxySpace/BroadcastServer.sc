
BroadcastServer {
	var	<>homeServer, <>addresses, >addr;
	var	broadcast = true;
	
	*new { |name, addr, options, clientID|
		^super.new.init(name, addr, options, clientID)
	}
	
	init { arg argName, argAddr, options, clientID;
		homeServer = Server.new(argName.asSymbol, argAddr, options, clientID ? 0);
		addr = argAddr;
	}
	
	*for { arg homeServer, addresses; //local server is a router
		^super.new.homeServer_(homeServer)
			.addr_(homeServer.addr)
			.addresses_(addresses)
	}
	
	// iterating
	
	at { arg index;
		^addresses.clipAt(index)
	}
	
	wrapAt { arg index;
		^addresses.wrapAt(index)
	}
	
	do { arg function;
		^addresses.do(function)
	}
	
	// message forwarding
		
	sendMsg { arg ... args;
		if(broadcast) 
			{ addresses.do({ arg addr; addr.sendMsg(*args) }) }
			{ addr.sendMsg(*args) }
	
	}
	sendBundle { arg time ... messages;
		if(broadcast) 
			{ addresses.do({ arg addr; addr.sendBundle(time, *messages); }) }
			{ addr.sendBundle(time, *messages) }
	}
	sendRaw { arg rawArray;
		if(broadcast) 
			{ addresses.do({ arg addr; addr.sendRaw(rawArray) }) } 
			{ addr.sendRaw(rawArray) }
	}
	
	listSendMsg { arg msg;
		if(broadcast) 
			{ addresses.do({ arg addr; addr.sendMsg(*msg) }) }
			{ addr.sendMsg(*msg) }
	}
 	listSendBundle { arg time,bundle;
 		if(broadcast) 
 			{ addresses.do({ arg addr; addr.sendBundle(time, *bundle) }) }
 			{ addr.sendBundle(time, *bundle) }
 		
	}
	
	openBundle { arg bundle;  // pass in a bundle that you want to continue adding to, or nil for a new bundle.
		addr = BundleNetAddr.copyFrom(addr, bundle);
		broadcast = false;
	}
	closeBundle { arg time; // set time to false if you don't want to send.
		var bundle;
		bundle = addr.bundle;
		addr = addr.saveAddr;
		broadcast = true;
		if (time != false) { this.listSendBundle(time, bundle); }
		^bundle;
	}
	makeBundle { arg time, func, bundle;
		this.openBundle(bundle);
		try {
			func.value(this);
			bundle = this.closeBundle(time);
		}{|error|
			addr = addr.saveAddr; // on error restore the normal NetAddr
			broadcast = true;
			error.throw
		}
		^bundle
	}
	

	options { ^homeServer.options }
	
	makeWindow { homeServer.makeWindow }
	
	boot { 
		var bundle;
		homeServer.waitForBoot { 
			this.notify; 
			bundle = homeServer.makeBundle(false, { homeServer.initTree });
			this.listSendBundle(nil, bundle);
		}
	}

	serverRunning { ^homeServer.serverRunning }
	serverBooting { ^homeServer.serverBooting }
	
	status {
		this.sendMsg("/status");
	}
	notify { arg flag=true;
		this.sendMsg("/notify", flag.binaryValue);
	}
	
	asTarget { ^homeServer.asTarget }

	// use home server allocators

	nodeAllocator { ^homeServer.nodeAllocator }
	staticNodeAllocator { ^homeServer.staticNodeAllocator }
	controlBusAllocator { ^homeServer.controlBusAllocator }
	audioBusAllocator { ^homeServer.audioBusAllocator }
	bufferAllocator { ^homeServer.bufferAllocator }
	
	nextNodeID { ^homeServer.nodeAllocator.alloc }

		// compatibility of other server messages
	doesNotUnderstand { |selector ... args|
		^homeServer.perform(selector, *args)
	}
	

	// sync
	sendMsgSync { arg condition ... args;
		var cmdName, count;
		if (condition.isNil) { condition = Condition.new };
		condition.test = false;
		count = addresses.size;
		cmdName = args[0].asString;
		if (cmdName[0] != $/) { cmdName = cmdName.insert(0, $/) };
		addresses.do { arg addr;
			var resp;
			resp = OSCresponderNode(addr, "/done", { |time, resp, msg|
			[\synced, args].postln;

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
		count = addresses.size;
		condition.test = false;
		addresses.do { arg addr;
			var resp, id;
			id = UniqueID.next;
			resp = OSCresponderNode(addr, "/synced", { |time, resp, msg|
			addr.dump;
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
	

}

