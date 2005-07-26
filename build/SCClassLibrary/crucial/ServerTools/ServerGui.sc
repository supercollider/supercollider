

ServerGui : ObjectGui {
	var status,running,stopped,booting,recorder, updater;
	
	writeName {}
	guiBody { arg layout;

		var active,booter;
		
		if(model.isLocal,{
			booter = SCButton(layout, Rect(0,0, 47, 17));
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
		
		active = SCStaticText(layout, Rect(0,0, 60, 17));
		active.string = model.name.asString;
		active.align = \center;
		active.font = Font("Helvetica-Bold", 11);
		active.background = Color.black;
		if(model.serverRunning,running,stopped);		
		
		if (model.isLocal, {
			running = {
				active.stringColor_(Color.red);
				booter.setProperty(\value,1);
				recorder.enabled = true;
			};
			stopped = {
				status.label = "";
				active.stringColor_(Color.grey(0.3));
				booter.setProperty(\value,0);
				recorder.enabled = false;
			};
			booting = {
				active.stringColor_(Color.yellow(0.9));
				//booter.setProperty(\value,0);
			};
			
		},{	
			running = {
				active.background = Color.red;
				recorder.enabled = true;
			};
			stopped = {
				status.label = "";
				active.background = Color.black;
				recorder.enabled = false;
			};
			booting = {
				active.background = Color.yellow;
			};
		});
			
		status = CXLabel(layout,"               ");
		status.font = Font("Helvetica",9);
		status.background = Color.black;
		status.stringColor = Color.green;
		status.align = \right;
		if(model.dumpMode == 0,{
			model.startAliveThread(0.0,1.0);
		});
		
		recorder = SCButton(layout, Rect(0,0, 10, 17));
		recorder.states = [
			["*", Color.black, Color.clear],
			["*", Color.red, Color.gray(0.1)],
			["X", Color.black, Color.red]
		];
		recorder.action = {
			if (recorder.value == 1) {
				model.prepareForRecord;
			}{
				if (recorder.value == 2) { model.record } { model.stopRecording };
			};
		};
		recorder.enabled = false;
		
		if(model.serverRunning,running,stopped);
		updater = IdentityDictionary[
					\serverRunning -> { if(model.serverRunning,running,stopped) },
					\counts -> { status.label = 
					model.avgCPU.round(0.1).asString ++ "/" ++ model.peakCPU.round(0.1) ++ "%      "
					+ model.numUGens ++ "u"
					+ model.numSynths ++ "s";},
					\cmdPeriod -> {
						recorder.setProperty(\value,0);
					}];
	}
	update { arg changer,what;
		updater[what].value;
	}
}

ServerErrorGui : ObjectGui {
	
	var failer,errors;
	
	writeName {}
	guiBody { arg layout;
		errors = CXLabel(layout,"                                          ");
		errors.font = Font("Helvetica-Bold",12);
		errors.background = Color.grey(0.4,0.8);
		errors.stringColor = Color.white;
		errors.align = \left;
		// Document.listener.front;
		
		failer = OSCresponderNode(model.addr, '/fail', { arg time, responder, msg;
			{
				errors.label = msg[1].asString + msg[2].asString + "(" ++ time.asTimeString++")";
				//errors.stringColor = Color.white;
			}.defer
		});
		failer.add;
		if(thisThread.exceptionHandler.notNil,{
			"There is already an exception handler installed".inform;
		},{
			thisThread.exceptionHandler = { |error|
				if(Error.handling,{ error.dump; this.halt; });
				Error.handling = true;
				{ errors.label = error.errorString.copyRange(0,50); nil }.defer;
				nil.handleError(error);
			}
		})
	}
	remove {
		failer.remove;
		thisThread.exceptionHandler = nil;
		super.remove;
	}
}


