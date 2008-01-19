Main : Process {
	var platform, argv;
	var <>recvOSCfunc;
	
		// proof-of-concept: the interpreter can set this variable when executing code in a file
		// should be nil most of the time
	var	<>nowExecutingPath;

	startup {
		super.startup;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		this.platform.startup;
		StartUp.run;
		(
			osx: "Welcome to SuperCollider, type cmd-d for help",
			linux: "Welcome to SuperCollider, type ctrl-c ctl-h for help from Emacs",
		 	windows: "Welcome to SuperCollider, press F1 for help"
		 ).at(platform.name).postln;
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
//		TempoClock.default.clear;
		CmdPeriod.clear;
		
		Server.freeAll; // stop all sounds on local servers
		Server.resumeThreads;
	}
	
	recvOSCmessage { arg time, replyAddr, msg;
		/// added by tboverma on Jul-17-2006
		recvOSCfunc.value(time, replyAddr, msg);
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
		var win, palette;
		win = SCWindow("construction");
		win.front;
		win.toggleEditMode;
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

	showHelpBrowser {
		Help.gui
	}
	
	// PRIVATE
	prArgv {
		_Argv
		^[]
	}
}
