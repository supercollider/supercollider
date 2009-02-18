ProxyChainGui { 
	var <guiFuncs; 
	var w, <butComp, <butLay, <buttons, <slotButs, <chain, <editor; 
		
	*new { |chain, name=\proxyChain, buttonList, nSliders=16, win| 
		buttonList = buttonList ?? { chain.slotNames.collect ([_, \slotCtl]) };
		^super.new.init(name, buttonList, nSliders, win).chain_(chain)
	}
	
	chain_ { |inchain| 
		try { 
			editor.proxy_(inchain.proxy);
			chain = inchain;
		} { 
			"ProxyChainGui: chain not changed.".inform; 
		};
			// check and update buttons here... 
	}
	buttonList_ { |buttonList|
		buttonList.do { |list, i| 
			var name, kind, but, func, setup; 
			#name, kind, func, setup = list;
			but = buttons[i];
			if (but.notNil) { 
				kind = kind ? \slotCtl; 
				if (name.notNil) {
					guiFuncs[kind].value(but, name, func);
					setup.value(this, but);
				};
			} { 
				"ProxyChainGui: out of buttons...".postln;
			}
		};
	}
	
	init { |name, buttonList, nSliders, parent|
			
		var butComp, butLay, bounds, editcomp;

		guiFuncs =  (			
			btlabel: { |but, name| but.states_([[name, Color.black, Color(1, 0.5, 0)]]) },
			label: { |but, name| but.states_([[name, Color.white, Color(1, 0.5, 0)]]) },
			slotCtl: { | but, name, level=0| 
				but.states_([["[" + name + "]"], [name, Color.black, Color.green(5/7)], ]);
				but.action_({ |but| 
					[ { chain.remove(name) }, { chain.add(name, level) } ][but.value].value 
				});
			}, 
			
			extra: { |but, name, func| 
				but.states_([[name, Color.black, Color(1, 0.7, 0)]]);
				but.action_(func);
			}
		);
		
		bounds =  Rect(200, 200, 160 + 355, 
			(buttonList.size + 1 * 24).max(nSliders + 3 * GUI.skin.buttonHeight)
		); 
		if (parent.isNil) { 
			parent = Window(name, bounds, false).front; 
			parent.asView.addFlowLayout;
		};
		
		butComp = CompositeView(parent, Rect(0,0, 150, bounds.height));
		butComp.addFlowLayout;
		butLay = butComp.decorator;
		
		buttons = buttonList.collect { Button(butComp, Rect(0,0, 140, 20)) };
		
		this.buttonList_(buttonList);
		
		editcomp = CompositeView(parent.asView, Rect(155, 0, 350, bounds.height));

		editor = NodeProxyEditor(nil, nSliders, parent, editcomp); 
	}
	
}