Main : Process {
	var platform, argv;

	startup {
		super.startup;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;
		this.platform.startup;
		StartUp.run;
	}
	
	shutdown { // at recompile, quit
		Server.quitAll;
		this.platform.shutdown;
		super.shutdown;
	}
	
	run { // called by command-R
	
	}
	
	stop { // called by command-.


		SystemClock.clear;
		AppClock.clear;
		TempoClock.default.clear;
		CmdPeriod.clear;
		
		Server.freeAll; // stop all sounds on local servers
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

	platformClass {
		// override in platform specific extension
		^Platform
	}
	platform {
		^platform ?? { platform = this.platformClass.new }
	}
	argv {
		^argv ?? { argv = this.prArgv }
	}

	// PRIVATE
	prArgv {
		_Argv
		^[]
	}
}
