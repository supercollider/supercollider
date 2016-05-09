TaskProxyGui : JITGui {

	var <nameBut, <playBut, <pauseBut, <srcBut, <envBut;
	var <envirGui, <usedKeys;

	*observedClass { ^TaskProxy }

	getObjectKey {
		^if (object.respondsTo(\key)) { object.key } { 'anon' };
	}

	setDefaults { |options|
		defPos = 10@260;
		minSize = 260 @ (skin.buttonHeight * (numItems + 1) + (numItems.sign * 4));
		if (parent.notNil) { skin = skin.copy.put(\margin, 0@0) };
		//	"% - minSize: %\n".postf(this.class, minSize);
	}

	makeViews { |options|
		var height = skin.buttonHeight;
		var lineWidth = zone.bounds.width - (skin.margin.y * 3);
		var width = lineWidth * 0.62 / 4;
		var nameWidth = lineWidth * 0.38;
		var zoneMargin = if ( (numItems > 0) or: { parent.isKindOf(Window) }) { skin.margin } { 0@0 };

		zone.decorator = FlowLayout(zone.bounds, zoneMargin, skin.gap);

		nameBut = Button(zone, Rect(0,0, nameWidth, height))
		.font_(font)
		.resize_(2)
		.states_([
			[" ", skin.fontColor, skin.onColor]
		])
		.keyDownAction_({ |btn, char|
			char.postcs;
			if (char.ascii == 127) {
				object.clear;
				object.class.all.removeAt(btn.states.first.first.asSymbol);
				object = nil;
			};
		});

		playBut = Button(zone, Rect(0,0, width, height))
		.font_(font)
		.resize_(3)
		.states_([
			[" >", skin.fontColor, skin.offColor],
			[" _", skin.fontColor, skin.onColor ],
			[" |", skin.fontColor, skin.offColor ]
		])
		.action_({ |but| var string;
			if (object.notNil) {
				if (History.started) {
					// historical action, sets cmdLine and gets recorded.
					string = object.asCompileString
					++ [".play;", ".play;", ".stop;" ][but.value];
					thisProcess.interpreter.cmdLine_(string)
					.interpretPrintCmdLine;
				} {
					// a-historical, but faster
					[ { object.play }, { object.play }, { object.stop } ][but.value].value
				};
				this.checkUpdate;
			};
		});

		pauseBut = Button(zone, Rect(0,0, width, height))
		.font_(font)
		.resize_(3)
		.states_([
			["paus", skin.fontColor, skin.onColor],
			["rsum", skin.fontColor, skin.offColor]
		])
		.action_({ |but| var string;
			if (object.notNil) {
				if (History.started) { 		//	"// historical".postln;
					string = object.asCompileString
					++ [".resume;", ".pause;" ][but.value];
					thisProcess.interpreter.cmdLine_(string)
					.interpretPrintCmdLine;
				} { 						//	"// faster".postln;
					[ { object.resume },{ object.pause } ][but.value].value
				};
				this.checkUpdate;
			};
		});

		srcBut = Button(zone, Rect(0,0, width, height))
		.font_(font)
		.resize_(3)
		.states_([
			["src", skin.fontColor, skin.offColor],
			["src", skin.fontColor, skin.onColor]
		])
		.action_({ |but|
			this.openDoc(this.srcString);
			but.value_(object.hasSource.binaryValue)
		});

		envBut = Button(zone, Rect(0,0, width, height))
		.font_(font)
		.resize_(3)
		.states_([
			["env", skin.fontColor, skin.offColor],
			["env", skin.fontColor, skin.onColor]
		])
		.action_({ |but, mod|
			if (mod.isAlt) {
				this.class.new(object, max(object.envir.size, 8), nil, 400@20);
			} {
				if (object.envir.isNil) {
					this.openDoc(this.editString)
				} {
					this.openDoc(this.editStrings)
				}
			};
			but.value_(object.hasEnvir.binaryValue)
		});

		if (numItems > 0) { this.makeEnvirGui(lineWidth, height) };
		this.checkUpdate;
	}

	makeEnvirGui { |lineWidth, height|
		zone.decorator.nextLine.shift(0, 2);

		envirGui = EnvirGui(
			try { this.object.envir },
			numItems,
			zone,
			Rect(0, 20, lineWidth, numItems * height),
			false
		);
	}

	accepts { |obj| ^obj.isNil or: { obj.isKindOf(PatternProxy) } }

	name_ { |name|
		super.name_(name);
		nameBut.states_(nameBut.states.collect(_.put(0, name.asString))).refresh;
	}

	getState {
		if (object.isNil) {
			^(playState: 0, hasSource: 0, hasEnvir: 0, canPause: 0, isPaused: 0)
		};

		^(
			isPlaying: object.isPlaying, 	// == proxy is playing now or will play
			isActive: object.isActive,		// == really does something right now
			hasSource: object.source.notNil,	// has a source
			hasEnvir: object.envir.notNil, 	// has an envir
			canPause: object.canPause,
			isPaused: object.isPaused
		).collect(_.binaryValue)
		.put(\name, this.getObjectKey)
		.put(\object, object);
	}

	checkUpdate {
		var newState = this.getState;
		var playState;

		// compare newState and prevState, update gui items as needed
		if (newState == prevState) { ^this };

		if (newState[\object].isNil) {
			prevState = newState;
			zone.visible_(false);
			^this;
		};

		zone.visible_(true);

		if (newState[\name] != prevState[\name]) {  // name
			this.name_(newState[\name]);
		};

		playState = newState[\isPlaying] * 2 - newState[\isActive];
		newState.put(\playState, playState);

		if (playState != prevState[\playState]) {
			// stopped/playing/ended
			// 0 is stopped, 1 is active, 2 is playing but waiting:
			playBut.value_(playState).refresh;
		};
		if (newState[\hasSource] != prevState[\hasSource]) {
			srcBut.value_(newState[\hasSource]).refresh;
		};
		if (newState[\hasEnvir] != prevState[\hasEnvir]) {  // has envir
			envBut.value_(newState[\hasEnvir]).refresh;
		};
		if (newState[\canPause] != prevState[\canPause]) {
			pauseBut.visible_(newState[\canPause] > 0).refresh;
		};

		if (newState[\isPaused] != prevState[\isPaused]) {
			pauseBut.value_(newState[\isPaused]).refresh;
		};

		if (envirGui.notNil) {
			if (envirGui.object != object.envir) {
				envirGui.object_(object.envir);
			};
			envirGui.checkUpdate;
		};
		prevState = newState
	}

	clear { object = nil; this.checkUpdate }

	srcString {
		^this.class.observedClass.asString
		++ "(" + this.getObjectKey.asCompileString + ","
		+ object.source.asCompileString + ");\n"
	}

	editString { |edKey|
		var keyText = if (edKey.isNil) {
			"\\anyKey, nil"
		} {
			edKey.asCompileString + ","
			+ object.envir[edKey].asCompileString
		};

		^(this.class.observedClass.asString
			++ "(" + this.getObjectKey.asCompileString
			+ ").set(" + keyText + ");\n"
		)
	}

	editStrings { |edKeys|
		edKeys = edKeys ? this.getUsedKeys;
		^edKeys.collect (this.editString(_))
	}

	getUsedKeys {
		if (object.envir.isNil) { ^[] };
		^usedKeys = object.envir.keys.rejectAs(_ == \self, Array).sort;
	}

	openDoc { |strings, bounds|
		var doc = strings.join.newTextWindow("edit me");
		try { doc.bounds_(bounds ? Rect(0, 400, 400, 200)) };
	}

}

