+ Server {
	makeEmacsWindow { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped;
		var recorder, scoper;
		var countsViews, ctlr;
		var dumping=false, startDump, stopDump, stillRunning;

		if (emacsbuf.notNil, { ^emacsbuf.front });

		if(w.isNil,{
			w = emacsbuf = EmacsBuffer("*" ++ name.asString ++ " server*");
		});

		if(isLocal,{
			booter = EmacsButton(w, ["Boot","Quit"]);
			booter.action = { arg value;
				if(value == 1, {
					booting.value;
					this.boot;
				});
				if(value == 0,{
					this.quit;
				});
			};
			booter.value=this.serverRunning.binaryValue;

			killer = EmacsButton(w, ["K"], { Server.killAll });
			killer.enabled = false;
		});

		active = EmacsText(w, this.name.asString, 12, \center);
		//		active.background = Color.black;
		if(this.serverRunning,running,stopped);

		makeDefault = EmacsButton(w, ["-> default"], {
			thisProcess.interpreter.s = this;
			Server.default = this;
		});

		w.newline;

		recorder = EmacsButton(w, ["prepare rec","record >","stop []"], {
			if (recorder.value == 1) {
				this.prepareForRecord;
			}{
				if (recorder.value == 2) { this.record } { this.stopRecording };
			};
		});

		recorder.enabled = false;

		stillRunning = {
			SystemClock.sched(0.2, { this.stopAliveThread });
		};
		w.defineKey("n", { this.queryAllNodes })
		 .defineKey(" ", { if(this.serverRunning.not) { this.boot } })
		 .defineKey("d", {
			 startDump = {
				 this.dumpOSC(1);
				 this.stopAliveThread;
				 dumping = true;
				 CmdPeriod.add(stillRunning);
			 };
			 stopDump = {
				 this.dumpOSC(0);
				 this.startAliveThread;
				 dumping = false;
				 CmdPeriod.remove(stillRunning);
			 };
			 if(dumping, stopDump, startDump)
		 });

		if (isLocal, {
			running = {
				//				active.stringColor_(Color.red);
				booter.value=1;
				recorder.enabled = true;
				killer.enabled = true;
			};
			stopped = {
				//				active.stringColor_(Color.grey(0.3));
				booter.value=0;
				recorder.value=0;
				recorder.enabled = false;
				killer.enabled = false;
			};
			booting = {
				//				active.stringColor_(Color.yellow(0.9));
				//booter.setProperty(\value,0);
			};

			w.onClose = {
				emacsbuf = nil;
				ctlr.remove;
			};
		},{
			running = {
				//				active.background = Color.red;
				recorder.enabled = true;
			};
			stopped = {
				//				active.background = Color.black;
				recorder.value=0;
				recorder.enabled = false;
			};
			booting = {
				//				active.background = Color.yellow;
			};
			w.onClose = {
				this.stopAliveThread;
				ctlr.remove;
			};
		});
		if(this.serverRunning,running,stopped);

		w.newline;

		countsViews =
		#[
			"Avg CPU: ", "Peak CPU: ",
			"UGens: ", "Synths: ", "Groups: ", "SynthDefs: "
		].collect({ arg name, i;
			var label,numView, pctView;
			label = EmacsText(w, name, 12, \right);

			if (i < 2, {
				numView = EmacsText(w, "?", 5, \right);
				pctView = EmacsText(w, "%");
			},{
				numView = EmacsText(w, "?", 6, \left);
			});
			if (i == 1) { w.newline };
			numView
		});

		ctlr = SimpleController(this)
			.put(\serverRunning, {	if(this.serverRunning,running,stopped) })
			.put(\counts,{
				countsViews.at(0).string = this.avgCPU.round(0.1);
				countsViews.at(1).string = this.peakCPU.round(0.1);
				countsViews.at(2).string = this.numUGens;
				countsViews.at(3).string = this.numSynths;
				countsViews.at(4).string = this.numGroups;
				countsViews.at(5).string = this.numSynthDefs;
			})
			.put(\cmdPeriod,{
				recorder.value=0;
			});
		w.gotoBob;
		w.front;
		this.startAliveThread;
	}
}

+ SynthDesc {
	makeEmacsWindow {
		var w, s, startButton, sliders;
		var id, cmdPeriodFunc;
		var synthDesc;
		var usefulControls, numControls;
		var getSliderValues;

		s = Server.default;

		usefulControls = controls.select {|controlName, i|
			var ctlname;
			ctlname = controlName.name;
			(ctlname != "?") && (ctlname != "gate")
		};

		numControls = usefulControls.size;
		sliders = Array.newClear(numControls);

		id = s.nextNodeID; // generate a note id.

		// make the window
		w = EmacsBuffer("*SynthDesc"+name++"*");
		// add a button to start and stop the sound.
		startButton = EmacsButton(w, ["Start","Stop"]);
		w.newline;
		getSliderValues = {
			var envir;

			envir = ();
			usefulControls.do {|controlName, i|
				var ctlname;
				ctlname = controlName.name.asSymbol;
				envir.put(ctlname, sliders[i].value);
			};
			envir.use {
				msgFunc.valueEnvir
			};
		};

		startButton.action = {|value|
				if (value == 1) {
					// start sound
					s.sendBundle(s.latency, ["/s_new", name, id, 0, 0] ++ getSliderValues.value);
				};
				if (value == 0) {
					if (hasGate) {
						// set gate to zero to cause envelope to release
						s.sendMsg("/n_set", id, "gate", 0);
					}{
						s.sendMsg("/n_free", id);
					};
				};
		};

		// create controls for all parameters
		usefulControls.do {|controlName, i|
			var ctlname, ctlname2, capname, spec;
			ctlname = controlName.name;
			capname = ctlname.copy;
			capname[0] = capname[0].toUpper;
			spec = ctlname.asSymbol.asSpec;
			sliders[i] = EmacsNumber(w, capname, spec?ControlSpec(-1e8,1e8),
				{|ez| s.sendMsg("/n_set", id, ctlname, ez); }, controlName.defaultValue);
		};

		// set start button to zero upon a cmd-period
		cmdPeriodFunc = { startButton.value = 0; };
		CmdPeriod.add(cmdPeriodFunc);

		// stop the sound when window closes and remove cmdPeriodFunc.
		w.onClose = {
			s.sendMsg("/n_free", id);
			CmdPeriod.remove(cmdPeriodFunc);
		};
		w.front; // make window visible and front window.
	}
}
