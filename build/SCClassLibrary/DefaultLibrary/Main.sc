Main : Process {
	// do not change the next lines manually:
	//==== replace with new version from bash script ====
classvar scVersionMajor=3, scVersionMinor=3, scVersionPostfix=1;
	//==== end replace ====

	var <platform, argv;
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
		
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		this.platform.startup;
		StartUp.run;
		(
			osx: "Welcome to SuperCollider, type cmd-d for help",
			linux: "Welcome to SuperCollider, for help type ctrl-c ctrl-h (Emacs) or :SChelp (vim) or ctrl-U (sced/gedit)",
		 	windows: "Welcome to SuperCollider, press F1 for help"
		 ).at(platform.name).postln;
	}
	
	shutdown { // at recompile, quit
		Server.quitAll;
		this.platform.shutdown;
		super.shutdown;
	}
	
	run { // used to be called by command-R, customisation now via CocoaMenuItem
		//interpreter.interpretPrintCmdLine;
	}
	
	stop { // called by command-.
		CmdPeriod.run;
	}
	
	hardStop { // called by command alt dot
		CmdPeriod.hardRun;
	}
	
	recvOSCmessage { arg time, replyAddr, msg;
		// this method is called when an OSC message is received.
		recvOSCfunc.value(time, replyAddr, msg);
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
	
	showClassBrowser {
		var string, class, method, words;
		string = interpreter.cmdLine;
		class = string.asSymbol.asClass;
		(class ? Object).browse;
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
	
	recompile { platform.recompile }
	
	escapeWindow { platform.escapeWindow }
	
	exitFullScreen { platform.exitFullScreen }
}
