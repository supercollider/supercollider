

ServerGui : ObjectGui {
	var status,running,stopped,booting,recorder, updater;

	writeName {}
	guiBody { arg layout;

		var active,booter;

		if(model.isLocal,{
			booter = GUI.button.new(layout, Rect(0,0, 47, GUI.skin.buttonHeight));
			booter.states = [["Boot", Color.black, Color.clear],
						   ["Quit", Color.black, Color.clear]];
			booter.font = GUI.font.new("Helvetica",10);
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

		active = GUI.staticText.new(layout, Rect(0,0, 60, GUI.skin.buttonHeight));
		active.string = model.name.asString;
		active.align = \center;
		active.font = GUI.font.new("Helvetica-Bold", 11);
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

		status = CXLabel(layout,"                                       ",font: GUI.font.new("Helvetica",9));
		status.background = Color.black;
		status.stringColor = Color.green;
		status.align = \right;
		if(model.dumpMode == 0,{
			model.startAliveThread(0.0,1.0);
		});

		recorder = GUI.button.new(layout, Rect(0,0, 10, GUI.skin.buttonHeight));
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
					\counts -> {
						 status.label =
							model.avgCPU.round(0.1).asString ++ "/" ++ model.peakCPU.round(0.1) ++ "%      "
							+ model.numUGens ++ "u"
							+ model.numSynths ++ "s";
					},
					\cmdPeriod -> {
						recorder.setProperty(\value,0);
					}];
	}
	update { arg changer,what;
		updater[what].value;
	}
}

ServerErrorGui : ObjectGui {

	var failer,errors,handler;

	writeName {}
	guiBody { arg layout;
		errors = CXLabel(layout,"                                          ");
		errors.font = GUI.font.new("Helvetica-Bold",12);
		errors.background = Color.grey(0.4,0.8);
		errors.stringColor = Color.white;
		errors.align = \left;
		// Document.listener.front;

		failer = OSCresponderNode(model.addr, '/fail', { arg time, responder, msg;
			{
				// band-aid  the window close is not always removing
				if(errors.isClosed,{ this.remove },{
					errors.label = msg[1].asString + msg[2].asString + "(" ++ time.asTimeString++")";
				});
			}.defer
		});
		failer.add;
		if(thisThread.exceptionHandler.notNil,{
			"ServerErrorGui : There is already an exception handler installed".inform;
		},{
			thisThread.exceptionHandler = handler = { |error|
				if(Error.handling,{ error.dump; this.halt; });
				Error.handling = true;
				{
					if(errors.isClosed,{
						this.remove
					},{
						errors.label = error.errorString.copyRange(0,50);
					});
					nil
				}.defer;
				nil.handleError(error);
			};
		});

		//layout.removeOnClose(this);
	}

	remove {
		// closing large windows fail to call close on all views
		if(thisThread.exceptionHandler === handler,{
			thisThread.exceptionHandler = nil;
		});
		if(errors.isClosed.not,{
			failer.remove;
			super.remove;
		})
	}
	isClosed {
		^errors.isClosed
	}
}


