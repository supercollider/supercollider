
+ Server {

	// splitting makeWindow and makeGui, makes sure that makeWindow can be overridden 
	// while maintaining the availability of the GUI server window
	
	makeWindow { arg w;
		this.makeGui( w );
	}
	
	calculateViewBounds {
		var width = 288, height = 98, taskBarHeight = 27; // the latter should be in SCWindow
		var keys = set.asArray.collect(_.name).sort;
		^Rect(5, keys.indexOf(name) * (height + taskBarHeight) + 5, width, height)
	}
	
	makeGui { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped, bundling;
		var recorder, scoper;
		var countsViews, ctlr;
		var dumping = false, label, gui, font, volumeNum;
		
		if (window.notNil) { ^window.front };
		
		gui = GUI.current;
		font = GUI.font.new("Helvetica", 10);
		
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
			booter.states = [["Boot", Color.black, Color.clear],
						   ["Quit", Color.black, Color.green.alpha_(0.2)]];
			
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
			killer.states = [["K", Color.black, Color.clear]];
			killer.font = font;
			killer.canFocus = false;
			killer.action = { Server.killAll };	
		};
		
		active = gui.staticText.new(w, Rect(0,0, 78, 18));
		active.string = this.name.asString;
		active.align = \center;
		active.font = gui.font.new( gui.font.defaultSansFace, 12 ).boldVariant;
		active.background = Color.white;
		if(serverRunning,running,stopped);		

		makeDefault = gui.button.new(w, Rect(0,0, 54, 18));
		makeDefault.font = font;
		makeDefault.canFocus = false;
		makeDefault.states = [["-> default", Color.black, Color.clear]];
		makeDefault.action = {
			thisProcess.interpreter.s = this;
			Server.default = this;
		};

		//w.view.decorator.nextLine;
		
		recorder = gui.button.new(w, Rect(0,0, 66, 18));
		recorder.font = font;
		recorder.states = [
			["prepare rec", Color.black, Color.clear],
			["record >", Color.red, Color.gray(0.1).alpha_(0.3)],
			["stop []", Color.black, Color.red.alpha_(0.3)]
		];
		
		recorder.action = {
			if (recorder.value == 1) {
				this.prepareForRecord;
			}{
				if (recorder.value == 2) { this.record } { this.stopRecording };
			};
		};
		recorder.enabled = false;
		
		w.view.keyDownAction = { arg view, char, modifiers;
			var startDump, stopDump, stillRunning;
			
				// if any modifiers except shift key are pressed, skip action
			if(modifiers & 16515072 == 0) {
				
				case 
				{char === $n } { this.queryAllNodes(false) }
				{char === $N } { this.queryAllNodes(true) }
				{char === $l } { this.tryPerform(\meter) }
				{char === $ } { if(serverRunning.not) { this.boot } }
				{char === $s and: { gui.stethoscope.isValidServer( this ) } } { 
					GUI.use( gui, { this.scope })}
				{char == $d } {
					if(this.isLocal or: { this.inProcess }) {
						stillRunning = {
							SystemClock.sched(0.2, { this.stopAliveThread });
						};
						startDump = { 
							this.dumpOSC(1);
							this.stopAliveThread;
							dumping = true;
							w.name = "dumping osc: " ++ name.asString;
							CmdPeriod.add(stillRunning);
						};
						stopDump = {
							this.dumpOSC(0);
							this.startAliveThread;
							dumping = false;
							w.name = label;
							CmdPeriod.remove(stillRunning);
						};
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
		
		if (isLocal, {
			
			running = {
				active.stringColor_(Color.new255(74, 120, 74));
				active.string = "running";
				booter.setProperty(\value,1);
				recorder.enabled = true;
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				active.string = "inactive";
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
			
			w.onClose = {
				window = nil;
				ctlr.remove;
			};
		},{	
			running = {
				active.stringColor_(Color.new255(74, 120, 74));
				active.string = "running";
				active.background = Color.white;
				recorder.enabled = true;
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				active.string = "inactive";
				recorder.setProperty(\value,0);
				recorder.enabled = false;

			};
			booting = {
				active.stringColor_(Color.new255(255, 140, 0));
				active.string = "booting";
			};
			
			bundling = {
				active.stringColor = Color.new255(237, 157, 196);
				active.background = Color.red(0.5);
				booter.setProperty(\value,1);
				recorder.enabled = false;
			};
			
			w.onClose = {
				// but do not remove other responders
				this.stopAliveThread;
				window = nil;
				ctlr.remove;
			};
		});
		if(serverRunning,running,stopped);
			
		w.view.decorator.nextLine;

		countsViews = 
		#[
			"Avg CPU :", "Peak CPU :", 
			"UGens :", "Synths :", "Groups :", "SynthDefs :"
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
				.string_(" volume :");

			muteButton = gui.button.new(w, Rect(0, 0, 20, 16))
				.font_(font)
				.canFocus_(false)
				.states_([
					["M", Color.black, Color.clear],
					["M", Color.black, Color.red.alpha_(0.3)]
					])
				.action_({arg me;
					this.serverRunning.if({
						muteActions[me.value].value;
						}, {
						"The server must be booted to mute it".warn;
						me.value_(0);
						})
					});
					
			volumeNum = gui.numberBox.new(w, Rect(0, 0, 28, 15))
				.font_(font)
				.value_(0.0)
				.align_(\center)
				.action_({arg me;
					var newdb;
					newdb = me.value.clip(-90, 6);
					this.volume_(newdb);
					volumeSlider.value_(volSpec.unmap(newdb));
					});
			
			volumeSlider = gui.slider.new(w, Rect(0, 0, 172, 16))
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
			.put(\cmdPeriod,{
				recorder.setProperty(\value,0);
			})
			.put(\bundling, bundling);	
		this.startAliveThread;
	}
}
