
TaskProxyEditor {

	var <>proxy, <>nVars, <win, <zone, <flow, <skipjack, <usesPlayN; 
	var <nameBut, <playBut, <pauseBut, <srcBut, <envBut, <varButs;
	var <usedKeys, <oldStates = #[-1], oldProxy=0;
	
	*new { |px, nVars=0, height=16, width = 30, w, makeWatcher=true| 
		^super.new.proxy_(px).nVars_(nVars)
			.init(w, height, width, makeWatcher); 
	}
	
	srcString {
		^this.observedClass.asString 
			++ "(" + proxy.key.asCompileString + "," 
			+ proxy.source.asCompileString + ");\n"
	}

	editString { |edKey| 
		var keyText = if (edKey.isNil) { 
			"\\anyKey, nil"
		} { 
			edKey.asCompileString + "," 
			+ proxy.envir[edKey].asCompileString 
		};
		
		^(this.observedClass.asString 
			++ "(" + proxy.key.asCompileString 
			+ ").set(" + keyText + ");\n"
		)
	}

	editStrings { |edKeys| 
		edKeys = edKeys ? this.getUsedKeys;
		^edKeys.collect (this.editString(_))
	}

	getUsedKeys { 
		if (proxy.envir.isNil) { ^[] };
		^usedKeys = proxy.envir.keys.rejectAs(_ == \self, Array);
	}

	openDoc { |strings, bounds|
		Document("edit me", strings.join ).bounds_(bounds ? Rect(0, 400, 400, 200));
	}

	init { |w, height, width, makeWatcher| 
		var skin = GUI.skins.jit; 
		var font = Font(*skin.fontSpecs);

		win = w ?? { Window("task px edit", Rect(0, 0, 190, 30)) };
		zone = CompositeView(win, Rect(0, 0, 190, height)); 
		zone.resize_(5);
		zone.background_(skin.foreground);
		
		flow = FlowLayout(zone.bounds, 0@0, skin.gap);
		zone.decorator = flow; 
		try { win.front };

		nameBut = Button(zone, Rect(0,0, 70, height))
			.font_(font)
			.states_([
				[" ", skin.fontColor, skin.onColor]
			])
		//	.action_({ "edit all".postln })
			.keyDownAction_({ |btn, char| char.postcs; 
				if (char.ascii == 127) { 
					proxy.clear; proxy = nil;
					this.observedClass.all.removeAt(btn.states.first.first.asSymbol);
				}; 
			})
			.resize_(2)
		;

		playBut = Button(zone, Rect(0,0, width, height))
			.font_(font)
			.resize_(3)
			.states_([
				[" >", skin.fontColor, skin.offColor], 
				[" _", skin.fontColor, skin.onColor ],
				[" |", skin.fontColor, skin.offColor ]
			]);
			
		playBut.action_({ |but| var string;
				if (proxy.notNil) { 	
					if (History.started) { 
								// historical action, sets cmdLine and gets recorded.
						string = proxy.asCompileString 
							++ [".play;", ".play;", ".stop;" ][but.value];
						thisProcess.interpreter.cmdLine_(string)
							.interpretPrintCmdLine; 
					} {
					 			// a-historical, but faster
						[ { proxy.play }, { proxy.play }, { proxy.stop } ][but.value].value 
					};
					this.updateAll;
				};
			});

		pauseBut = Button(zone, Rect(0,0, width, height))
			.font_(font)
			.resize_(3)
			.states_([
				["paus", skin.fontColor, skin.onColor], 
				["rsum", skin.fontColor, skin.offColor]
			]);
						
					
		pauseBut.action_({ |but| var string; 
				if (proxy.notNil) { 
					if (History.started) { 		//	"// historical".postln;
						string = proxy.asCompileString 
							++ [".resume;", ".pause;" ][but.value];
						thisProcess.interpreter.cmdLine_(string)
							.interpretPrintCmdLine; 
					} { 						//	"// faster".postln;
						[ { proxy.resume }, { proxy.pause } ][but.value].value 
					};
						this.updateAll;
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
				but.value_(proxy.hasSource.binaryValue)
			});

		envBut = Button(zone, Rect(0,0, width, height))
			.font_(font)
			.resize_(3)
			.states_([ 
				["env", skin.fontColor, skin.offColor],
				["env", skin.fontColor, skin.onColor]
			])
			.action_({ |but|
				if (proxy.envir.isNil) { 
					this.openDoc(this.editString)
				} { 
					this.openDoc(this.editStrings)
				};
				but.value_(proxy.hasEnvir.binaryValue)
			});

		this.updateAll;

		if (makeWatcher) { this.makeWatcher };
	}

	makeWatcher { 
		skipjack.stop;
		skipjack = SkipJack({ this.updateAll }, 
			0.5, 
			{ win.isClosed },
			this.observedClass.name.asString + try { proxy.key } { "blank" };
		);
		skipjack.start;
	}
	updateAll { 
		var newState, guiStates;
		
		if (win.isClosed) {skipjack.stop; ^this };
		
			// blank out controls if no proxy there.
		if (proxy.isNil) { 
			if (oldProxy.isNil) { ^this };	// do nothing if there was none.
		//	"blank em out".postln;
			[ nameBut, playBut, pauseBut, srcBut, envBut ].flat.do (_.visible_(false));
			win.refresh;
			
			oldStates = [-1];
			oldProxy = proxy; 
			
			^this;
		}; 
			// we have a proxy, and check if its state has changed 
		if (oldProxy.isNil) { 
			[ nameBut, playBut, srcBut, envBut ].flat.do (_.visible_(true));
		};
		
			// "got a proxy".postln;

		if (proxy != oldProxy) { 
			nameBut.states_(nameBut.states.collect(_.put(0, proxy.key.asString)));
			nameBut.visible_(true).refresh;
		};
				
				// get the new State of the proxy: 
		newState = [ 
			proxy.isPlaying, 		// == isplaying or will play
			proxy.isActive,		// == really does something right now
			proxy.source.notNil,	// has a source
			proxy.envir.notNil, 	// has an envir
			proxy.canPause, 
			proxy.isPaused
		].collect(_.binaryValue);
		
	//	[\newState, newState].postln; 
		
		guiStates = [ 
				// 0 is stopped, 
				// 1 is active, 
				// 2 is playing but waiting: 
			newState[0] * 2 - newState[1], 
			newState[2], 
			newState[3],
			newState[4],
			newState[5]			
		];
	//	[\guiStates, guiStates].postln; 
	//	[\oldStates, oldStates].postln; 
	//	"".postln; 
		
		if (guiStates[0] != oldStates[0]) { 
			playBut.value_(guiStates[0]).refresh;
			pauseBut
				.visible_(guiStates[3] > 0) // canPause, i.e. either active or paused, or stopped from cmd-.
				.value_(guiStates[4]) 		// isPaused
				.refresh;
		};
		
		if (guiStates[1] != oldStates[1]) { 
		//	"src changed: ".post; [guiStates[1], oldStates[1]].postln;
			srcBut.value_(guiStates[1]).refresh;
		};
		if (guiStates[2] != oldStates[2]) { 
		//	"env changed: ".post; [guiStates[1], oldStates [1]].postln;
			envBut.value_(guiStates[2]).refresh;
		};

		oldStates = guiStates;	// keep for next check
		oldProxy = proxy; 
	}
	
	clear { proxy = nil }
}

TdefEditor : TaskProxyEditor { 
	observedClass { ^Tdef }
}

PdefEditor : TaskProxyEditor { 
	observedClass { ^Pdef }
}

TaskProxyAllGui { 
	var <win, <filtBut, <filTextV, <edits, <scrolly, <names, <keysRotation=0; 
	var <>prefix="", <>filtering=false; 
	
	*new { |size=24, w| 
		^super.new.init(size, w);
	}
	
	init { | size, w|
		var zone, flow;
		var name = "" ++ this.observedClass ++ "AllGui";
		win = w ?? { Window(name, Rect(0, 0, 210, 16 * size + 5)) };
		zone = CompositeView(win, Rect(0, 0, 210, 16 * size + 5)); 
//		zone.background_(skin.foreground);
		zone.resize_(2);
		
		flow = FlowLayout(zone.bounds, 0@0, 0@0);
		zone.decorator = flow; 
		try { win.front };
		
		filtBut = Button(zone, Rect(0,0,60,20))
			.canFocus_(false)
			.states_([["all"], ["filt"]])
			.action_({ |btn| 
				this.filtering_(btn.value > 0);
			});
					
		filTextV = TextView(zone, Rect(60,0,60,20))
			.string_("")
			.enterInterpretsSelection_(false)
			.resize_(2)
			.keyDownAction_({ |txvw, char, mod, uni, keycode| 
				var str = txvw.string;
				if (str == "") { str = nil };
				this.prefix_(txvw.string);
			});
			
		edits = Array.fill(size, { this.editorClass.new(w: zone, makeWatcher: false) }); 

		scrolly = EZScroller(win, 
			Rect(flow.bounds.right - 16, 2, 12, size * 16), 
			size, size, 
			{ |sc| keysRotation = sc.value.asInteger.max(0); 
			
			}
		).visible_(false);
		scrolly.slider
			.resize_(3);

		SkipJack({ 
			var overflow, tooMany; 

			names = this.observedClass.all.keys.as(Array);
			try { names.sort };
			if (filtering) {
				if (prefix == "") { 
					names = names.reject { |name| name.asString.includes($_) };
				} { 
					names = names.select { |name| name.asString.contains(prefix) };
				};	
			};
			overflow = (names.size - size).max(0); 
			if (overflow > 0) {
				scrolly.visible_(true);
				scrolly.numItems_(names.size);
				scrolly.value_(keysRotation ? overflow);
				names = names.drop(keysRotation).keep(size);
			} { 
				scrolly.visible_(false);
			};
			edits.do { |edit, i| edit.proxy_(this.observedClass.all[names[i]]).updateAll };

		}, 0.5, { win.isClosed }, name);
	}
}

TdefAllGui : TaskProxyAllGui { 
	observedClass { ^Tdef }
	editorClass { ^TdefEditor }
}
PdefAllGui : TaskProxyAllGui { 
	observedClass { ^Pdef }
	editorClass { ^PdefEditor }
}
