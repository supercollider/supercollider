
+ Server {
	makeWindow { arg w;
		var active, booter, killer, makeDefault, running, booting, stopped;
		var countsViews, ctlr;
		
		if (window.notNil, { ^window.front });
		
		if(w.isNil,{
			w = window = SCWindow(name.asString ++ " server", 
						Rect(10, named.values.indexOf(this) * 140 + 10, 256, 100));
			w.view.decorator = FlowLayout(w.view.bounds);
		});
		
		if(isLocal,{
			booter = SCButton(w, Rect(0,0, 50, 24));
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
		
		active = SCStaticText(w, Rect(0,0, 80, 24));
		active.string = this.name.asString;
		active.align = \center;
		active.font = Font("Helvetica-Bold", 16);
		active.background = Color.black;
		if(serverRunning,running,stopped);		

		makeDefault = SCButton(w, Rect(0,0, 68, 24));
		makeDefault.states = [["-> default", Color.black, Color.clear]];
		makeDefault.action = {
			thisProcess.interpreter.s = this;
			Server.default = this;
		};
		
		killer.action = { Server.killAll };	
		
		if (isLocal, {
			
			running = {
				active.stringColor_(Color.red);
				booter.setProperty(\value,1);
			};
			stopped = {
				active.stringColor_(Color.grey(0.3));
				booter.setProperty(\value,0);
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
			};
			stopped = {
				active.background = Color.black;
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
			label = SCStaticText(w, Rect(0,0, 72, 14));
			label.string = name;
			label.align = \right;
		
			if (i < 2, { 
				numView = SCStaticText(w, Rect(0,0, 30, 14));
				numView.string = "?";
				numView.align = \left;
			
				pctView = SCStaticText(w, Rect(0,0, 12, 14));
				pctView.string = "%";
				pctView.align = \left;
			},{
				numView = SCStaticText(w, Rect(0,0, 42, 14));
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
			});	
		this.startAliveThread;
	}
}
