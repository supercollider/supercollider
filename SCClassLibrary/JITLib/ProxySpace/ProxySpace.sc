ProxySpace : LazyEnvir {

	classvar <>all;

	var <name, <server, <clock, <fadeTime, <quant;
	var <awake=true, tempoProxy, <group;

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
		if(clock.notNil) { this.quant = 1.0 };
		if(argName.notNil) { this.add };
	}


	makeProxy {
			var proxy = NodeProxy.new;
			this.initProxy(proxy);
			^proxy
	}

	initProxy { arg proxy;
		proxy.server = server;
		proxy.clock = clock;
		proxy.awake = awake;
		if(fadeTime.notNil) { proxy.fadeTime = fadeTime };
		if(group.isPlaying) { proxy.parentGroup = group };
		if(quant.notNil) { proxy.quant = quant };
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

	quant_ { arg val;
		quant = val;
		this.do { arg item; item.quant = val };
	}

	awake_ { arg flag;
		this.do(_.awake_(flag));
		awake = flag;
	}

	makeTempoClock { arg tempo=1.0, beats, seconds;
		var clock, proxy;
		proxy = envir[\tempo];
		if(proxy.isNil) { proxy = NodeProxy.control(server, 1); envir.put(\tempo, proxy); };
		proxy.fadeTime = 0.0;
		proxy.put(0, { |tempo = 1.0| tempo }, 0, [\tempo, tempo]);
		this.clock = TempoBusClock.new(proxy, tempo, beats, seconds).permanent_(true);
		if(quant.isNil) { this.quant = 1.0 };
	}


	play { arg key=\out, out=0, numChannels=2;
		^this.use({ arg envir;
			this.at(key).play(out, numChannels);
		});
	}

	stop { arg fadeTime;
		this.do({ arg proxy; proxy.stop(fadeTime) })
	}

	record { arg key, path, headerFormat="aiff", sampleFormat="int16";
		^this.use({ arg envir;
			this.at(key).record(path, headerFormat="aiff", sampleFormat);
		});
	}

	ar { arg key=\out, numChannels, offset=0;
		^this.use({ arg envir;
			this.at(key).ar(numChannels, offset)
		})
	}

	kr { arg key=\out, numChannels, offset=0;
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
		this.unregisterServer;
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

	// maintaining state

	registerServer {
		ServerQuit.add(this, server);
		ServerBoot.add(this, server);
	}

	unregisterServer {
		ServerQuit.remove(this, server);
		ServerBoot.remove(this, server);
	}

	doOnServerBoot { arg server;
		this.do(_.freeBus);
	}

	doOnServerQuit { arg server;
		this.do(_.freeBus);
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

	getStructure { arg keys, excluding;
		^keys.collect { |key|
			var proxy = envir.at(key);
			var structure;
			proxy !? {
				structure = proxy.getStructure(excluding);
				structure.deepCollect(inf, { |px|
					px !? { envir.findKeyForValue(px) -> px }
				});
			}
		}
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
		} { "a ProxySpace with no name cannot be added to the repository".warn }
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

	includes { |proxy| ^envir.includes(proxy) }

	*findSpace { |proxy, getCode = false|
		var space = [ currentEnvironment, thisProcess.interpreter.p ]
			.detect { |cand|  cand.isKindOf(this) and: { cand.includes(proxy) } };
		if (space.notNil) { ^space };

		space = ProxySpace.all.detect(_.includes(proxy));
		if (space.notNil) { ^space };

		space = Ndef.all.detect(_.includes(proxy));
		if (space.notNil) { ^space };
			// none found
		^nil
	}
}
