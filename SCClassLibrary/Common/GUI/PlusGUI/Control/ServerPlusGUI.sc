+ Server {

	// splitting makeWindow and makeGui, makes sure that makeWindow can be overridden
	// while maintaining the availability of the GUI server window

	makeWindow { arg w;
		if (Platform.makeServerWindowAction.notNil) {
			^Platform.makeServerWindowAction.value(this, w)
		} {
			^this.makeGui( w );
		}
	}

	calculateViewBounds {
		var width = 288, height = 98, taskBarHeight = 27; // the latter should be in SCWindow
		var keys = set.asArray.collect(_.name).sort;
		^Rect(5, keys.indexOf(name) * (height + taskBarHeight) + 5, width, height)
	}

	makeGui { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped, bundling, showDefault;
		var startDump, stopDump, blockAliveThread, dumping = false;
		var recorder, scoper;
		var countsViews, ctlr;
		var label, gui, font, volumeNum;
		var buttonColor, faintGreen, faintRed;

		if (window.notNil) { ^window.front };

		gui = GUI.current;
		font = Font.sansSerif(10);

		if (gui.id == \qt) {
			buttonColor = gui.palette.button;
			faintGreen = buttonColor.blend(Color.green, 0.2);
			faintRed = buttonColor.blend(Color.red, 0.25);
		} {
			faintGreen = Color.green.alpha_(0.2);
			faintRed = Color.red.alpha_(0.3)
		};

		if(w.isNil) {
			label = name.asString + "server";
			w = window = gui.window.new(label, this.calculateViewBounds, resizable: false);
			w.view.decorator = FlowLayout(w.view.bounds);
		} {
			label = w.name
		};

		if(isLocal) {
			booter = gui.button.new(w, Rect(0,0, 44, 18));
			booter.canFocus = false;
			booter.font = font;
			booter.states = [["Boot"],
						     ["Quit", nil, faintGreen]];

			booter.action = { arg view;
				if(view.value == 1, {
					booting.value;
					this.boot;
				});
				if(view.value == 0,{
					this.quit;
				});
			};
			booter.setProperty(\value,serverRunning.binaryValue);

			killer = gui.button.new(w, Rect(0,0, 20, 18));
			killer.states = [["K"]];
			killer.font = font;
			killer.canFocus = false;
			killer.action = { Server.killAll; stopped.value; };
		};

		active = gui.staticText.new(w, Rect(0,0, 78, 18));
		active.string = this.name.asString;
		active.align = \center;
		active.font = Font.sansSerif( 12 ).boldVariant;
		if(serverRunning,running,stopped);

		makeDefault = gui.button.new(w, Rect(0,0, 54, 18));
		makeDefault.font = font;
		makeDefault.canFocus = false;
		makeDefault.states = [["-> default"],
			["-> default", nil, faintGreen]];
		makeDefault.value_((this == Server.default).binaryValue);
		makeDefault.action = { Server.default_(this) };

		//w.view.decorator.nextLine;

		if(isLocal){
			recorder = gui.button.new(w, Rect(0,0, 66, 18));
			recorder.font = font;
			recorder.states = [
				["record >"],
				["stop []", nil, faintGreen]
			];
			recorder.action = {
				if (recorder.value == 1) { this.record } { this.stopRecording };
			};
			recorder.enabled = false;
		};

		w.view.keyDownAction = { arg view, char, modifiers;
				// if any modifiers except shift key are pressed, skip action
			if(modifiers & 16515072 == 0) {

				case
				{char === $n } { this.queryAllNodes(false) }
				{char === $N } { this.queryAllNodes(true) }
				{char === $l } { this.tryPerform(\meter) }
				{char === $p}  { if(serverRunning) { this.plotTree } }
				{char === $ }  { if(serverRunning.not) { this.boot } }
				{char === $s } { if( (this.isLocal and: (GUI.id == \qt)) or: ( this.inProcess ))
					                 {this.scope(options.numOutputBusChannels)}
					                 {warn("Scope not supported")}
				               }
				{char === $f } { if( (this.isLocal and: (GUI.id == \qt)) or: ( this.inProcess ))
					                 {this.freqscope}
					                 {warn("FreqScope not supported")}
				               }
				{char == $d } {
					if(this.isLocal or: { this.inProcess }) {
						if(dumping, stopDump, startDump)
					} {
						"cannot dump a remote server's messages".inform
					}
				}
				{char === $m } { if(this.volume.isMuted) { this.unmute } { this.mute } }
				{char === $0 and: {volumeNum.hasFocus.not}} {
					this.volume = 0.0;
				};
			};
		};

		if (isLocal) {
			running = {
				active.stringColor_(Color.new255(74, 120, 74));
				active.string = "running";
				booter.setProperty(\value,1);
				recorder.enabled = true;
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				active.string = "inactive";
				stopDump.value;
				booter.setProperty(\value,0);
				recorder.setProperty(\value,0);
				recorder.enabled = false;
				countsViews.do(_.string = "");
			};
			booting = {
				active.stringColor_(Color.new255(255, 140, 0));
				active.string = "booting";
				//booter.setProperty(\value,0);
			};
			bundling = {
				active.stringColor_(Color.new255(237, 157, 196));
				booter.setProperty(\value,1);
				recorder.enabled = false;
			};
			blockAliveThread = {
				SystemClock.sched(0.2, { this.stopAliveThread });
			};
			startDump = {
				this.dumpOSC(1);
				this.stopAliveThread;
				dumping = true;
				w.name = "dumping osc: " ++ name.asString;
				CmdPeriod.add(blockAliveThread);
			};
			stopDump = {
				this.dumpOSC(0);
				if(serverRunning) { this.startAliveThread };
				dumping = false;
				w.name = label;
				CmdPeriod.remove(blockAliveThread);
			};

			w.onClose = {
				window = nil;
				ctlr.remove;
			};

		} {
			running = {
				active.stringColor_(Color.new255(74, 120, 74));
				active.string = "running";
				active.background = Color.clear;
			};
			stopped = {
				active.stringColor_(Color.grey(0.5));
				active.string = "inactive";
			};
			booting = {
				active.stringColor_(Color.new255(255, 140, 0));
				active.string = "booting";
			};

			bundling = {
				active.stringColor = Color.new255(237, 157, 196);
				active.background = Color.red(0.5);
				booter.setProperty(\value,1);
			};

			w.onClose = {
				// but do not remove other responders
				this.stopAliveThread;
				window = nil;
				ctlr.remove;
			};
		};

		showDefault = {
			makeDefault.value = (Server.default == this).binaryValue;
		};

		if(serverRunning,running,stopped);

		w.view.decorator.nextLine;

		countsViews =
		#[
			"Avg CPU:", "Peak CPU:",
			"UGens:", "Synths:", "Groups:", "SynthDefs:"
		].collect { arg name, i;
			var label,numView, pctView;
			label = gui.staticText.new(w, Rect(0,0, 80, 12));
			label.string = name;
			label.font = font;
			label.align = \right;

			if (i < 2, {
				numView = gui.staticText.new(w, Rect(0,0, 34, 12));
				numView.font = font;
				numView.align = \left;

				pctView = gui.staticText.new(w, Rect(0,0, 12, 12));
				pctView.string = "%";
				pctView.font = font;
				pctView.align = \left;
			},{
				numView = gui.staticText.new(w, Rect(0,0, 50, 12));
				numView.font = font;
				numView.align = \left;
			});

			numView
		};

		if(isLocal or: { options.remoteControlVolume }) {
		{
			var volSpec, cpVol;
			var volumeSlider, muteButton, muteActions, volController;
			muteActions = [{this.unmute}, {this.mute}];
			volSpec = [volume.min, volume.max, \db].asSpec;

			gui.staticText.new(w, Rect(0,0, 44, 18))
				.font_(font)
				.string_("volume:");

			muteButton = gui.button.new(w, Rect(0, 0, 20, 18))
				.font_(font)
				.canFocus_(false)
				.states_([
					["M"],
					["M", nil, faintRed]
					])
				.action_({arg me;
					this.serverRunning.if({
						muteActions[me.value].value;
						}, {
						"The server must be booted to mute it".warn;
						me.value_(0);
						})
					});

			volumeNum = gui.numberBox.new(w, Rect(0, 0, 28, 18))
				.font_(font)
				.value_(0.0)
				.align_(\center)
				.action_({arg me;
					var newdb;
					newdb = me.value.clip(-90, 6);
					this.volume_(newdb);
					volumeSlider.value_(volSpec.unmap(newdb));
					});

			volumeSlider = gui.slider.new(w, Rect(0, 0, 172, 18))
				.value_(volSpec.unmap(0))
				.onClose_{volController.remove}
				.action_({arg me;
					var newdb;
					newdb = volSpec.map(me.value).round(0.1);
					this.volume_(newdb);
					volumeNum.value_(newdb);
					})
				.keyDownAction_({arg slider, char, modifiers, unicode, keycode;
					if (char == $], { slider.increment; });
					if (char == $[, { slider.decrement; });
					if (unicode == 16rF700, { slider.increment; });
					if (unicode == 16rF703, { slider.increment; });
					if (unicode == 16rF701, { slider.decrement; });
					if (unicode == 16rF702, { slider.decrement; });
					nil;
					})
					;
			volController = SimpleController(volume)
				.put(\amp, {|changer, what, vol|
					{
						volumeNum.value_(vol.round(0.01));
						volumeSlider.value_(volSpec.unmap(vol));
					}.defer
				})
				.put(\mute, {|changer, what, flag|
					{
						muteButton.value_(flag.binaryValue);
					}.defer
				})
				.put(\ampRange, {|changer, what, min, max|
					volSpec = [min, max, \db].asSpec;
					volumeSlider.value_(volSpec.unmap(volume.volume));
				})

		}.value;
		};

 		w.front;

		ctlr = SimpleController(this)
			.put(\serverRunning, {	if(serverRunning,running,stopped) })
			.put(\counts,{
				countsViews.at(0).string = avgCPU.round(0.1);
				countsViews.at(1).string = peakCPU.round(0.1);
				countsViews.at(2).string = numUGens;
				countsViews.at(3).string = numSynths;
				countsViews.at(4).string = numGroups;
				countsViews.at(5).string = numSynthDefs;
			})
			.put(\bundling, bundling)
			.put(\default, showDefault);
		if(isLocal){
			ctlr.put(\cmdPeriod,{
					recorder.setProperty(\value,0);
				})
		};

		this.startAliveThread;
	}

	plotTree {|interval=0.5|
		var resp, done = false;
		var collectChildren, levels, countSize;
		var window, view, bounds;
		var updater, updateFunc;
		var tabSize = 25;
		var pen, font;

		pen = GUI.current.pen;
		font = Font.sansSerif(10);

		window = Window.new(name.asString + "Node Tree",
			Rect(128, 64, 400, 400),
			scroll:true
		).front;
		window.view.hasHorizontalScroller_(false).background_(Color.grey(0.9));

		view = UserView.new(window, Rect(0,0,400,400));

		view.drawFunc = {
			var xtabs = 0, ytabs = 0, drawFunc;

			drawFunc = {|group|
				var thisSize, rect, endYTabs;
				xtabs = xtabs + 1;
				ytabs = ytabs + 1;
				pen.font = font;
				group.do({|node|
					if(node.value.isArray, {
						thisSize = countSize.value(node);
						endYTabs = ytabs + thisSize + 0.2;
						rect = Rect(xtabs * tabSize + 0.5,
							ytabs * tabSize + 0.5,
							window.view.bounds.width - (xtabs * tabSize * 2),
							thisSize * tabSize;
						);
						pen.fillColor = Color.grey(0.8);
						pen.fillRect(rect);
						pen.strokeRect(rect);
						pen.color = Color.black;
						pen.stringInRect(
							" Group" + node.key.asString +
							(node.key == 1).if("- default group", ""),
							rect
						);
						drawFunc.value(node.value);
						ytabs = endYTabs;
					},{
						rect = Rect(xtabs * tabSize + 0.5,
							ytabs * tabSize + 0.5,
							7 * tabSize,
							0.8 * tabSize
						);
						pen.fillColor = Color.white;
						pen.fillRect(rect);
						pen.strokeRect(rect);
						pen.color = Color.black;
						pen.stringInRect(
							" " ++ node.key.asString + node.value.asString,
							rect
						);
						ytabs = ytabs + 1;
					});
				});
				xtabs = xtabs - 1;
			};
			drawFunc.value(levels);
		};

		// msg[1] controls included
		// msg[2] nodeID of queried group
		// initial number of children
		resp = OSCFunc({ arg msg;
			var finalEvent;
			var i = 2, j, controls, printControls = false, dumpFunc;
			if(msg[1] != 0, {printControls = true});
			dumpFunc = {|numChildren|
				var event, children;
				event = ().group;
				event.id = msg[i];
				event.instrument = nil; // need to know it's a group
				i = i + 2;
				children = Array.fill(numChildren, {
					var id, child;
					// i = id
					// i + 1 = numChildren
					// i + 2 = def (if synth)
					id = msg[i];
					if(msg[i+1] >=0, {
						child = dumpFunc.value(msg[i+1]);
					}, {
						j = 4;
						child = ().synth.instrument_(msg[i+2]);
						if(printControls, {
							controls = ();
							msg[i+3].do({
								controls[msg[i + j]] = msg[i + j + 1];
								j = j + 2;
							});
							child.controls = controls;
							i = i + 4 + (2 * controls.size);
						}, {i = i + 3 });
					});
					child.id = id;
				});
				event.children = children;
				event;
			};
			finalEvent = dumpFunc.value(msg[3]);
			done = true;
			collectChildren = {|group|
				group.children.collect({|child|
					if(child.children.notNil,{
						child.id -> collectChildren.value(child);
					}, {
						child.id -> child.instrument;
					});
				});
			};
			levels = collectChildren.value(finalEvent);
			countSize = {|array|
				var size = 0;
				array.do({|elem|
					if(elem.value.isArray, { size = size + countSize.value(elem.value) + 2}, {size = size + 1;});
				});
				size
			};
			defer {
				view.bounds = Rect(0, 0, 400, max(400, tabSize * (countSize.value(levels) + 2)));
				view.refresh;
			}
		}, '/g_queryTree.reply', addr).fix;

		updateFunc = {
			fork {
				loop {
					this.sendMsg("/g_queryTree", 0, 0);
					interval.wait;
				}
			}
		};
		updater = updateFunc.value;
		CmdPeriod.add(updateFunc);
		window.onClose = {
			updater.stop;
			CmdPeriod.remove(updateFunc);
			resp.free;
		};
		SystemClock.sched(3, {
			if(done.not, {
				defer {window.close};
				"Server failed to respond to Group:queryTree!".warn;
			});
		});
	}
}
