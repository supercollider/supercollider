/*** abstract superclass for 
	TdefGui,PdefGui,
	TdefAllGui, PdefAllGui
	EnvirGui, 

	MonitorGui, 
	NPGui, NdefGui, 
	ProxyMixer (ProxySpaceGui), 
	ParamGui, 
	and possibly others to follow.
	
	common traits: 
	an observed object (a proxy, monitor, space, envir, ...)
	a skipjack for watching it; 
	a getState method for getting all the observed state
	a prevState instvar for keeping the old state around; 
	a checkUpdate method for comparing old and new state, 
		and updating the gui elements that have changed;
	
	parent, minimum bounds for its numItems, actual bounds;
	
	if the object has settings, make an EnvirGui or ParamGui
	ParamGui can do sliders or knobs ...
	
	Simpler ones: EnvirGui, TaskProxyGui;
	
***/ 

JITGui {
	var <object, <numItems, <parent, <bounds, <zone, <minSize, <defPos, <skin, <font; 
	var <prevState = #[], <skipjack, <scroller; 
	var <nameView, <csView;

	*initClass {
		Class.initClassTree(GUI);
		Class.initClassTree(ProxyMixer);
		
		GUI.skins.put(\jit, (
				fontSpecs: 	["Helvetica", 12],
				fontColor: 	Color.black,
				background: 	Color(0.8, 0.85, 0.7, 0.5),
				foreground:	Color.grey(0.95),
				onColor:		Color(0.5, 1, 0.5),
				offColor:		Color.clear,
				gap:			0 @ 0,
				margin: 		2@2,
				buttonHeight:	18,
				headHeight: 	24
			)
		);
	}
	
	accepts { |obj| ^true	}

	object_ { |obj| 
		if (this.accepts(obj)) {
			object = obj;
			this.checkUpdate;
		} { 
			"% : object % not accepted!".format(this.class, obj).warn;
		}
	}
	
	getName { 
		^(this.class.name ++ "_" ++ (try { object.key } ? "anon"))
	}
	
	*new { |object, numItems = 0, parent, bounds, makeSkip = true, extras = #[]| 
		^super.newCopyArgs(object, numItems, parent, bounds).init(makeSkip, extras);
	}
		
	init { |makeSkip, extras|

		skin = GUI.skins.jit;
		font = Font(*skin.fontSpecs);

		// calc bounds - at least minbounds 
		this.setDefaults;
		
		bounds = if (bounds.isNil) { 
			Rect.fromPoints(defPos, defPos + minSize + (skin.margin + skin.margin))
		} { 
			if (parent.isNil and: bounds.isKindOf(Point)) {
				Rect.fromPoints(defPos, defPos + minSize + (skin.margin + skin.margin)) 
			} { 
				bounds.asRect;
			};	
		};
		
		bounds.setExtent(max(bounds.width, minSize.x), max(bounds.height, minSize.y));
		
		if (parent.isNil) { 
			parent = Window(this.getName, bounds.copy.resizeBy(10, 10)).front;
			parent.addFlowLayout;
		};
		
		zone = CompositeView(parent, bounds).background_(Color.white);
		zone.addFlowLayout(skin.margin, skin.gap);
		zone.resize_(2);
		zone.background_(skin.foreground);
		
		
			// then put all the other gui items on it
		this.makeViews;
			// and start watching
		if (makeSkip) { this.makeSkip };
	}
		
	moveTo { |h, v|
		if (parent.isKindOf(Window.implClass)) { 
			parent.bounds = parent.bounds.moveTo(h, v);
		}
	}
	
	close { 
		if (parent.isKindOf(Window.implClass)) { parent.close }
	}
	
	makeSkip { 
		SkipJack(	{ this.checkUpdate }, 0.5, { parent.isNil or: { parent.isClosed } }, this.getName);
	}
	
	
		// these methods should be overridden in subclasses: 

	setDefaults { 
		defPos = 10@260;
		minSize = 250 @ (numItems * skin.buttonHeight + skin.headHeight);
		"minSize: %\n".postf(minSize);
	}

	makeViews { 
		nameView = DragBoth(zone, Rect(0,0, 60, skin.headHeight))
			.font_(font).align_(\center);
			
		csView = EZText(zone, 
			Rect(0,0, bounds.width - 65, skin.buttonHeight * numItems + skin.headHeight), 
			nil, { |ez| object = ez.value; });
		csView.textField.keyDownAction = { |tx, char|
			if (char == Char.tab) { 
				"yo".postln;
				tx.focus_(false)
			}
		}
	}

	getState { 
		// get all the state I need to know of the object I am watching
		^[object] 
	}
	
	checkUpdate { 
		var newState = this.getState;
		// compare newState and prevState, update gui items as needed
		if (newState == prevState) { ^this };
		
		if (newState[0] != prevState[0]) { 
			if (parent.isKindOf(Window.implClass)) { parent.name = this.getName };
			nameView.object_(object).string_(try { object.key } ? "");
			if (csView.textField.hasFocus.not) { 
				csView.value_(object);
			};
		};		
	} 

	makeScroller { 
		// if numItems is exceeded, move sliders by some number... 
		
	}	
}

