
///////// Experimental Network Classes //////////



PublicProxySpace : ProxySpace {

	var <>sendingKeys, <>listeningKeys, <>public=true;
	var <>addresses, <>channel, <>nickname;
	var <>action, <>logSelf=true, <logAll=false, <>basicSafety=true;
	
	classvar <listeningSpaces, <resp;
	
	join { arg channelName, nick=\anybody; 
		channel = channelName;
		nickname = nick;
		if(listeningSpaces.isNil or: { listeningSpaces.includes(this).not }) {
			listeningSpaces = listeningSpaces.add(this)
		};
		("\nwelcome to #" ++ channel ++ "."+"\nplease respect the privacy of others.\n\n").postln;
	}
	
	leave { 
		channel = nil;
		listeningSpaces.remove(this);
	}
	
	put { arg key, obj;
		if(this.sendsTo(key))
		{
			if(obj.isKindOf(Function) and: { obj.isClosed.not}) {
				(Char.bullet ++ " only closed functions can be sent").postln;
				^this
			};
			this.localPut(key, obj);
			this.broadcast(name, key, obj) 
		} {
			this.localPut(key, obj);
		};
	}
	
	lurk { listeningKeys = \all; sendingKeys = nil; }
	boss { listeningKeys = nil; sendingKeys = \all; }
	merge {Ê listeningKeys = \all; sendingKeys = \all; }
	 
	logAll_ { arg flag; if(flag) {
						if(sendingKeys === \all) { logAll = true } 
						{ "please set sendingKeys to \\all first".postln }
					} { logAll = false }
	}
	 
	
	makeLogWindow { arg bounds, color;
	 	var d; 
	 	d = Document((name ? "log").asString);
	 	d.bounds_(bounds ? Rect(10, 400, 600, 500));
	 	d.background_(color ? Color.new255(180, 160, 180));
	 	d.string_("//" + Date.getDate.asString ++ "\n\n\n");
	 	action = { arg ps, nickname, key, str;
	 		defer { 
	 			d.selectRange(d.text.size-1, 0); // deselect user
	 			str = "~" ++ key ++ " = " ++ str;
	 			if(str.last !== $;) { str = str ++ $; };
	 			d.selectedString_(
	 				"\n" ++ "//" + nickname
	 				+ "______"
	 				+ Date.getDate.hourStamp 
	 				+ "________________________________________________\n\n"
	 				++ str
	 				++ "\n\n"
	 			);
	 			d.selectRange(d.text.size-1, 0)
	 		};
	 	};
	 	d.onClose = { action = nil }
	 }
	 
	 *getListening { arg channelName, nickname;
	 	^listeningSpaces.select { |p| 
	 		p.channel === channelName
	 		and:
	 		{ p.nickname !== nickname }  // don't listen to myself
	 	};
	 }
	 
	 *startListen { arg addr; // normally nil
		resp.remove;
		resp = OSCresponderNode(addr, '/proxyspace', { arg time, resp, msg;
			var channel, nickname, key, str, proxyspaces, obj, listeningKeys;
			#nickname, channel, key, str = msg[1..4];
			proxyspaces = this.getListening(channel, nickname);
			proxyspaces.do { arg proxyspace;
				if(proxyspace.listensTo(key))
				{
					str = str.asString;
					if(proxyspace.avoidTheWorst(str)) { 
						obj = str.interpret;
						proxyspace.action.value(proxyspace, nickname, key, str);
					
						if(obj.notNil) {
							proxyspace.localPut(key, obj);
						};
					}
				} { 
					if(proxyspace.logAll) {Ê
						proxyspace.action.value(proxyspace, nickname, key, str) 
					} 
				};
			};
		
		});
		resp.add;

	}
	
	
	*stopListen {
		resp.remove;
		resp = nil;
	}
	
		
	// private implementation //
	
	
	
	localPut { arg key, obj;
		if(currentEnvironment === this) {
				this.at(key).put(0, obj);
		} {
			this.use {
				this.at(key).put(0, obj);
			}
		}
	}
	
	broadcast { arg name, key, obj;
		var str, b;
		str = obj.asCompileString;
		if(logSelf) { action.value(this, nickname, key, str) };
		if(str.size > 8125) { "string too large to publish".postln; ^this };
		if(channel.isNil or: { nickname.isNil }) { Error("first join a channel, please").throw };
		b = ['/proxyspace', nickname, channel, key, str];
		addresses.do { arg addr; addr.sendBundle(nil, b) };
	}
	
	// in future: use a function to allow more flexibility.
	
	sendsTo { arg key;
		^public and: { sendingKeys.notNil } and: 
			{ 
				sendingKeys === \all 
				or: 
				{ sendingKeys.includes(key) } 
			} 
	}
	listensTo { arg key;
		^public and: { listeningKeys.notNil } and: 
			{
				listeningKeys === \all 
				or:
				{ listeningKeys.includes(key) }
			}
	}
	
	avoidTheWorst { arg str;
		^(basicSafety.not) or: {
			str.find("unixCmd").isNil 
			and: { str.find("File").isNil } 
			and: { str.find("Pipe").isNil }
			and: { str.find("PublicProxySpace").isNil }
		}
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
		{ Error("this name" + string + "creates a too high node id. choose a shorter one").throw };
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
