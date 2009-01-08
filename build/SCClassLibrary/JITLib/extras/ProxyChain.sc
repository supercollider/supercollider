ProxyChain { 

	classvar <allSources;
	 
	var <slotNames, <slotsInUse, <px, <ed, <bw, <sources; 
	
	*initClass { 
		allSources = (); 
	}
	
	*add { |...args| 
		args.pairsDo { |k, v| allSources.put(k, v) }
	}
	
	*from { arg proxy, slotNames = #[];
		^super.new.initPx(proxy, slotNames)
	}
	
	*new { arg numChannels=2, slotNames = #[], server;
		^super.new.init(numChannels, slotNames, server); 
	}
	
	initPx { |argProxy, argSlotNames|
		slotNames = Order.new; 
		slotsInUse = Order.new;
		sources = ();
		sources.parent_(allSources);
		
		argSlotNames.do { |name, i| slotNames.put(i + 1 * 10, name) };
		px = argProxy;
	}
	
	init { |argNumChans, argSlotNames, server|
		slotNames = Order.new; 
		slotsInUse = Order.new;
		sources = ();
		sources.parent_(allSources);
		
		argSlotNames.do { |name, i| slotNames.put(i + 1 * 10, name) };
		px = NodeProxy.audio(server ? Server.default, argNumChans);
	}
		
	add { |key, wet, func| 	// assume the index exists
		var index = slotNames.indexOf(key);
			// only overwrite existing keys so far. 
		if (func.notNil, { this.sources.put(key, func) });
		this.addSlot(key, index, wet);
	}
	
	remove { |key| 
	 	var oldSlotIndex = slotsInUse.indexOf(key);
		if (oldSlotIndex.notNil) { px[oldSlotIndex] = nil; };
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
			prevVal = px.nodeMap.get(specialKey).value;
			if (wet.isNil) { wet = prevVal ? 0 };
			px.set(specialKey, wet);
		};
		px[index] = func; 
	}
	
	gui { 	|name, buttonList, nSliders=16, win| 
		^ProxyChainGui(this, name, buttonList, nSliders, win) 
	}
	
	butWin {	|name, buttonList, nSliders=16| 
		
		var w, butcomp, butdeco, funx, bounds, editcomp;
		if (bw.notNil) { try { bw.close } };
		
		bounds =  Rect(200, 200, 160 + 385, 
			(buttonList.size + 1 * 24).max(nSliders + 3 * GUI.skin.buttonHeight)
		); 
		
		w = GUI.window.new(name ? "ProxyChain", bounds, false).front; 

	 	funx = (			
			btlabel: { |but, name| but.states_([[name, Color.black, Color(1, 0.5, 0)]]) },
			label: { |v, name| v.string_(name.asString).background_(Color(1, 0.5, 0)).align_(0) },
			switch: { | but, name| 
				but.states_([["[" + name + "]"], [name, Color.black, Color.green(5/7)], ]);
				but.action_({ |but| 
					[ { this.remove(name) }, { this.add(name) } ][but.value].value 
				});
				if (slotsInUse.includes(name)) { but.value_(1) };
			}, 
			
			extra: { |but, name, func| 
				but.states_([[name, Color.black, Color(1, 0.7, 0)]]);
				but.action_(func);
			}
		);
		
		w.view.decorator = FlowLayout(w.bounds.moveTo(0,0));
		
		butcomp = GUI.compositeView.new(w, Rect(0,0, 150, bounds.height));
		
		butcomp.decorator = butdeco = FlowLayout(butcomp.bounds);
		
		buttonList.do { |list, i| var name, kind, but, func; 
			#name, kind, func = list;
			if (kind == \label) { 
				if(i > 0) { butdeco.shift(0, 10) };
				but = GUI.staticText.new(butcomp, Rect(0,0, 140, 20));
			} {
				but = GUI.button.new(butcomp, Rect(0,0, 140, 20));
			};
			funx[kind].value(but, name, func);
			butdeco.nextLine;
		};
		
		bw = w;
		bw.onClose = { bw = nil };
		
		editcomp = GUI.compositeView.new(w, Rect(155, 0, 380, bounds.height));

		this.makeEdit(name, nSliders, bw, editcomp);
		^bw
	}
	
	makeEdit { |name, nSliders=24, win, comp| 
	
		if (win.notNil) { try { ed.w.close } };
		
		ed = NodeProxyEditor(px, nSliders, win, comp).pxKey_(name); 
			// this should be more specific to be efficient.
		slotNames.do { |name, i| ed.replaceKeys.put(("wet" ++ i).asSymbol, name) };
		slotNames.do { |name, i| ed.replaceKeys.put(("mix" ++ i).asSymbol, name) };
		
		if (win.isNil) { ed.w.name_ ( "Edit" + name) };

		^ed
	}
}