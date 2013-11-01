// used to bundle and cache set, map, mapa, setn, mapn, mapna commands.


NodeMap {
	var <>settings; // cache args:
	var <>upToDate, <>setArgs, <>setnArgs, <>mapArgs, <>mapnArgs, <>mapaArgs, <>mapanArgs;
	var controlNames;

	*new {
		^super.new.clear
	}

	settingClass {
		^NodeMapSetting
	}


	clear {
		settings = IdentityDictionary.new;
		upToDate = false;
		this.changed(\clear);
	}


	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).map(args.at(i+1));
		});
		upToDate = false;
		this.changed(\map, args);
	}

	mapa { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).mapa(args.at(i+1));
		});
		upToDate = false;
		this.changed(\map, args);
	}

	unmap { arg ... keys;
		keys.do { arg key;
			var setting;
			setting = settings.at(key);
			if(setting.notNil and: { setting.isMapped }) {
				setting.map(nil, nil);
				if(setting.isEmpty) { settings.removeAt(key) }
			};
		};
		upToDate = false;
		this.changed(\unmap, keys);

	}

	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).set(args.at(i+1));
		});
		upToDate = false;
		this.changed(\set, args);
	}

	setn { arg ... args; this.set(*args)  }


	unset { arg ... keys;
		keys.do { arg key;
			var setting;
			setting = settings.at(key);
			if(setting.notNil and: { setting.isMapped.not }) {
				setting.map(nil, nil);
				if(setting.isEmpty) { settings.removeAt(key) }
			};
		};
		upToDate = false;
		this.changed(\unset, keys);
	}

	mapn { arg ... args;
		forBy(0, args.size-1, 3, { arg i;
			this.get(args.at(i)).mapn(args.at(i+1), args.at(i+2));
		});
		upToDate = false;
		this.changed(\mapn, args);
	}

	mapan { arg ... args;
		forBy(0, args.size-1, 3, { arg i;
			this.get(args.at(i)).mapan(args.at(i+1), args.at(i+2));
		});
		upToDate = false;
		this.changed(\mapan, args);
	}

	send { arg server, nodeID, latency;
		var bundle;
		bundle = List.new;
		this.addToBundle(bundle, nodeID);
		server.listSendBundle(latency, bundle);
	}

	sendToNode { arg node, latency;
		node = node.asTarget;
		this.send(node.server, node.nodeID, latency)
	}

	controlNames {
		this.updateBundle;
		^controlNames
	}

	get { arg key;
		var setting;
		setting = settings.at(key);
		if(setting.isNil, {
			setting = this.settingClass.new(key);
			settings.put(key, setting)
		});
		^setting
	}

	at { arg key;
		^settings.at(key)
	}

	settingKeys {
		var res;
		settings.do { arg item; if(item.isMapped.not) { res = res.add(item.key) } };
		^res
	}
	mappingKeys {
		var res;
		settings.do { arg item; if(item.isMapped) { res = res.add(item.key) } };
		^res
	}

	updateBundle {
			if(upToDate.not) {
				upToDate = true;
				setArgs = setnArgs = mapArgs = mapnArgs = mapaArgs = mapanArgs = nil;
				settings.do { arg item; item.updateNodeMap(this) };
				controlNames = settings.values.collect(_.asControlName);
			}
	}

	addToEvent { arg event;
		settings.do { |x| x.addToEvent(event) }
	}

	addToBundle { arg bundle, target;
			var msgs;
			target = target.asNodeID;
			this.updateBundle;
			if(setArgs.notNil) { bundle.add([15, target] ++ setArgs) };
			if(setnArgs.notNil) { bundle.add([16, target] ++ setnArgs) };
			if(mapArgs.notNil) { bundle.add([14, target] ++ mapArgs) };
			if(mapnArgs.notNil) { bundle.add([48, target] ++ mapnArgs) };
			if(mapaArgs.notNil) { bundle.add([60, target] ++ mapaArgs) };
			if(mapanArgs.notNil) { bundle.add([61, target] ++ mapanArgs) };
	}

	unsetArgsToBundle { arg bundle, target, keys;
		var args;
		if(settings.isEmpty) { ^this };
		(keys ?? { settings.keys }).do { arg key;
			var item;
			item = settings[key];
			if(item.notNil) { args = args ++ [key, -1] }
		};
		if(args.notNil) { bundle.add([15, target.asNodeID] ++ args) };
	}

	unmapArgsToBundle { arg bundle, target, keys;
		var args;
		if(settings.isEmpty) { ^this };
		(keys ?? { settings.keys }).do { arg key;
			var item;
			item = settings[key];
			if(item.notNil and: { item.isMapped }) {
					args = args ++ [key, -1, item.busNumChannels];
			};
		};
		if(args.notNil) { bundle.add([48, target.asNodeID] ++ args) };
	}

	blend { arg another, frac;
		var res = this.copy;
		another.updateBundle;
		another.setArgs.pairsDo { |key, x|
			var s = settings[key], y;
			s !? { y = s.getValue };
			y !? { res.set(key, blend(y, x, frac)) }
		};
		^res
	}


	copy {
		var res, nset;
		res = this.class.new;
		nset = res.settings;
		settings.keysValuesDo({ arg key, val; nset.put(key, val.copy) });
		^res
	}

	printOn { arg stream;
		stream << this.class.name << "(";
		settings.printItemsOn(stream);
		stream << ")"
	}


}


