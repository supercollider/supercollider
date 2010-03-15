
TaskProxyGui : JITGui {

	var <nameBut, <playBut, <pauseBut, <srcBut, <envBut;
	var <envirGui, <usedKeys;

	setDefaults { 
		var xMargin = (numItems.sign + parent.isKindOf(Window.implClass).binaryValue) * (2 * skin.margin.y); 
		defPos = 10@260;
		minSize = 250 @ (skin.buttonHeight * (numItems + 1) + xMargin);
	//	"minSize: %\n".postf(minSize);
	}
	
	makeViews {
		var height = skin.buttonHeight;
		var lineWidth = zone.bounds.width - (skin.margin.y * 2);
		var width = lineWidth * 0.62 / 4;
		var nameWidth = lineWidth * 0.38;
		var zoneMargin = if ( (numItems > 0) or: { parent.isKindOf(Window.implClass) }) { skin.margin } { 0@0 };
		
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
					object.clear; object = nil;
					this.class.observedClass.all.removeAt(btn.states.first.first.asSymbol);
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
			.action_({ |but|
				if (object.envir.isNil) {
					this.openDoc(this.editString)
				} {
					this.openDoc(this.editStrings)
				};
				but.value_(object.hasEnvir.binaryValue)
			});

		if (numItems > 0) { this.makeEnvirGui(lineWidth, height) };
	}
	
	makeEnvirGui { |lineWidth, height|
		envirGui = EnvirGui(
			try { this.object.envir }, 
			numItems,
			zone, 
			Rect(0, 20, lineWidth, numItems * height), 
			false
		);
	}

	accepts { |obj| ^obj.isNil or: { obj.isKindOf(this.class.observedClass) } }

	checkUpdate { 
		var newState = this.getState;
		var playState;
		// compare newState and prevState, update gui items as needed
		if (newState == prevState) { ^this };
		
		if (newState[0].isNil) { 
			zone.visible_(false);
			^this; 
		}; 
		
		if (newState[0] != prevState[0]) {  // name
			zone.visible_(true);
			nameBut.states_(nameBut.states.collect(_.put(0, object.key.asString))).refresh;
		};

		if (newState[[1, 2]] != prevState[[1, 2]]) {  
				// stopped/playing/ended
				// 0 is stopped, 1 is active, 2 is playing but waiting:
			playState = newState[1] * 2 - newState[2];
			playBut.value_(playState).refresh;
		};
		if (newState[3] != prevState[3]) {  // has source
			srcBut.value_(newState[3]).refresh;
		};
		if (newState[4] != prevState[4]) {  // has envir
			envBut.value_(newState[4]).refresh;
		};
		if (newState[[5, 6]] != prevState[[5, 6]]) { 
			pauseBut.visible_(newState[5] > 0).value_(newState[6]).refresh; // canPause, isPaused
		};

		
		if (envirGui.notNil) { 
			envirGui.object_(try { object.envir } { nil });
		};
	}
	
	getState { 
		if (object.isNil) { ^[nil, 0, 0, 0, 0, 0, 0] };
		
		^[object.key] ++ [	
			object.isPlaying, 		// == is playing now or will play
			object.isActive,		// == really does something right now
			object.source.notNil,	// has a source
			object.envir.notNil, 	// has an envir
			object.canPause,
			object.isPaused
		].collect(_.binaryValue);
	}

	clear { object = nil; this.checkUpdate }

	srcString {
		^this.class.observedClass.asString
			++ "(" + object.key.asCompileString + ","
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
			++ "(" + object.key.asCompileString
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

	*new { |numItems = 16, parent, bounds, makeSkip = true, makeEnvir = false| 
		
		^super.new(nil, numItems, parent, bounds, makeSkip ); // .init(makeEnvir);
	}

	makeViews { 
		zone.decorator.dump;
		if (parent.isKindOf(Window.implClass)) { 
			parent.name = this.class.observedClass.name ++ ".all";
		};
		
		filtBut = Button(zone, Rect(0,0, 80,20))
			.canFocus_(false)
			.states_([["all"], ["filt"]])
			.action_({ |btn|
				this.filtering_(btn.value > 0);
			});

		filTextV = TextView(zone, Rect(60,0, 80,20))
			.string_("")
			.enterInterpretsSelection_(false)
			.resize_(2)
			.keyDownAction_({ |txvw, char, mod, uni, keycode|
				var str = txvw.string;
				if (str == "") { str = nil };
				this.prefix_(txvw.string);
			});
		
		edits = Array.fill(numItems, { 
				this.class.tpGuiClass.new(
					numItems: 0, 
					parent: zone, 
					bounds: Rect(0,0, zone.bounds.width - 15, skin.buttonHeight), 
					makeSkip: false
				) 
		});
		
		parent.view.decorator.left_(zone.bounds.right - 12).top_(zone.bounds.top + 20);
		
		scroller = EZScroller(parent,
			Rect(0,0, 12, numItems * 16 - 2),
			numItems, numItems,
			{ |sc| keysRotation = sc.value.asInteger.max(0) }
		).visible_(false);
		
		scroller.slider.resize_(3);
		
//		if (makeEdit) { 
//			zone.resize_(1);
//			scroller.slider.resize_(1); 
//			editor = this.class.tpGuiClass.new(nil, numItems: numItems, height: 18, parent: parent.view, makeWatcher: false); 
//		};
	
	}
	
	checkUpdate {
			var overflow, tooMany;

			names = this.class.observedClass.all.keys.as(Array);
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
			edits.do { |edit, i| edit.object_(this.class.observedClass.all[names[i]]) };
			if (tpGui.notNil) { tpGui.checkUpdate };
	}
}

TdefAllGui : TaskProxyAllGui {
	*observedClass { ^Tdef }
	*tpGuiClass { ^TdefGui }

	setDefaults { 
		defPos = 10@660;
		minSize = 250 @ (numItems + 1 * 20);		
	}
}

PdefAllGui : TaskProxyAllGui {
	*observedClass { ^Pdef }
	*tpGuiClass { ^PdefGui }

	setDefaults { 
		defPos = 270@660;
		minSize = 250 @ (numItems + 1 * 20);		
	}
}
