Inspector {
	classvar <allInspectors;

	var <object, <window, vpos=0, gui;

	*new { arg object;
		var inspector;
		inspector = this.inspectorFor(object) ?? {
			super.newCopyArgs(object).init;
		};
		inspector.window.front;
		^inspector
	}
	*initClass { allInspectors = []; }
	*inspectorFor { arg object;
		^allInspectors.detect({ arg item;
				item.object === object
		});
	}
	init {
		allInspectors = allInspectors.add(this);
		gui = GUI.current;
		this.makeWindow;
		vpos = 4;
		this.makeHead;
		this.makeBody;
		window.refresh;
	}

	didClose {
		allInspectors.remove(this);
	}
	lineHeight { ^20 }
	buttonHeight { ^this.lineHeight - 4 }
	makeWindow {
		var bounds;
		bounds = Rect(80, 80, 376, this.lineHeight * (this.numLines + 1) + 16);

		window = gui.window.new(object.class.name.asString ++ " inspector", bounds);
		window.onClose = Message(this, \didClose);
	}
	//numLines{^0}
	//makeHead{}
	//makeBody {}
}


ObjectInspector : Inspector {

	var stringView, slotInspectors;

	numLines {
		^min(30, object.slotSize); // don't display too many lines
	}

	makeHead {
		var view;
		view = gui.button.new(window, Rect(8, vpos, 128, this.buttonHeight));
		view.states = [[object.class.name]];
		view.action = { GUI.use( gui, { object.class.inspect })};

		if (object.mutable, {
			view = gui.button.new(window, Rect(140, vpos, 50, this.buttonHeight));
			view.states = [["update"]];
			view.action = Message(this, \update);
		});

		view = gui.dragSource.new(window, Rect(194, vpos, 174, this.buttonHeight));
		view.object = object;
		view.resize = 2;
		stringView = view;

		vpos = vpos + this.lineHeight;
	}

	makeBody {
		this.numLines.do({ arg i;
			slotInspectors = slotInspectors.add(
				SlotInspector(this, i, vpos)
			);
			vpos = vpos + this.lineHeight;
		});
		// ... link to rest, or scroll view
	}

	update {
		stringView.object = object;
		slotInspectors.do({ arg slotinsp;
			slotinsp.update;
		});
	}
}

StringInspector : ObjectInspector {
	// don't bother with the array of Chars.
	numLines { ^0 }
}

ClassInspector : ObjectInspector {
	makeHead {
		var view;
		view = gui.button.new(window, Rect(8, vpos, 128, this.buttonHeight));
		view.states = [[object.class.name]];
		view.action = { GUI.use( gui, { object.class.inspect })};

		view = gui.button.new(window, Rect(140, vpos, 50, this.buttonHeight));
		view.states = [["edit"]];
		view.action = Message(object, \openCodeFile);

		if (object.superclass.notNil, {
			view = gui.button.new(window, Rect(194, vpos, 70, this.buttonHeight));
			view.states = [["superclass"]];
			view.action = { GUI.use( gui, { object.superclass.inspect })};
		});

		view = gui.dragSource.new(window, Rect(268, vpos, 96, this.buttonHeight));
		view.object = object;
		view.resize = 2;
		stringView = view;

		vpos = vpos + this.lineHeight;
	}

}

FunctionDefInspector : ObjectInspector {
	openSuper {
		GUI.use( gui, { object.superclass.inspect });
	}
	makeHead {
		var view;
		view = gui.button.new(window, Rect(8, vpos, 128, this.buttonHeight));
		view.states = [[object.class.name]];
		view.action = { GUI.use( gui, { object.class.inspect })};

		if (object.code.notNil, {
			view = gui.button.new(window, Rect(194, vpos, 70, this.buttonHeight));
			view.states = [["dump code"]];
			view.action = Message(object, \dumpByteCodes);
		});

		view = gui.dragSource.new(window, Rect(268, vpos, 96, this.buttonHeight));
		view.object = object;
		view.resize = 2;
		stringView = view;

		vpos = vpos + this.lineHeight;
	}
}

