ProxyNodeMap : NodeMap {

		var <>parents;
		
		clear {
			super.clear;
			parents = IdentityDictionary.new;
		}
		
		wakeUpParentsToBundle { arg bundle;
			parents.do({ arg item; item.wakeUpParentsToBundle(bundle) });
		}
		
		unmapProxy { arg keys;
			keys = keys.asArray;
			if(keys.at(0).isNil, { keys = mappings.keys });
			this.performList(\unmap, keys);
			keys.do({ arg key; 
				parents.removeAt(key);
			});
		}
		
		mapToProxy { arg args;
			var mapArgs;
			mapArgs = [];
		
			(args.size div: 2).do({ arg i;
				var key, proxy, bus, ok;
				key = args.at(i*2).asArray;
				proxy = args.at(2*i+1);
				ok = proxy.initBus(\control, key.size);
			
				if(ok, {
					bus = proxy.outbus;
					min(key.size, bus.numChannels).do({ arg chan;
						var theKey;
						theKey = key.at(chan);
						mapArgs = mapArgs ++ [theKey, bus.index + chan];
						parents = parents.put(theKey, proxy);
					});
				}, {
					"rate / numChannels doesn't match".inform
				});
			});
			this.performList(\map, mapArgs.postln);		
		}
		
		/*
		translate { arg proxy; 
			^proxy.outbus.tryPerform(\index); 				}
		*/
}


