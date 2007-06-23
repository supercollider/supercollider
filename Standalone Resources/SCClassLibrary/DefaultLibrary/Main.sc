// Special version of Main for demo stand-alone application.

Main : Process {
	
	startup {
		super.startup;
		
		// set the 's' interpreter variable to the internal server.
		// You should use the internal server for standalone applications--
		// otherwise, if your application has a problem, the user will
		// be stuck with a process, possibly making sound, that he won't know 
		// how to kill
		interpreter.s = Server.internal;
//		GUI.fromID( this.platform.defaultGUIScheme );
//		GeneralHID.fromID( this.platform.defaultHIDScheme );
//		this.platform.startup;
		StartUp.run;
		
		// server windows turned off for stand-alone application
//		Server.internal.makeWindow;
//		Server.local.makeWindow;
		
		// Start the application using internal server
		interpreter.s.waitForBoot({
			var sb, demo;
			sb = SCWindow.screenBounds;
			
			demo = SCSA_Demo.new(
				"the cheese stands alone", 
				Rect(
					(sb.width - SCSA_Demo.width) * 0.5, 
					(sb.height - SCSA_Demo.height) * 0.5,
					SCSA_Demo.width,
					SCSA_Demo.height
				),
				interpreter.s
			);
			demo.front;
			// Close post window after application launches. If you want
			// to hide it completely, put this line after Document.startup instead.
			Document.closeAll(false);
		}, 25);
				
		// You probably don't want to include this, since the user won't have it
//		"~/scwork/startup.rtf".loadPaths;
	}
	
	shutdown { // at recompile, quit
		Server.quitAll;
		HIDDeviceService.releaseDeviceList;
		super.shutdown;
	}
	
	run { // called by command-R
	
	}
	
	stop { // called by command-.
		
		Server.freeAll; // stop all sounds on local servers
		
		SystemClock.clear;
		AppClock.clear;
		TempoClock.default.clear;
		CmdPeriod.clear;
		
		Server.resumeThreads;
	}
	
	recvOSCmessage { arg time, replyAddr, msg;
		// this method is called when an OSC message is received.
		OSCresponder.respond(time, replyAddr, msg);
	}
	
	recvOSCbundle { arg time, replyAddr ... msgs;
		// this method is called when an OSC bundle is received.
		msgs.do({ arg msg; 
			this.recvOSCmessage(time, replyAddr, msg); 
		});
	}
	
	newSCWindow {
		SCWindow.viewPalette;
		SCWindow.new.front;
	}
	
}