TdefGui : TaskProxyGui {
	*observedClass { ^Tdef }
}

PdefGui : TaskProxyGui {
	*observedClass { ^Pdef }
}

TaskProxyAllGui :JITGui {

	var <filtBut, <filTextV, <edits, <tpGui;
	var <>prefix = "", <>filtering = false;
	var <names, <keysRotation=0;
	var <editZone;

	*observedClass { ^TaskProxy }
	*tpGuiClass { ^TaskProxyGui }

	*observedAll {
		^if (this.observedClass.respondsTo(\all)) {
			this.observedClass.all
		} { nil };
	}

	*new { |numItems = 16, parent, bounds, makeSkip = true, options = #[]|
		^super.new(this.observedAll, numItems, parent, bounds, makeSkip, options );
	}

	// options could include a TdefGui with EnvirGui ...
	makeViews { |options|

		if (parent.isKindOf(Window)) {
			parent.name = this.class.observedClass.name ++ ".all";
		};

		filtBut = Button(zone, Rect(0,0, 80, skin.headHeight))
		.canFocus_(false)
		.states_([
			["all", skin.fontColor, skin.foreground],
			["filt", skin.fontColor, skin.onColor]])
		.action_({ |btn|
			this.filtering_(btn.value > 0);
		});

		filTextV = TextView(zone, Rect(60,0, 80, skin.headHeight))
		.string_("")
		.enterInterpretsSelection_(false)
		.resize_(2)
		.keyDownAction_({ |txvw, char, mod, uni, keycode|
			var str = txvw.string;
			if (str == "") { str = nil };
			this.prefix_(txvw.string);
		});
		filTextV
			.background_(skin.background)
			.stringColor_(skin.fontColor);

		edits = Array.fill(numItems, {
			this.class.tpGuiClass.new(
				numItems: 0,
				parent: zone,
				bounds: Rect(0,0, zone.bounds.width - 16, skin.buttonHeight),
				makeSkip: false
			)
		});

		parent.view.decorator.left_(zone.bounds.right - 12)
		.top_(zone.bounds.top + skin.headHeight);

		scroller = EZScroller(parent,
			Rect(0, 0, 12, numItems * skin.buttonHeight),
			numItems, numItems,
			{ |sc| keysRotation = sc.value.asInteger.max(0) }
		).visible_(false);

		scroller.slider.resize_(3);

		//		if (options.includes(\edit)) {
		//			editZone = CompositeView.new(parent,)
		//			zone.resize_(1);
		//		};
	}

	getState {
		^(object: object);
	}

	checkUpdate {
		var newState = this.getState;
		var overflow, tooMany;

		if (newState[\object] != prevState[\object]) {
			zone.visible_(newState[\object].notNil);
		};

		if (newState[\object].isNil) {
			prevState = newState;
			^this
		};

		if (object.notNil) {
			names = object.keys.as(Array);
			try { names.sort };

			if (filtering) {
				if (prefix == "") {
					names = names.reject { |name| name.asString.includes($_) };
				} {
					names = names.select { |name| name.asString.contains(prefix) };
				};
			};
			overflow = (names.size - numItems).max(0);
			if (overflow > 0) {
				scroller.visible_(true);
				scroller.numItems_(names.size);
				scroller.value_(keysRotation ? overflow);
				names = names.drop(keysRotation).keep(numItems);
			} {
				scroller.visible_(false);
			};
			edits.do { |edit, i| edit.object_(object[names[i]]) };
			if (tpGui.notNil) { tpGui.checkUpdate };
		};

		prevState = newState;
	}
}

