+ Server {
	makeWindow { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped;
		var recorder, scoper;
		var countsViews, ctlr;
		var dumping=false;
		
		if (window.notNil, { ^window.focus });
		
		if(w.isNil,{
			w = window = EmacsBuffer("*" ++ name.asString ++ " server*");
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
			booter.value=serverRunning.binaryValue;
			
			killer = EmacsButton(w, ["K"], { Server.killAll });
			killer.enabled = false;
		});
		
		active = EmacsText(w, this.name.asString, 12, \center);
		//		active.background = Color.black;
		if(serverRunning,running,stopped);		

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
		
		/*
				w.view.keyDownAction = { arg ascii, char;
				var startDump, stopDump, stillRunning;
			
			case 
			{char === $n} { this.queryAllNodes }
			{char === $ } { if(serverRunning.not) { this.boot } }
			{char === $s and: {this.inProcess}} { this.scope }
			{char == $d} {
				stillRunning = {
					SystemClock.sched(0.2, { this.stopAliveThread });
				};
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
			
			};
				};*/
		
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
				window = nil;
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
		if(serverRunning,running,stopped);
			
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
		
		w.focus;

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
				recorder.value=0;
			});	
		w.gotoBob;
		this.startAliveThread;
	}
}
