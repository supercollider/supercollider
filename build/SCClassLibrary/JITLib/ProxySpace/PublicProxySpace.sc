
///////// Experimental Network Classes //////////



PublicProxySpace : ProxySpace {

	var <>sendingKeys, <>listeningKeys;
	var <>addressList, <>sendToName;
	classvar <>all, <resp;
	
	*initClass { all = IdentityDictionary.new }

	put { arg key, obj; 
		var notCurrent;
		notCurrent = currentEnvironment !== this;
		if(notCurrent) { this.push };
		if(this.sendsTo(key))
		{Ê
			if(obj.isKindOf(Function) and: {obj.isClosed.not}) {
				(Char.bullet ++ " only closed functions can be sent").postln;
				^this
			};
			this.at(key).put(nil, obj);
			this.broadcast(name, key, obj) 
		};
		try { this.at(key).put(nil, obj) }; // test
		if(notCurrent) { this.pop };
	}
	
	remotePut { arg key, obj;
		if(currentEnvironment === this) {
				this.at(key).put(nil, obj);
		} {
			"not current".postln;
			this.use {
				key.envirGet.postln;
				this.at(key).put(nil, obj);
			}
		}
	}
	
	broadcast { arg name, key, obj;
		var str, b;
		str = obj.asCompileString;
		if(str.size > 8125) {Ê"string too large to publish".postln; ^this };		b = ['/proxyspace', sendToName ? name, key, str];
		addressList.do { arg addr; addr.sendBundle(nil, b) };
	}
	
	sendsTo { arg key;
		^sendingKeys.notNil and: 
			{ 
				sendingKeys === \all 
				or: 
				{ sendingKeys.includes(key) } 
			} 
	}
	listensTo {Êarg key;
		^listeningKeys.notNil and: 
			{
				listeningKeys === \all 
				or:
				{ listeningKeys.includes(key) }
			}
	}
	
	*startListen { arg addr; // normally nil
		resp.remove;
		resp = OSCresponderNode(addr, '/proxyspace', { arg time, resp, msg;
			var name, key, str, proxyspace, obj, listeningKeys;
			#name, key, str = msg[1..3];
			[name, key, str].postln;
			proxyspace = all[name];
			if(proxyspace.notNil) {
				if(proxyspace.listensTo(key)) {
					obj = str.asString.interpret;
					if(obj.notNil) {
						proxyspace.remotePut(key, obj);
					};
				};
			};
		
		});
		resp.add;
	
	}
	
	
	*stopListen {
		resp.remove;
		resp = nil;
	}

}


SharedProxySpace : ProxySpace {

	var <>broadcast;	
	
	*new { arg broadcast, name, clock; // server should be a fresh, unused, broadcast-server
		^super.new(broadcast.homeServer, name, clock).broadcast_(broadcast);
	}
	
	makeSharedProxy { arg key, rate=\audio, numChannels=2;
			var proxy, groupID;
			groupID = this.stringToNodeID(key.asString);
			proxy = if(rate.isNil, {
				SharedNodeProxy.new(broadcast, groupID)
			},{
				
				SharedNodeProxy.new(broadcast, groupID).defineBus(rate, numChannels);			});
			proxy.clock = clock;
			envir.put(key, proxy);
			^proxy
	}

	// default: initialize single letters as shared busses, 
	// up to firstAudioKey control, the rest audio
	// in shared networks this has to be done initially, so server keys match.
	
	// todo: think about shared busses.
	
	addSharedKeys { arg controlKeys, audioKeys, firstAudioKey=$s;
	
		if(controlKeys.isNil) { 
			controlKeys = this.class.defaultControlKeys(firstAudioKey.digit - 10) 
		};
		if(audioKeys.isNil) { 
			audioKeys = this.class.defaultAudioKeys(26 - controlKeys.size, controlKeys.size) 
		};
		
		controlKeys.do({ arg key; this.makeSharedProxy(key, 'control') });
		audioKeys.do({ arg key; this.makeSharedProxy(key, 'audio') });
	}
	
	*defaultControlKeys { arg n;
		^Array.fill(n, { arg i; asSymbol(asAscii(97 + i)) })
	}
	*defaultAudioKeys { arg n, offset;
		^Array.fill(n, { arg i; asSymbol(asAscii(97 + offset + i)) }) 
	}

	stringToNodeID { arg string;
		var res=2;
		string.do { arg char, i; 
			var a;
			a = char.ascii;
			if(a < 97 or: { a > 122 }) { Error("use only lower case letters in shared names").throw };
			res = res + (a - 97 + (i * 26)); 
		};
		if(res > 999) // use nodeIDs between 2 and 999
		{ÊError("this name" + string + "creates a too high node id. choose a shorter one").throw };
		^res
	}

	clear {		// assume user error: free shared proxies, 
				// but keep them in envir to avoid buffer re-alloc madness.
		this.do({ arg proxy; proxy.clear });
		tempoProxy.clear;
	}
	
}






/*
	to do: 
	-node map thing
	-lazy init


PlayerSpace : ProxySpace {
	
	makeProxy {
			var proxy;
			proxy = PlayerSocket(\audio, 1, server); //for now..
			^proxy
	}


	at { arg key;
		var proxy;
		proxy = envir.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
			envir.put(key, proxy);
		});
		^proxy
	
	}
	
	put { arg key, obj;
		var proxy;
		proxy = this.at(key);
		proxy.prepareAndSpawn(obj.asPlayer, 0.1);
	}

}

*/
