
ProxySpace : LazyEnvir {

	classvar <>all;
	
	var <name, <server, <clock, <fadeTime;
	var <>awake=true, tempoProxy, <group;
	
	*initClass { all = IdentityDictionary.new }
	
	*new { arg server, name, clock;
		^super.new.init(server ? Server.default, name, clock)
	}
	
	*push { arg server, name, clock;
		if(name.isNil and: { currentEnvironment.isKindOf(this) }) 
			{ currentEnvironment.clear.pop }; // avoid nesting
		^this.new(server, name, clock).push;
	}
	
	init { arg argServer, argName, argClock;
		server = argServer;  
		clock = argClock;
		name = argName;
		this.add;
	}
	
	
	makeProxy {
			var proxy;
			proxy = NodeProxy.new(server);
			proxy.clock = clock;
			proxy.awake = awake;
			if(fadeTime.notNil) { proxy.fadeTime = fadeTime };
			if(group.isPlaying) { proxy.parentGroup = group };
			^proxy
	}
	
	
	// access and control
	
	clock_ { arg aClock;
		clock = aClock;
		this.do { arg item; item.clock = aClock };
	}
	
	fadeTime_ { arg dt;
		fadeTime = dt;
		this.do { arg item; item.fadeTime = dt };
	}
	
	group_ { arg node;
		node = node.asGroup;
		NodeWatcher.register(node, true);
		if(node.isPlaying.not) { "group % not playing!".postf(node); ^this };
		group = node;
		this.do { arg item; item.parentGroup = node };
	}
	
	makeTempoClock { arg tempo=1.0, beats, seconds;
		var clock, proxy;
		proxy = envir[\tempo];
		if(proxy.isNil) { proxy = NodeProxy.control(server, 1); envir.put(\tempo, proxy); };
		proxy.fadeTime = 0.0;
		proxy.put(0, { |tempo = 1.0| tempo }, 0, [\tempo, tempo]);
		this.clock = TempoBusClock.new(proxy, tempo, beats, seconds).permanent_(true);
	}

	
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
	
	clear { arg fadeTime=0.0;
		this.do({ arg proxy; proxy.clear(fadeTime) });
		tempoProxy !? { tempoProxy.clear };
		super.clear;
	}
	
	end { arg fadeTime;
		this.do({ arg proxy; proxy.end(fadeTime) });
		tempoProxy.free;
	}
	
	release { arg fadeTime;
		this.do({ arg proxy; proxy.release(fadeTime) });
		tempoProxy.free;
	}
		
	wakeUp {
		this.use({ arg envir;
			this.do({ arg proxy;
				proxy.deepWakeUp;
			});
		});
	}
	
	// garbage collector
	
	// ends all proxies that are not needed
	reduce { arg excluding, method=\end;
		this.gcList(excluding).do { arg proxy; proxy.perform(method) };
	}
	
	// removes all proxies that are not needed
	clean { arg excluding;
		this.reduce(nil, \clear);
		this.removeNeutral;
	}
	
	removeNeutral {
		envir.copy.keysValuesDo { arg key, val; if(val.isNeutral) { envir.removeAt(key) } };
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
			if(proxy.monitor.isPlaying) { monitors =  monitors.add(proxy) }
		};
		^monitors
	}
	
	activeProxies { ^this.arProxyNames({ |px, key| px.isPlaying }) }
	
	playingProxies { ^this.arProxyNames({ |px, key| px.monitor.isPlaying }) }
	
	existingProxies { ^this.arProxyNames }
	
	arProxyNames { |func=true| ^this.proxyNames(\audio, func) }
	krProxyNames { |func=true| ^this.proxyNames(\control, func) } 

	proxyNames { |rate, func=true| 
		var pxs; 
		pxs = SortedList(8, { |a,b| a < b }); 
		this.keysValuesDo({ arg key, px;
			if (px.rate === rate)  { 
				if (func.value(px, key), { pxs.add(key) }) 
			} 
		});
		^pxs;
	}

	doFunctionPerform { arg selector; ^this[selector] }
	
	// global access
	
	add {
		if(name.notNil) { 
			if(this.class.all[name].isNil) 
			{ this.class.all.put(name, this) } 
			{ "there is already an environment with this name".warn };
		}
	}
	
	remove {
		this.class.all.removeAt(name)
	}

	
	*clearAll {
		this.all.do({ arg item; item.clear });
	}
	
	
	printOn { arg stream;
		stream << this.class.name;
		if(envir.isEmpty) { stream << " ()\n"; ^this };
		stream << " ( " << (name ? "") << Char.nl;
		this.keysValuesDo { arg key, item, i;
			stream << "~" << key << " - "; 
			stream << if(item.rate === 'audio') { "ar" } { 
					if(item.rate === 'control', { "kr" }, { "ir" })
					}
			<< "(" << item.numChannels << ")   " << if(i.even) { "\t\t" } { "\n" };
		};
		stream << "\n)\n"
		
	}
	
	postln { Post << this }
	
}	