PdefnGui : JITGui {

	*observedClass { ^Pdefn }

	accepts { |obj| ^obj.isNil or: { obj.isKindOf(this.class.observedClass) } }

	getState {
		// get all the state I need to know of the object I am watching
		var state = (object: object);
		object !? { state.put(\source, object.source) };
		^state
	}

	checkUpdate {
		var newState = this.getState;

		var show = newState[\object].notNil;
		zone.visible_(show);
		if (show.not) {
			prevState = newState;
			^this
		};

		if (newState[\object] != prevState[\object]) {
			this.name_(this.getName);
		};
		// works with a little delay, but works
		if (newState[\source] != prevState[\source]) {
			defer { csView.textField.string_(object.asCode); };
		};
		prevState = newState;
	}
}

TdefAllGui : TaskProxyAllGui {
	*observedClass { ^Tdef }
	*tpGuiClass { ^TdefGui }

	setDefaults {
		defPos = 10@660;
		minSize = 260 @ (numItems + 1 * 20);
	}
}

PdefAllGui : TaskProxyAllGui {
	*observedClass { ^Pdef }
	*tpGuiClass { ^PdefGui }

	setDefaults {
		defPos = 270@660;
		minSize = 260 @ (numItems + 1 * 20);
	}
}

PdefnAllGui : TaskProxyAllGui {
	*observedClass { ^Pdefn }
	*tpGuiClass { ^PdefnGui }

	setDefaults {
		defPos = 540@660;
		minSize = 260 @ (numItems + 1 * 20);
	}
}
