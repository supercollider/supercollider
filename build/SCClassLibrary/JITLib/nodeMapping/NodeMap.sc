

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
			this.at(args.at(i)).bus_(args.at(i+1));
		});
		upToDate = false;
	}
	
	unmap { arg ... keys;
		keys.do({ arg key;
			var setting;
			setting = this.at(key);
			if(setting.value.isNil, { 
				settings.removeAt(key) 
			}, {
				setting.bus_(nil);
			});
		});
		upToDate = false;
		
	}
	
	setn { arg ... args;
		this.performList(\set, args);
		upToDate = false;
		
	}

	set { arg ... args;
		forBy(0, args.size-1, 2, { arg i;
			this.at(args.at(i)).value_(args.at(i+1));
		});
		upToDate = false;
	}
	
	unset { arg ... keys;
		keys.do({ arg key;
			this.at(key).value_(nil);
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
	
	updateBundle { arg nodeID;
		var newBundle;
		if(upToDate, {
				bundle.do({ arg item;
					item.put(1, nodeID); //the nodeID is always second in a synth message
				});
			}, {
				bundle = List.new;
				settings.do({ arg item; item.addToBundle(bundle, nodeID) });
				upToDate = true;
		});
	}
	
	addToBundle { arg inBundle, target;
		var mapArgs, valArgs, multiValArgs;
			target = target.asNodeID;
			this.updateBundle(target);
			inBundle.addAll(bundle);
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
		
		unmapProxy { arg keys;
			keys = keys.asArray;
			if(keys.at(0).isNil, { keys = this.mappingKeys });
			this.performList(\unmap, keys);
			keys.do({ arg key; 
				parents.removeAt(key);
			});
		}
		
		mappingKeys {
			^settings.select({ arg item; item.bus.notNil }).collect({ arg item; item.key })
		}
						
		map { arg args;
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
	
}


