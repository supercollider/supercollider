//used to store and cache set, map setn commands


NodeMap {
	var <>settings;
	var <bundle, <upToDate=false;
	var <>setArgs, <>setnArgs, <>mapArgs, <>mapnArgs;
	
	*new {
		^super.new.clear
	}
	
	controlClass {
		^NodeMapSetting
	}
	
	clearArgs {
		setArgs = setnArgs = mapArgs = mapnArgs = nil;
	}
	
	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).bus_(args.at(i+1));
		});
		upToDate = false;
	}
	
	unmap { arg ... keys;
		keys.do({ arg key;
			var setting;
			setting = settings.at(key);
			if(setting.notNil, {
				setting.bus_(nil);
				if(setting.isEmpty, { settings.removeAt(key) })
			});
		});
		upToDate = false;
		
	}
	
	setn { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).value_(args.at(i+1).asCollection);
		});
		upToDate = false;
	}
	
	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.get(args.at(i)).value_(args.at(i+1));
		});
		upToDate = false;
	}
	
	unset { arg ... keys;
		keys.do({ arg key;
			var s;
			s = settings.at(key);
			if(s.notNil, {
				s.value_(nil);
				if(s.isEmpty, { settings.removeAt(key) })
			})
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
	
	clear {
		settings = IdentityDictionary.new;
	}
	
	
	get { arg key;
		var setting;
		setting = settings.at(key);
		if(setting.isNil, { 
			setting = this.controlClass.new(key); 
			settings.put(key, setting) 
		});
		^setting
	}
	
	at { arg key;
		^settings.at(key)
	}
	
	settingKeys { 
		var res;
		settings.do({ arg item; if(item.value.notNil, { res = res.add(item.key) }) });
		^res
	}
	mappingKeys { 
		var res;
		settings.do({ arg item; if(item.bus.notNil, { res = res.add(item.key) }) });
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
	setToBundle { arg bundle, nodeID; if(setArgs.notNil, { bundle.add([15, nodeID] ++ setArgs) }) }
	setnToBundle { arg bundle, nodeID;if(setnArgs.notNil,{ bundle.add([16, nodeID] ++ setnArgs)}) }
	mapToBundle { arg bundle, nodeID; if(mapArgs.notNil, { bundle.add([14, nodeID] ++ mapArgs) }) }

	addToBundle { arg inBundle, target;
			var msgs;
			target = target.asNodeID;
			this.updateBundle;
			this.setToBundle(inBundle, target);
			this.setnToBundle(inBundle, target);
			this.mapToBundle(inBundle, target);
	}
	unsetArgs {
		var res;
		if(settings.isEmpty) { ^nil };
		res = Array.newClear(settings.size * 2);
		settings.keys.do { arg item, i; res.put(i * 2, item) };
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
		
		clear {
			super.clear;
			parents = IdentityDictionary.new;
		}
		
		controlClass {
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
		}
		
		ratesFor { arg keys;
			^keys.collect({ arg key;
				var res;
				res = settings.at(key);
				if(res.notNil, { res.rate }, { nil })
			})
		}
				
		mappingKeys {
			^settings.select({ arg item; item.bus.notNil })
					.asArray.collect({ arg item; item.key })
		}
						
		map { arg ... args;
			var playing;
			playing = proxy.isPlaying;
			(args.size div: 2).do({ arg i;
				var key, mapProxy, bus, ok;
				key = args.at(i*2).asArray;
				mapProxy = args.at(2*i+1);
				if(mapProxy.isKindOf(BusPlug).not) { Error("map: not a node proxy").throw };
				ok = mapProxy.initBus(\control, key.size);
				if(ok, {
					if(playing, { mapProxy.wakeUp });
					min(key.size, mapProxy.numChannels ? 1).do({ arg chan;
						var theKey;
						theKey = key.at(chan);
						this.get(theKey).bus_(mapProxy).channelOffset_(chan);
						parents = parents.put(theKey, mapProxy);
					});
				}, {
					("rate / numChannels doesn't match:" + key + mapProxy).inform
				});
			});
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
					setting.bus_(nil);
					parents.removeAt(key);
					if(setting.isEmpty, { settings.removeAt(key) })
				});
			});
			upToDate = false;
		}
			
}