MethodInspector : ObjectInspector {
	openSuper {
		GUI.use( gui, { object.superclass.inspect });
	}
	makeHead {
		var view;
		view = gui.button.new(window, Rect(8, vpos, 128, this.buttonHeight));
		view.states = [[object.class.name]];
		view.action = { GUI.use( gui, { object.class.inspect })};

		view = gui.button.new(window, Rect(140, vpos, 50, this.buttonHeight));
		view.states = [["edit"]];
		view.action = Message(object, \openCodeFile);

		if (object.code.notNil, {
			view = gui.button.new(window, Rect(194, vpos, 70, this.buttonHeight));
			view.states = [["dump code"]];
			view.action = Message(object, \dumpByteCodes);
		});

		view = gui.dragSource.new(window, Rect(268, vpos, 96, this.buttonHeight));
		view.object = object;
		view.resize = 2;
		stringView = view;

		vpos = vpos + this.lineHeight;
	}
}

SlotInspector {
	var <object, <>index, <key, <slotKeyView, <slotValueView, <inspectButton;

	var gui;

	*new { arg inspector, index, vpos;
		^super.newCopyArgs(inspector.object, index).init(inspector, vpos)
	}
	init { arg inspector, vpos;
		var w, class, hasGetter, hasSetter, vbounds, value;

		gui	= GUI.current;

		w = inspector.window;
		key = object.slotKey(index);
		class = object.class;

		slotKeyView = gui.staticText.new(w, Rect(8, vpos, 110, this.buttonHeight));
		slotKeyView.align = \right;
//		slotKeyView.font = gui.font.default ?? { gui.font.new("Helvetica", 12) };

		if (key.isKindOf(Symbol), {
			hasGetter = class.findRespondingMethodFor(key).notNil;
			hasSetter = class.findRespondingMethodFor(key.asSetter).notNil && object.mutable;
		},{
			hasGetter = true;
			hasSetter = object.mutable;
		});
		//slotKeyView.background = Color.grey(if(hasGetter,0.95,0.85));

		vbounds = Rect(122, vpos, 218, this.buttonHeight);
		if (hasSetter, {
			if (hasGetter, {
				slotValueView = gui.dragBoth.new(w, vbounds);
			},{
				slotValueView =  gui.dragSink.new(w, vbounds);
			});
			slotValueView.action = Message(this, \setSlot);
		},{
			if (hasGetter, {
				slotValueView = gui.dragSource.new(w, vbounds);
			},{
				slotValueView = gui.staticText.new(w, vbounds);
			});
		});
		slotValueView.resize = 2;
//		slotValueView.font = gui.font.default ?? { gui.font.new("Helvetica", 12) };
		//slotValueView.background = Color.grey(if(hasSetter,0.95,0.85));

		inspectButton = gui.button.new(w, Rect(344, vpos, this.buttonHeight, this.buttonHeight));
		inspectButton.states = [["I"]];
		inspectButton.action = Message(this, \inspectSlot);
		inspectButton.resize = 3;
		//inspectButton.visible = true; //object.slotAt(index).canInspect;
		this.update;
	}
	update {
		key = object.slotKey(index);
		slotKeyView.string = key;
		slotValueView.object = object.slotAt(index);
		//inspectButton.visible = true; //object.slotAt(index).canInspect;
	}
	inspectSlot {
		GUI.use( gui, { object.slotAt(index).inspect });
	}
	setSlot {
		if (key.isKindOf(Symbol), {
			object.perform(key.asSetter, gui.view.currentDrag);
		},{
			object.put(key, gui.view.currentDrag);
		});
		this.update;
	}
	lineHeight { ^20 }
	buttonHeight { ^this.lineHeight - 4 }
}

FrameInspector : Inspector {

	numLines{^0}
	makeHead {
		var view;
		view = gui.button.new(window, Rect(8, vpos, 128, this.buttonHeight));
		view.states = [[object.class.name]];
		view.action = { GUI.use( gui, { object.class.inspect })};

		vpos = vpos + this.lineHeight;
	}
	makeBody {}

}
