

ServerGui : ObjectGui {
	var status,running,stopped,booting;
	
	writeName {}
	guiBody { arg layout;

		var active,booter;
		
		if(model.isLocal,{
			booter = SCButton(layout, Rect(0,0, 50, 17));
			booter.states = [["Boot", Color.black, Color.clear],
						   ["Quit", Color.black, Color.clear]];
			booter.font = Font("Helvetica",10);
			booter.action = { arg view; 
				if(view.value == 1, {
					booting.value;
					if(model.isLocal,{
						InstrSynthDef.loadCacheFromDir(model);
					});
					model.boot(model.dumpMode == 0);
				});
				if(view.value == 0,{
					model.quit;
				});
			};
			booter.setProperty(\value,model.serverRunning.binaryValue);
		});
		
		active = SCStaticText(layout, Rect(0,0, 80, 17));
		active.string = model.name.asString;
		active.align = \center;
		active.font = Font("Helvetica-Bold", 12);
		active.background = Color.black;
		if(model.serverRunning,running,stopped);		
		
		if (model.isLocal, {
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
		});
		if(model.serverRunning,running,stopped);
			
		status = CXLabel(layout,"               ");
		status.font = Font("Helvetica",9);
		status.background = Color.black;
		status.stringColor = Color.green;
		status.align = \right;
		if(model.dumpMode == 0,{
			model.startAliveThread(0.0,1.0);
		});
	}
	update { arg changer,what;
		if(what == \serverRunning,{
			if(model.serverRunning,running,stopped)
		},{
			status.label = 
				model.avgCPU.round(0.1).asString ++ "/" ++ model.peakCPU.round(0.1) ++ "%      "
				+ model.numUGens ++ "u"
				+ model.numSynths ++ "s";
		})
	}
}
