/***
	Abstract superclass for
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
	var <nameView, <csView;
	var <prevState, <skipjack, <scroller;
	var <config, <hasWindow = false;

	*initClass {
		Class.initClassTree(GUI);

		GUI.skins.put(\jit, (
				fontSpecs: 	["Helvetica", 12],
				fontColor: 	Color.black,
				background: 	Color(0.8, 0.85, 0.7, 0.5),
				foreground:	Color.grey(0.95),
				onColor:		Color(0.5, 1, 0.5),
				offColor:		Color.clear,
				hiliteColor:	Color.green(1.0, 0.5),
				gap:			0 @ 0,
				margin: 		2@2,
				buttonHeight:	18,
				headHeight: 	24

			)
		);
	}

	*new { |object, numItems = (0), parent, bounds, makeSkip = true, options = #[]|
		^super.newCopyArgs(nil, numItems, parent, bounds)
			.init(makeSkip, options)
			.object_(object);
	}

	init { |makeSkip, options|

		skin = GUI.skins.jit;
		font = Font(*skin.fontSpecs);
		prevState = ();

		// calc bounds - at least minbounds
		this.setDefaults(options);
		this.calcBounds(options);

		if (parent.isNil) { this.makeWindow };
		this.makeZone;

			// then put all the other gui items on it
		this.makeViews(options);
			// and start watching
		if (makeSkip) { this.makeSkip };
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

	name_ { |name|
		if (hasWindow) { parent.name_(this.winName(name)) };
		if (nameView.notNil) {
			try { nameView.object_(object) };
			nameView.string_(name);
		};
	}
	
	hasName {
		^nameView.notNil and: { nameView.string.notNil }
	}

	getName {	^try { object.key } ? "_anon_" }
	winName { |name| ^this.class.name ++ $_ ++ (name ?? { this.getName }) }

	calcBounds {
		var defBounds;
		if(bounds.isKindOf(Rect)) {
			bounds.setExtent(max(bounds.width, minSize.x), max(bounds.height, minSize.y));
			^this
		};

		defBounds = Rect.fromPoints(defPos, defPos + minSize + (skin.margin + skin.margin));
		if (bounds.isNil) {
			bounds = defBounds;
			^this
		};

		if (bounds.isKindOf(Point)) {
			bounds = defBounds.setExtent(max(bounds.x, minSize.x), max(bounds.y, minSize.y));
		}
	}

	makeWindow {
		parent = Window(this.winName, bounds.copy.resizeBy(10, 10)).front;
		parent.addFlowLayout;
		hasWindow = true;
	}

	makeZone {
		zone = CompositeView(parent, bounds).background_(Color.white);
		zone.addFlowLayout(skin.margin, skin.gap);
		zone.resize_(2);
		zone.background_(skin.foreground);
	}

	moveTo { |h, v|
		if (hasWindow) { parent.bounds = parent.bounds.moveTo(h, v); }
	}

	close {
		if (hasWindow) { parent.close }
	}

	makeSkip {
		skipjack = SkipJack({
		//	try {
				this.checkUpdate
		//	} {
		//		(this.getName  + "checkUpdate failed.").postln;
		//		}
			},
			0.5,
			{ parent.isNil or: { parent.isClosed } },
			this.getName
		);
	}


		// these methods should be overridden in subclasses:
	setDefaults { |options|
		if (parent.isNil) {
			defPos = 10@260
		} {
			defPos = skin.margin;
		};
		minSize = 250 @ (numItems * skin.buttonHeight + skin.headHeight);
	//	"minSize: %\n".postf(minSize);
	}

	makeViews {
		var lineheight = max(
			skin.buttonHeight * numItems + skin.headHeight,
			zone.bounds.height)  - (skin.margin.y * 2);

		nameView = DragBoth(zone, Rect(0,0, 60, lineheight))
			.font_(font)
			.align_(\center)
			.receiveDragHandler_({ arg obj; this.object = View.currentDrag });

		csView = EZText(zone,
			Rect(0,0, bounds.width - 65, lineheight),
			nil, { |ez| object = ez.value; })
			.font_(font);
		csView.bounds.postln;
	}

	getState {
		// get all the state I need to know of the object I am watching
		^(object: object)
	}

	checkUpdate {
		var newState = this.getState;

		// compare newState and prevState, update gui items as needed
		if (newState == prevState) { ^this };

		if (newState[\object] != prevState[\object]) {
			this.name_(this.getName);
			if (csView.textField.hasFocus.not) { csView.value_(object) };
		};
		prevState = newState;
	}

	makeScroller {
		// if numItems is exceeded, shift items along by some number with an EZScroller
	}
}
