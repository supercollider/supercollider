Main : Process {
	// do not change the next lines manually:
	//==== replace with new version from bash script ====
classvar scVersionMajor=3, scVersionMinor=2, scVersionPostfix="1";
	//==== end replace ====

	var <platform, argv;
	var <>recvOSCfunc;
	
		// proof-of-concept: the interpreter can set this variable when executing code in a file
		// should be nil most of the time
	var	<>nowExecutingPath;

	startup {	
		var mainpath, mainlocStr, rootdir;
		// setup the platform first so that class initializers can call platform methods.
		// create the platform, then intialize it so that initPlatform can call methods
		platform = this.platformClass.new;
		platform.initPlatform;
		super.startup;
		
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.internal;
		Server.default = interpreter.s;
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		mainlocStr = thisMethod.filenameSymbol.asString;
		rootdir = mainlocStr[0..mainlocStr.size - 38]; 
		SynthDef.synthDefDir_(rootdir ++ "synthdefs");
		this.platform.startup;
		StartUp.run;
		(
			osx: "Welcome to SuperCollider, type cmd-d for help",
			linux: "Welcome to SuperCollider, for help type ctrl-c ctrl-h (Emacs) or :SChelp (vim)",
		 	windows: "Welcome to SuperCollider, press F1 for help"
		 ).at(platform.name).postln;
		
		// from here on, you shoulod customize what should happen...
		// I like to boot, then use thisProcess.interpreter.executeFile
		// OR - put things into a class... like the SCSA_Demo
		interpreter.s.boot;
		
		interpreter.s.waitForBoot({
			SCSA_Demo.new("The Cheese Stands Alone", Rect(400, 400, 300, 200), interpreter.s).front;
//			thisProcess.interpreter.executeFile(rootdir ++ "MFBSD.rtf");
			});
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

//	override in platform specific extension
//
//	platformClass {		
//		^Platform
//	}

	argv {
		^argv ?? { argv = this.prArgv }
	}

	showHelpBrowser {
		Help.gui
	}
	showHelpSearch {
		Help.searchGUI
	}

	*version {^[scVersionMajor, ".", scVersionMinor, scVersionPostfix].join}
	
	*versionAtLeast { |maj, min|
		^if((maj==scVersionMajor) and:{min.notNil}){
			scVersionMinor >= min
		}{
			scVersionMajor >= maj
		};
	}
	
	*versionAtMost { |maj, min|
		^if((maj==scVersionMajor) and:{min.notNil}){
			scVersionMinor <= min
		}{
			scVersionMajor <= maj
		};
	}
	
	pid {
		_GetPid
		^this.primitiveFailed
	}
	
	// PRIVATE
	prArgv {
		_Argv
		^[]
	}
}
