ProxyChain { 

	classvar <allSources;
	 
	var <slotNames, <slotsInUse, <proxy, <sources; 
	
	*initClass { 
		allSources = (); 
	}
	
	*add { |...args| 
		args.pairsDo { |k, v| allSources.put(k, v) }
	}
	
	*from { arg proxy, slotNames = #[];
		^super.new.initProxy(proxy, slotNames)
	}
	
	*new { arg numChannels=2, slotNames = #[], server;
		^super.new.init(numChannels, slotNames, server); 
	}
	
	initProxy { |argProxy, argSlotNames|
		slotNames = Order.new; 
		slotsInUse = Order.new;
		sources = ();
		sources.parent_(allSources);
		
		argSlotNames.do { |name, i| slotNames.put(i + 1 * 10, name) };
		proxy = argProxy;
	}
	
	init { |argNumChans, argSlotNames, server|
		slotNames = Order.new; 
		slotsInUse = Order.new;
		sources = ();
		sources.parent_(allSources);
		
		argSlotNames.do { |name, i| slotNames.put(i + 1 * 10, name) };
		proxy = NodeProxy.audio(server ? Server.default, argNumChans);
	}
		
	add { |key, wet, func| 	// assume the index exists
		var index = slotNames.indexOf(key);
			// only overwrite existing keys so far. 
		if (func.notNil, { this.sources.put(key, func) });
		this.addSlot(key, index, wet);
	}
	
	remove { |key| 
	 	var oldSlotIndex = slotsInUse.indexOf(key);
		if (oldSlotIndex.notNil) { proxy[oldSlotIndex] = nil; };
		slotsInUse.remove(key);
	}
	
	addSlot { |key, index, wet| 
	
		var func = sources[key];
		var prefix, prevVal, specialKey;
		if (func.isNil) { "ProxyChain: no func called \%.\n".postf(key, index); ^this };
		if (index.isNil) { "ProxyChain: index was nil.".postln; ^this };

		this.remove(key); 
		slotsInUse.put(index, key);

		if (func.isKindOf(Association)) { 
			prefix = (filter: "wet", mix: "mix", filterIn: "wet")[func.key];
			specialKey = (prefix ++ index).asSymbol;
			prevVal = proxy.nodeMap.get(specialKey).value;
			if (wet.isNil) { wet = prevVal ? 0 };
			proxy.set(specialKey, wet);
		};
		proxy[index] = func; 
	}


		// forward basic messages to the proxy 
	play { arg out, numChannels, group, multi=false, vol, fadeTime, addAction;  
		proxy.play(out, numChannels, group, multi=false, vol, fadeTime, addAction)
	}
	
	playN { arg outs, amps, ins, vol, fadeTime, group, addAction;
		proxy.playN(outs, amps, ins, vol, fadeTime, group, addAction);
	}
	
	stop { arg fadeTime, reset=false;
		proxy.stop(fadeTime, reset);
	}
	
	end { arg fadeTime, reset=false;
		proxy.end(fadeTime, reset);
	}
	

		// JIT gui support	
	gui { 	|name, buttonList, nSliders=16, win| 
		^ProxyChainGui(this, name, buttonList, nSliders, win) 
	}
	
	informEditor { |ed| 
			// this should be more specific to be more efficient.
		slotNames.do { |name, i| ed.replaceKeys.put(("wet" ++ i).asSymbol, name) };
		slotNames.do { |name, i| ed.replaceKeys.put(("mix" ++ i).asSymbol, name) };
	}
	
	makeEdit { |name, nSliders=24, win, comp| 
		var ed = NodeProxyEditor(proxy, nSliders, win, comp); 
		this.informEditor(ed);
		if (win.isNil) { ed.w.name_ ( "Edit" + name) };
		^ed
	}
}