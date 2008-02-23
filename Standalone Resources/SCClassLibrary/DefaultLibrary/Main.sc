Main : Process {
	var platform, argv;
	var <>recvOSCfunc;
	
		// proof-of-concept: the interpreter can set this variable when executing code in a file
		// should be nil most of the time
	var	<>nowExecutingPath;

	startup {
		// setup the platform first so that class initializers can call platform methods.
		// create the platform, then intialize it so that initPlatform can call methods
		// that depend on thisProcess.platform methods.
		platform = this.platformClass.new;
		platform.initPlatform;

		super.startup;
		// set the 's' interpreter variable to the internal server.
		interpreter.s = Server.internal;
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		this.platform.startup;
		StartUp.run;
		(
			osx: "Welcome to SuperCollider, type cmd-d for help"
		 ).at(platform.name).postln;
		/* uncomment these lines if you want to close the Server windows */
		// Server.local.window.close;
		// Server.internal.window.close;
		/* a sample stand-alone application */
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
