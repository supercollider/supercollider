NetAddr {
	var <addr=0, <>port=0, <hostname, <socket;
	classvar connections;

	*initClass {
		connections = IdentityDictionary.new;
	}

	*new { arg hostname, port=0;
		var addr;
		addr = if (hostname.notNil,{ hostname.gethostbyname },{ 0 });
		^super.newCopyArgs(addr, port, hostname);
	}
	*fromIP { arg addr, port=0;
		^super.newCopyArgs(addr, port, addr.asIPString)
	}

	*langPort {
		_GetLangPort
	}
	*localAddr {
		^this.new("127.0.0.1", this.langPort)
	}
	*useDoubles_ { arg flag = false;
		_NetAddr_UseDoubles
		^this.primitiveFailed;
	}
	*broadcastFlag {
		_NetAddr_GetBroadcastFlag
		^this.primitiveFailed
	}
	*broadcastFlag_ { arg flag = true;
		_NetAddr_SetBroadcastFlag
		^this.primitiveFailed
	}

	*disconnectAll {
		connections.keys.do({ | netAddr |
			netAddr.disconnect;
		});
	}

	hostname_ { arg inHostname;
		hostname = inHostname;
		addr = inHostname.gethostbyname;
	}


	sendRaw { arg rawArray;
		_NetAddr_SendRaw
		^this.primitiveFailed;
	}
	sendMsg { arg ... args;
		_NetAddr_SendMsg
		^this.primitiveFailed;
	}
	// warning: this primitive will fail to send if the bundle size is too large
	// but it will not throw an error.  this needs to be fixed
	sendBundle { arg time ... args;
		_NetAddr_SendBundle
		^this.primitiveFailed;
	}
	sendStatusMsg {
		this.sendMsg("/status");
	}
	sendClumpedBundles { arg time ... args;
		if(args.bundleSize > 65535) {// udp max size.
				args.clumpBundles.do { |item|
					if(time.notNil) { time = time + 1e-9 }; // make it a nanosecond later
					this.sendBundle(time, *item)
				};
			} {
				this.sendBundle(time, *args)
		}
	}

	sync { arg condition, bundles, latency; // array of bundles that cause async action
		var resp, id;
		if (condition.isNil) { condition = Condition.new };
		if(bundles.isNil) {
			id = this.makeSyncResponder(condition);
			this.sendBundle(latency, ["/sync", id]);
			condition.wait;
		} {
			// not sure what the exact size is, but its about 20000
			// this relates to what _NetAddr_SendBundle can send
 			if(bundles.bundleSize > 20000/*65515*/) { // 65515 = 65535 - 16 - 4 (sync msg size)
				bundles.clumpBundles.do { |item|
					id = this.makeSyncResponder(condition);
					this.sendBundle(latency, *(item ++ [["/sync", id]]));
					if(latency.notNil) { latency = latency + 1e-9 };
					condition.wait;
				}
			} {
				id = this.makeSyncResponder(condition);
				this.sendBundle(latency, *(bundles ++ [["/sync", id]]));
				condition.wait;
			}
		};
		// maybe needed: a timeout
	}

	makeSyncResponder { arg condition;
			var id = UniqueID.next;
			var resp;

			resp = OSCFunc({|msg|
				if (msg[1] == id) {
					resp.free;
					condition.test = true;
					condition.signal;
				};
			}, '/synced', this);
			condition.test = false;
			^id
	}

	isConnected {
		^socket.notNil
	}
	connect { | disconnectHandler |
		if (this.isConnected.not) {
			this.prConnect;
			connections.put(this, disconnectHandler);
		};
	}
	disconnect {
		if (this.isConnected) {
			this.prDisconnect;
			this.prConnectionClosed;
		};
	}

	== { arg that;
		^this.compareObject(that, #[\port, \addr])
	}

	hash {
		^this.instVarHash(#[\port, \addr])
	}

	// Asymmetric: "that" may be nil or have nil port (wildcards)
	matches { arg that;
		^this==that or:{
			that.isNil or: {
				this.addr == that.addr and: { that.port.isNil }
			}
		}
	}

	ip {
		^addr.asIPString
	}

	hasBundle { ^false }

	printOn { | stream |
		super.printOn(stream);
		stream << $( << this.ip << ", " << port << $)
	}
	storeOn { | stream |
		super.storeOn(stream);
		stream << $( << "\"" << this.ip << "\", " << port << $)
	}

	// PRIVATE
	prConnect {
		_NetAddr_Connect
		^this.primitiveFailed;
	}
	prDisconnect {
		_NetAddr_Disconnect
		^this.primitiveFailed;
	}
	prConnectionClosed {
		// called when connection is closed either by sclang or by peer
		socket = nil;
		connections.removeAt(this).value(this);
	}
	recover { ^this }
}

BundleNetAddr : NetAddr {
	var <saveAddr, <>bundle;
	var <async = false;

	*copyFrom { arg addr, bundle;
		^super.newCopyArgs(addr.addr, addr.port, addr.hostname, addr.socket, addr, bundle ? []);
	}

	sendRaw { arg rawArray;
		bundle = bundle.add( rawArray );
	}
	sendMsg { arg ... args;
		bundle = bundle.add( args );
	}
	sendBundle { arg time ... args;
		bundle = bundle.addAll( args );
	}
	sendClumpedBundles { arg time ... args;
		bundle = bundle.addAll( args );
	}
	sendStatusMsg {} // ignore status messages

	recover {
		^saveAddr.recover
	}
	hasBundle { ^true }


	sync { arg condition, bundles, latency;
		bundle = bundle.add([\syncFlag, bundles, latency]);
			// not sure about condition. here we ignore it.
		async = true;
	}

	closeBundle { arg time;
		var bundleList, lastBundles;
		if(time != false) {
			if(async.not) {
				saveAddr.sendClumpedBundles(time, *bundle);
				^bundle;
			};

			forkIfNeeded {
					bundleList = this.splitBundles(time);
					lastBundles = bundleList.pop;
					bundleList.do { |bundles|
						var t = bundles.removeAt(0);
						saveAddr.sync(nil, bundles, t); // make an independent condition.
					};
					saveAddr.sendClumpedBundles(*lastBundles);  // time ... args
			}
		};
		^bundle
	}

	splitBundles { arg time;
		var res, curr;
		curr = [time]; // first element in the array is always the time
		bundle.do { |item|
			if(item[0] === \syncFlag) {
				curr = curr.addAll(item[1]); // then comes the messages
				res = res.add(curr);
				curr = [item[2]]; // time
			} {
				curr = curr.add(item)
			}
		};
		res = res.add(curr);
		^res
	}
}