ProxyNodeMap : NodeMap {

		var <>parents, <>proxy;
		var hasRates=false;

		clear {
			super.clear;
			parents = IdentityDictionary.new;
		}

		settingClass {
			^ProxyNodeMapSetting
		}

		wakeUpParentsToBundle { arg bundle, checkedAlready;
			parents.do({ arg item; item.wakeUpToBundle(bundle, checkedAlready) });
		}

		setRates { arg args;
			forBy(0, args.size-1, 2, { arg i;
				var key, rate, setting;
				key = args[i];
				setting = this.get(key);
				rate = args[i+1];
				if(rate.isNil and: { setting.notNil } and: { setting.isEmpty })
				{ settings.removeAt(key) }
				{ setting.rate_(rate) };
			});
			hasRates = settings.any { arg item; item.rate.notNil };
		}

		ratesFor { arg keys;
			^if(hasRates) {
				keys.collect({ arg key;
					var res;
					res = settings.at(key);
					if(res.notNil, { res.rate }, { nil })
				})
			} { nil }
		}

		mapn { arg ... args;
			this.map(args) // for now, avoid errors.
		}

		map { arg ... args;
			var playing;
			playing = proxy.isPlaying;
			args.pairsDo { arg key, mapProxy;
				var setting, numChannels;
				if(mapProxy.isKindOf(BusPlug).not) { Error("map: not a node proxy").throw };
				if(playing) { mapProxy.wakeUp };
				setting = this.get(key);
				setting.map(mapProxy);
				parents = parents.put(key, mapProxy);
			};
			upToDate = false;
			this.changed(\map, args);
		}

		mapEnvir { arg ... keys;
			var args;
			keys = keys ? settings.keys;
			args = Array.new(keys.size*2);
			keys.do({ arg key; args.add(key); args.add(currentEnvironment.at(key)) });
			this.map(*args);
		}

		unmap { arg ... keys;
			var setting;
			if(keys.at(0).isNil, { keys = this.mappingKeys });
			keys.do({ arg key;
				setting = settings.at(key);
				if(setting.notNil, {
					setting.map(nil, nil);
					parents.removeAt(key);
					if(setting.isEmpty, { settings.removeAt(key) })
				});
			});
			upToDate = false;
			this.changed(\unmap, keys);
		}

		changed { arg ... args;
			proxy.changed(*args)
		}

}
