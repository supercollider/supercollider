//used to store and cache set, map setn commands
//todo:  more optimization

NodeMap {
	var <>settings;
	var <bundle, <upToDate=false;
	
	*new {
		^super.new.clear
	}
	
	controlClass {
		^NodeMapSetting
	}
	
	map { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			var key, val;
			key = args.at(i);
			val = args.at(i+1);
			if(upToDate, { this.quickUpdate(key, val, 14) });
			this.at(key).bus_(val);
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
			var key, val;
			key = args.at(i);
			val = args.at(i+1);
			if(upToDate, { this.quickUpdate(key, val, 16) });
			this.at(key).value_(val);
		});
	}
	
	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			var key, val;
			key = args.at(i);
			val = args.at(i+1).asCollection;
			if(upToDate, { this.quickUpdate(key, val, 15) });
			this.at(key).value_(val);
		});
		
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
	
	
	at { arg key;
		var setting;
		setting = settings.at(key);
		if(setting.isNil, { 
			setting = this.controlClass.new(key); 
			settings.put(key, setting) 
		});
		^setting
	}
	
	valueAt { arg key;
		^settings.at(key).value
	}
	
	updateBundle { arg nodeID;
		
		if(upToDate, {
				bundle.do({ arg item;
					item.put(1, nodeID); //the nodeID is always second in a synth message
				});
			}, {
				bundle = List.new; //maybe later for efficiency concatenate.
				settings.do({ arg item; item.addToBundle(bundle, nodeID) });
				upToDate = true;
		});
	}
	
	quickUpdate { arg key, val, command;
				var bundleIndex;
				bundleIndex = bundle.detectIndex({ arg item; 
					item.at(0) == command and: { item.at(2) == key } 
				});
				if(bundleIndex.notNil, { 
					bundle.at(bundleIndex).put(3, val) 
				}, {
					upToDate = false;
				})
	}
	
	addToBundle { arg inBundle, target;
		var mapArgs, valArgs, multiValArgs;
			target = target.asNodeID;
			this.updateBundle(target);
			inBundle.addAll(bundle);
	}
	
	copy {
		var res, nset;
		res = this.class.new;
		nset = res.settings; 
		settings.keysValuesDo({ arg key, val; nset.put(key, val.copy) });
		^res
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
		
		lag { arg args;
			forBy(0, args.size-1, 2, { arg i;
				this.at(args.at(i)).lag_(args.at(i+1));
			});
		}
		unlag { arg args;
			args.do({ arg key;
				var s;
				s = settings.at(key); 
				if(s.notNil, { 
					s.lag_(nil); 
					if(s.isEmpty, { settings.removeAt(key) })
				});
			});
		}
		
		lagsFor { arg keys;
			^keys.collect({ arg key;
				var res;
				res = settings.at(key);
				if(res.notNil, { res.lag }, { nil })
			})
		}
				
		mappingKeys {
			^settings.select({ arg item; item.bus.notNil }).collect({ arg item; item.key })
		}
						
		map { arg ... args;
			var mapArgs, playing;
			mapArgs = [];
			playing = proxy.isPlaying;
			(args.size div: 2).do({ arg i;
				var key, mapProxy, bus, ok;
				key = args.at(i*2).asArray;
				mapProxy = args.at(2*i+1);
				ok = mapProxy.initBus(\control, key.size);
				if(ok, {
					if(playing && mapProxy.isPlaying.not, { mapProxy.wakeUp;  });
					min(key.size, mapProxy.numChannels ? 1).do({ arg chan;
						var theKey;
						theKey = key.at(chan);
						this.at(theKey).bus_(mapProxy).channelOffset_(chan);
						parents = parents.put(theKey, mapProxy);
					});
				}, {
					"rate / numChannels doesn't match".inform
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


