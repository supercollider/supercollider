


ProxySpace : EnvironmentRedirect {
	classvar <>all; //access
	
	var <server, <clock, <fadeTime, <>awake=true;
	
	*initClass {
		all = IdentitySet.new;
	}
	
	*new { arg server, name, clock;
		^super.new(name).einit(server ? Server.default, name, clock)
	}
	
	*push { arg server, name, clock;
		^this.new(server, name, clock).push
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
		super.put(\tempo, proxy);
	}
	
	
	//todo add group to target
	einit { arg srv, argName, argClock; 
		server = srv;  
		clock = argClock;
		if(name.notNil, { this.class.all.add(this) });
	}
	
	makeProxy { arg key;
			var proxy;
			proxy = NodeProxy(server);
			proxy.clock = clock;
			if(fadeTime.notNil, { proxy.fadeTime = fadeTime });
			proxy.awake_(awake);
			envir.put(key, proxy);
			^proxy
	}
	
	at { arg key;
		var proxy;
		proxy = super.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		});
		^proxy
	
	}
	
	put { arg key, obj;
		var proxy;
		proxy = envir.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		}, {
			if(obj.isNil, { 
				proxy.clear; 
				this.removeAt(key);  
			});
			
		});
		proxy.source = obj;
	}
		
	
	play { arg key=\out, busIndex=0, nChan=2;
		^this.use({ arg envir;
			this.at(key).play(busIndex, nChan);
		});
	}
	
	stop { arg key=\out;
		^this.use({ arg envir;
			this.at(key).stop;
		});
	}
	
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
	
	free {
		this.do({ arg proxy; proxy.free });
	}
	
	clear {
		this.do({ arg proxy; proxy.clear });
		envir.makeEmpty;
	}
	
	release {
		this.do({ arg proxy; proxy.release });
	}
	
	remove { all.remove(this) }
	
	wakeUp {
		this.use({ arg envir;
			this.do({ arg proxy;
				proxy.wakeUp;
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
	
	removeNeutral { // should be in envir.select
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
		this.sortedKeysValuesDo({ arg key, item; 
			stream <<< key << Char.tab << Char.tab 
			<< if(item.rate === 'audio', { "ar" }, { 
					if(item.rate === 'control', { "kr" }, { "ir" })
					}) 
			<< "(" << item.numChannels << ")   " << Char.nl
		});
		stream << Char.nl
		
	}
		
}

//needs further testing

SharedProxySpace  : ProxySpace {
	
	*new { arg router, name, clock, controlKeys, audioKeys, firstAudioKey=$s;
		^super.new(router, name, clock).addSharedKeys(controlKeys, audioKeys, firstAudioKey)
	}
	
	
	*push { arg router, name, clock, controlKeys, audioKeys, firstAudioKey=$s;
		^this.new(router, name, clock, controlKeys, audioKeys, firstAudioKey).push
	}
	
	//default: initialize single letters as shared busses, 
	//up to firstAudioKey control, the rest audio
		
	addSharedKeys { arg controlKeys, audioKeys, firstAudioKey;
		var nControl, nAudio;
		
		nControl = firstAudioKey.digit - 10;
		nAudio = 26 - nControl;
		controlKeys = controlKeys ?? { this.defaultControlKeys(nControl) };
		this.makeSharedProxy(controlKeys.removeAt(0), 'control', 8); // make multichannel proxy
		controlKeys.do({ arg key; this.makeSharedProxy(key, 'control') });
		
		audioKeys = audioKeys ?? { this.defaultAudioKeys(nAudio, nControl) };
		this.makeSharedProxy(audioKeys.removeAt(0), 'audio', 8); // make multichannel proxy
		audioKeys.do({ arg key; this.makeSharedProxy(key, 'audio') });
	}
	
	defaultControlKeys { arg n;
		^Array.fill(n, { arg i; asSymbol(asAscii(97 + i)) })
	}
	defaultAudioKeys { arg n, offset;
		^Array.fill(n, { arg i; asSymbol(asAscii(97 + offset + i)) }) 
	}

	makeSharedProxy { arg key, rate, numChannels;
			var proxy, broadcast;
			broadcast = server.broadcast;
			proxy = if(rate.isNil, {
				SharedNodeProxy(broadcast)
			},{
				
				SharedNodeProxy.perform(rate, broadcast, numChannels)
			});
			proxy.clock = clock;
			envir.put(key, proxy);
			^proxy
	}
	


}

/*
	to do: 
	-node map thing
	-lazy init
*/

PlayerSpace : ProxySpace {
	
	makeProxy { arg key;
			var proxy;
			proxy = PlayerSocket(\audio, 1, server); //for now..
			envir.put(key, proxy);
			^proxy
	}


	at { arg key;
		var proxy;
		proxy = envir.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		});
		^proxy
	
	}
	
	put { arg key, obj;
		var proxy;
		proxy = envir.at(key);
		if(proxy.isNil, {
			proxy = this.makeProxy(key);
		}, {
			if(obj.isNil, { 
				proxy.free; 
				this.removeAt(key);  
			});
			
		});
		proxy.prepareAndSpawn(obj.asPlayer, 0.1);
	}

}

