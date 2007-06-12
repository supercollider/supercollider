
ProxySpace : EnvironmentRedirect {
	classvar <>all; //access
	
	var <server, <clock, <fadeTime, <>awake=true, tempoProxy;
	
	*initClass {
		all = IdentitySet.new;
	}
	
	*new { arg server, name, clock;
		^super.new(name).einit(server ? Server.default, name, clock)
	}
	
	*push { arg server, name, clock;
		if(name.isNil and: { currentEnvironment.isKindOf(this) }) { currentEnvironment.clear };
		^this.new(server, name, clock).push;
	}
	
	einit { arg srv, argName, argClock; 
		server = srv;  
		clock = argClock;
		if(name.notNil, { this.class.all.add(this) });
	}
	
	clock_ { arg aClock;
		clock = aClock;
		this.do({ arg item; item.clock = aClock });
	}
	
	fadeTime_ { arg dt;
		fadeTime = dt;
		this.do({ arg item; item.fadeTime = dt });
	}
	
	makeTempoClock { arg tempo=1, beats, seconds;
		var clock, proxy;
		proxy = NodeProxy.control(server, 1);
		proxy.fadeTime = 0.0;
		proxy.source = tempo;
		this.clock = TempoBusClock.new(proxy, tempo, beats, seconds).permanent_(true);
		envir.proto.put(\tempo, proxy);
	}
	
	
	makeProxy {
			var proxy;
			proxy = NodeProxy(server);
			proxy.clock = clock;
			proxy.awake = awake;
			if(fadeTime.notNil, { proxy.fadeTime = fadeTime });
			^proxy
	}
	
	//////// redirects
	
	at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil) {
			proxy = this.makeProxy(key);
			envir.put(key, proxy);
		};
		^proxy
	
	}
	
	put { arg key, obj;
		this.at(key).source = obj;
	}
	
	removeAt { arg key;
		var proxy;
		proxy = envir.removeAt(key);
		if(proxy.notNil) { proxy.clear };
	}
	
	///////////////
		
	
	play { arg key=\out, busIndex=0, nChan=2;
		^this.use({ arg envir;
			this.at(key).play(busIndex, nChan);
		});
	}
	
	stop { arg key=\out, fadeTime; this.at(key).stop(fadeTime) }
	
	record { arg key, path, headerFormat="aiff", sampleFormat="int16";
		^this.use({ arg envir;
			this.at(key).record(path, headerFormat="aiff", sampleFormat);
		});
	}
	
	ar { arg key, numChannels, offset=0;
		^this.use({ arg envir;
			this.at(key).ar(numChannels, offset)
		})
	}
	
	kr { arg key, numChannels, offset=0;
		^this.use({ arg envir;
			this.at(key).kr(numChannels, offset)
		})
	}
	
	free { arg fadeTime;
		this.do { arg proxy; proxy.free(fadeTime) };
		tempoProxy.free;
	}
	
	clear {
		this.do({ arg proxy; proxy.clear });
		tempoProxy.clear;
		super.clear;
	}
	
	release { arg fadeTime;
		this.do({ arg proxy; proxy.release(fadeTime) });
		tempoProxy.free;
	}
	
	remove { ^all.remove(this) }
	
	wakeUp {
		this.use({ arg envir;
			this.do({ arg proxy;
				proxy.deepWakeUp;
			});
		});
	}
	
	// garbage collector
	
	// ends all proxies that are not needed
	reduce { arg excluding;
		this.gcList(excluding).do { arg proxy; proxy.end };
	}
	
	// removes all proxies that are not needed
	clean { arg excluding;
		this.gcList(excluding).do { arg proxy; proxy.clear };
		this.removeNeutral;
	}
	
	removeNeutral { // rejectInPlace should be in envir.select
		var newEnvir;
		newEnvir = envir.copy;
		envir.keysValuesDo { arg key, val; if(val.isNeutral) {  newEnvir.removeAt(key) } };
		envir = newEnvir;
	}
	// get a list of all proxies that are not reached either by the list passed in
	// or (if nil) by the monitoring proxies
	gcList { arg excluding;
		var monitors, all, toBeKept, res; 
		monitors = excluding ?? { this.monitors };
		toBeKept = IdentitySet.new;
		res = List.new;
		monitors.do { arg proxy; proxy.getFamily(toBeKept) };
		envir.do { arg proxy; if(toBeKept.includes(proxy).not) { res.add(proxy) } };
		^res
	}
	monitors {
		var monitors;
		monitors = Array.new;
		envir.do { arg proxy; 
			if(proxy.monitorGroup.isPlaying) { monitors =  monitors.add(proxy) }
		};
		^monitors
	}
	
	// global clearing up
	
	*clearAll {
		all.do({ arg item; item.clear });
	}
	
	
	printOn { arg stream;
		stream << "ProxySpace: " << Char.nl;
		this.keysValuesDo { arg key, item, i;
			key = key.asString;
			stream << "~" << key << Char.tab << Char.tab << if(key.size < 3) { Char.tab } { "" } 
			<< if(item.rate === 'audio') { "ar" } { 
					if(item.rate === 'control', { "kr" }, { "ir" })
					}
			<< "(" << item.numChannels << ")   " << if(i.even) { Char.tab } { Char.nl };
		};
		stream << Char.nl
		
	}
	postln { Post << this }
		
	//////// shared proxy space
	
	makeSharedProxy { arg key, rate=\audio, numChannels=2;
			var proxy, broadcast, groupID;
			broadcast = server.broadcast;
			groupID = this.stringToNodeID(key.asString);
			proxy = if(rate.isNil, {
				SharedNodeProxy(broadcast, groupID)
			},{
				
				SharedNodeProxy.perform(rate, broadcast, numChannels)
			});
			proxy.clock = clock;
			envir.put(key, proxy);
			^proxy
	}

	// default: initialize single letters as shared busses, 
	// up to firstAudioKey control, the rest audio
	// in shared networks this has to be done initially, so server keys match.
	
	// todo: think about shared busses.
	
	addSharedKeys { arg controlKeys, audioKeys, firstAudioKey=$s;
		var nControl, nAudio;
		
		nControl = firstAudioKey.digit - 10;
		nAudio = 26 - nControl;
		controlKeys = controlKeys ?? { this.class.defaultControlKeys(nControl) };
		this.makeSharedProxy(controlKeys.removeAt(0), 'control', 8); // make 1 multichannel proxy
		controlKeys.do({ arg key; this.makeSharedProxy(key, 'control') });
		
		audioKeys = audioKeys ?? { this.class.defaultAudioKeys(nAudio, nControl) };
		this.makeSharedProxy(audioKeys.removeAt(0), 'audio', 8); // make multichannel proxy
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
			if(a < 97 or: a > 122) { Error("use only lower case letters in shared names").throw };
			res = res + (a - 97 + (i * 26)); 
		};
		if(res > 999) // use nodeIDs between 2 and 999
		{ÊError("this name" + string + "creates a too high node id. choose a shorter one").throw };
		^res
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
