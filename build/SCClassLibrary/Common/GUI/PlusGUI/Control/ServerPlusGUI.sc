
+ Server {
	makeWindow { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped;
		var recorder, scoper;
		var countsViews, ctlr;
		var dumping=false;
		
		if (window.notNil, { ^window.front });
		
		if(w.isNil,{
			w = window = SCWindow(name.asString ++ " server", 
						Rect(10, named.values.indexOf(this) * 120 + 10, 306, 92));
			w.view.decorator = FlowLayout(w.view.bounds);
		});
		
		if(isLocal,{
			booter = SCButton(w, Rect(0,0, 48, 24));
			booter.states = [["Boot", Color.black, Color.clear],
						   ["Quit", Color.black, Color.clear]];
			
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
			
			killer = SCButton(w, Rect(0,0, 24, 24));
			killer.states = [["K", Color.black, Color.clear]];
			
			killer.action = { Server.killAll };	
		});
		
		active = SCStaticText(w, Rect(0,0, 78, 24));
		active.string = this.name.asString;
		active.align = \center;
		active.font = Font("Helvetica-Bold", 16);
		active.background = Color.black;
		if(serverRunning,running,stopped);		

		makeDefault = SCButton(w, Rect(0,0, 60, 24));
		makeDefault.states = [["-> default", Color.black, Color.clear]];
		makeDefault.action = {
			thisProcess.interpreter.s = this;
			Server.default = this;
		};

		//w.view.decorator.nextLine;
		
		recorder = SCButton(w, Rect(0,0, 72, 24));
		recorder.states = [
			["prepare rec", Color.black, Color.clear],
			["record >", Color.red, Color.gray(0.1)],
			["stop []", Color.black, Color.red]
		];
		recorder.action = {
			if (recorder.value == 1) {
				this.prepareForRecord;
			}{
				if (recorder.value == 2) { this.record } { this.stopRecording };
			};
		};
		recorder.enabled = false;
		
		w.view.keyDownAction = { arg ascii, char;
			var startDump, stopDump, stillRunning;
			
			case 
			{char === $n} { this.queryAllNodes }
			{char === $ } { if(serverRunning.not) { this.boot } }
			{char === $s and: {this.inProcess}} { this.scope }
			{char == $d} {
				if(this.isLocal or: { this.inProcess }) {
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
				} {
					"cannot dump a remote server's messages".inform
				}
			
			};
		};
		
		if (isLocal, {
			
			running = {
				active.stringColor_(Color.red);
				booter.setProperty(\value,1);
				recorder.enabled = true;
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				booter.setProperty(\value,0);
				recorder.setProperty(\value,0);
				recorder.enabled = false;

			};
			booting = {
				active.stringColor_(Color.yellow(0.9));
				//booter.setProperty(\value,0);
			};
			
			w.onClose = {
				//OSCresponder.removeAddr(addr);
				//this.stopAliveThread;
				//this.quit;
				window = nil;
				ctlr.remove;
			};
		},{	
			running = {
				active.background = Color.red;
				recorder.enabled = true;
			};
			stopped = {
				active.background = Color.black;
				recorder.setProperty(\value,0);
				recorder.enabled = false;

			};
			booting = {
				active.background = Color.yellow;
			};
			w.onClose = {
				// but do not remove other responders
				this.stopAliveThread;
				ctlr.remove;
			};
		});
		if(serverRunning,running,stopped);
			
		w.view.decorator.nextLine;

		countsViews = 
		#[
			"Avg CPU :", "Peak CPU :", 
			"UGens :", "Synths :", "Groups :", "SynthDefs :"
		].collect({ arg name, i;
			var label,numView, pctView;
			label = SCStaticText(w, Rect(0,0, 80, 14));
			label.string = name;
			label.align = \right;
		
			if (i < 2, { 
				numView = SCStaticText(w, Rect(0,0, 38, 14));
				numView.string = "?";
				numView.align = \left;
			
				pctView = SCStaticText(w, Rect(0,0, 12, 14));
				pctView.string = "%";
				pctView.align = \left;
			},{
				numView = SCStaticText(w, Rect(0,0, 50, 14));
				numView.string = "?";
				numView.align = \left;
			});
			
			numView
		});
		
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
			});	
		this.startAliveThread;
	}
}
