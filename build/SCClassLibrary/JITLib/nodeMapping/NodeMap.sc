//used to store and cache set, map setn commands


NodeMap {
	var <>settings;
	var <bundle, <upToDate=false;
	var <>setArgs, <>setnArgs, <>mapArgs, <>mapnArgs;
	
	*new {
		^super.new.clear
	}
	
	settingClass {
		^NodeMapSetting
	}
	
	clearArgs {
		setArgs = setnArgs = mapArgs = mapnArgs = nil;
	}
	
	clear {
		settings = IdentityDictionary.new;
	}

	
	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).map(args.at(i+1));
		});
		upToDate = false;
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
		
	}
	
	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).set(args.at(i+1));
		});
		upToDate = false;
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
	}
	
	mapn { arg ... args;
		forBy(0, args.size-1, 3, { arg i;
			this.get(args.at(i)).map(args.at(i+1), args.at(i+2));
		});
		upToDate = false;
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
			if(upToDate.not, {
				this.clearArgs;
				settings.do({ arg item; item.updateNodeMap(this) });
				upToDate = true;
			});
	}
	
	setMsg  { arg nodeID; ^if(setArgs.notNil, { [15, nodeID] ++ setArgs }, { nil }) }
	mapMsg  { arg nodeID; ^if(mapArgs.notNil, { [14, nodeID] ++ mapArgs }, { nil }) }
	setnMsg { arg nodeID; ^if(setnArgs.notNil, { [16, nodeID] ++ setnArgs }, { nil }) }
	mapnMsg  { arg nodeID; ^if(mapArgs.notNil, { [48, nodeID] ++ mapnArgs }, { nil }) }
	
	setToBundle { arg bundle, nodeID; if(setArgs.notNil, { bundle.add([15, nodeID] ++ setArgs) }) }
	setnToBundle { arg bundle, nodeID;if(setnArgs.notNil,{ bundle.add([16, nodeID] ++ setnArgs)}) }
	mapToBundle { arg bundle, nodeID; if(mapArgs.notNil, { bundle.add([14, nodeID] ++ mapArgs) }) }
	mapnToBundle { arg bundle, nodeID; if(mapnArgs.notNil, { bundle.add([48, nodeID] ++ mapnArgs) }) }

	addToBundle { arg inBundle, target;
			var msgs;
			target = target.asNodeID;
			this.updateBundle;
			this.setToBundle(inBundle, target);
			this.setnToBundle(inBundle, target);
			this.mapToBundle(inBundle, target);
			this.mapnToBundle(inBundle, target);
	}
	
	unsetArgs {
		var res;
		if(settings.isEmpty) { ^nil };
		res = Array.newClear(settings.size * 2);
		settings.keys.do { arg item, i; res.put(i * 2, item) };
		^res
	}
	
	unmapArgsToBundle { arg bundle, target, keys;
		var args;
		if(settings.isEmpty) { ^this };
		keys.do { arg key;
			var item;
			item = settings[key];
			if(item.notNil and: {item.isMapped}) {
					args = args ++ [key, -1, item.busNumChannels];
			};
		};
		if(args.notNil) { bundle.add([48, target.asNodeID] ++ args) };
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
				
						
		map { arg ... args; this.prMap(false, args) }
		
		mapn { arg ... args; this.prMap(true, args) }
		
		prMap { arg multiChannel, args;
			var playing;
			playing = proxy.isPlaying;
			args.pairsDo { arg key, mapProxy;
				var ok, setting;
				if(mapProxy.isKindOf(BusPlug).not) { Error("map: not a node proxy").throw };
				ok = mapProxy.initBus(\control);
				if(ok) {
					if(playing, { mapProxy.wakeUp });
					setting = this.get(key);
					setting.map(mapProxy, if(multiChannel) { mapProxy.numChannels }{ 1 });
					parents = parents.put(key, mapProxy);
				}{
					("can only map to control proxy:" + key + mapProxy).inform
				};
			};
			upToDate = false;
		}
		
		mapEnvir { arg keys;
			var args;
			keys = keys ? settings.keys;
			args = Array.new(keys.size*2);
			keys.do({ arg key; args.add(key); args.add(currentEnvironment.at(key)) });
			this.map(args);
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
		}
			
}


