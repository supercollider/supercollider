Main : Process {
	
	startup {
		super.startup;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
		
		"~/scwork/startup.rtf".loadPaths;
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


