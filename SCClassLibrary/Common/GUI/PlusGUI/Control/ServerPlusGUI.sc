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
		var keys = all.asArray.collect(_.name).sort;
		^Rect(5, keys.indexOf(name) * (height + taskBarHeight) + 5, width, height)
	}

	makeGui { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped, bundling, showDefault;
		var startDump, stopDump, blockAliveThread, dumping = false;
		var recorder, scoper;
		var countsViews, serverController, serverStatusController;
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
			booter.setProperty(\value, this.serverRunning.binaryValue);

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
		if(this.serverRunning, running, stopped);

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
				{char === $p}  { if(this.serverRunning) { this.plotTree } }
				{char === $ }  { if(this.serverRunning.not) { this.boot } }
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
						"cannot dump a remote server's messages".postln
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
				defer {
					active.stringColor_(Color.new255(74, 120, 74));
					active.string = "running";
					booter.setProperty(\value,1);
					recorder.enabled = true;
				}
			};
			stopped = {
				defer {
					active.stringColor_(Color.grey(0.3));
					active.string = "inactive";
					stopDump.value;
					booter.setProperty(\value,0);
					recorder.setProperty(\value,0);
					recorder.enabled = false;
					countsViews.do(_.string = "");
				}
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
				if(this.serverRunning) { this.startAliveThread };
				dumping = false;
				w.name = label;
				CmdPeriod.remove(blockAliveThread);
			};

			w.onClose = {
				window = nil;
				serverController.remove;
				serverStatusController.remove;
			};

		} {
			running = {
				defer {
					active.stringColor_(Color.new255(74, 120, 74));
					active.string = "running";
					active.background = Color.clear;
				}
			};
			stopped = {
				defer {
					active.stringColor_(Color.grey(0.5));
					active.string = "inactive";
				}
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
				serverController.remove;
				serverStatusController.remove;
			};
		};

		showDefault = {
			makeDefault.value = (Server.default == this).binaryValue;
		};

		if(this.serverRunning, running, stopped);

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
			var volSpec, currentVolume;
			var volumeSlider, muteButton, muteActions, volController;

			currentVolume = this.volume.volume;

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
					if(this.serverRunning) {
						muteActions[me.value].value;
					} {
						"The server must be booted to mute it".warn;
						me.value_(0);
					}
				});

			volumeNum = gui.numberBox.new(w, Rect(0, 0, 28, 18))
				.font_(font)
				.value_(currentVolume)
				.align_(\center)
				.action_({arg me;
					var newdb;
					newdb = me.value.clip(-90, 6);
					this.volume_(newdb);
					volumeSlider.value_(volSpec.unmap(newdb));
					});

			volumeSlider = gui.slider.new(w, Rect(0, 0, 172, 18))
				.value_(volSpec.unmap(currentVolume).round(0.1))
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
					});
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

		serverController = SimpleController(this)
			.put(\serverRunning, {	if(this.serverRunning, running, stopped) })
			.put(\counts,{
				countsViews.at(0).string = statusWatcher.avgCPU.round(0.1);
				countsViews.at(1).string = statusWatcher.peakCPU.round(0.1);
				countsViews.at(2).string = statusWatcher.numUGens;
				countsViews.at(3).string = statusWatcher.numSynths;
				countsViews.at(4).string = statusWatcher.numGroups;
				countsViews.at(5).string = statusWatcher.numSynthDefs;
			})
			.put(\bundling, bundling)
			.put(\default, showDefault);
		if(isLocal){
			serverController.put(\cmdPeriod, {
				recorder.setProperty(\value, 0)
			})
		};
		this.startAliveThread;
	}

	plotTree { |interval, bounds|
		var window;
		nodeTreeView.isNil.if {
			bounds = bounds ?? { Rect(128, 64, 400, 400) };
			bounds = bounds.minSize(395@386);
			window = Window(name.asString ++ " Node Tree", bounds, scroll:true).front;
			nodeTreeView = this.plotTreeView(
				interval ?? { 0.5 },
				window);
			window.onClose_({ nodeTreeView = nil });
		} {
			bounds !? { nodeTreeView.window.bounds_(bounds) };
			interval !? { nodeTreeView.start(interval) };
			nodeTreeView.window.alwaysOnTop_(true).front.alwaysOnTop_(false);
		};
		^nodeTreeView
	}

	plotTreeView { |interval=0.5, parent, actionIfFail|
		^NodeTreeView(this, parent: parent).start(interval, actionIfFail);
	}
}
