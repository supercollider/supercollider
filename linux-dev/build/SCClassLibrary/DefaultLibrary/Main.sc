Main : Process {
	
	startUp {
		super.startUp;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.local;
		// make server window
		Server.internal.makeWindow;
		Server.local.makeWindow;
	}
	shutDown { // at recompile, quit
		Server.quitAll;
		super.shutDown;
	}
	run { // called by command-R
	}
	stop { // called by command-.
		// stop all sounds on local servers
		Server.freeAll;
		
		SystemClock.clear;
		AppClock.clear;
		
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


