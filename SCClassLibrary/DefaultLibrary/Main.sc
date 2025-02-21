Main : Process {
	var <platform, argv;
	var recvOSCfunc, prRecvOSCFunc;
	var recvRawFunc, prRecvRawFunc;
	var openPorts;

		// proof-of-concept: the interpreter can set this variable when executing code in a file
		// should be nil most of the time

	startup {
		var didWarnOverwrite = false;
		// setup the platform first so that class initializers can call platform methods.
		// create the platform, then intialize it so that initPlatform can call methods
		// that depend on thisProcess.platform methods.
		platform = this.platformClass.new;
		platform.initPlatform;

		super.startup;

		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;

		// 'langPort' may fail if no UDP port was available
		// this wreaks several manners of havoc, so, inform the user
		// also allow the rest of init to proceed
		try {
			openPorts = Set[NetAddr.langPort];
		} { |error|
			openPorts = Set.new;  // don't crash elsewhere
			"\n\nWARNING: An error occurred related to network initialization.".postln;
			"The error is '%'.\n".postf(error.errorString);
			"There may be an error message earlier in the sclang startup log.".postln;
			"Please look backward in the post window and report the error on the mailing list or user forum.".postln;
			"You may be able to resolve the problem by killing 'sclang%' processes in your system's task manager.\n\n"
			.postf(if(this.platform.name == \windows) { ".exe" } { "" });
		};

		this.platform.startup;
		StartUp.run;

		Main.overwriteMsg.split(Char.nl).drop(-1).collect(_.split(Char.tab)).do {|x|
			if(x[2].beginsWith(Platform.classLibraryDir) and: {x[1].contains(""+/+"SystemOverwrites"+/+"").not}
			) {
				warn("Extension in '%' overwrites % in main class library.".format(x[1],x[0]));
				didWarnOverwrite = true;
			}
		};
		if(didWarnOverwrite) {
			postln("Intentional overwrites must be put in a 'SystemOverwrites' subfolder.")
		};

		("\n\n*** Welcome to SuperCollider %. ***".format(Main.version)
			+ (Platform.ideName.switch(
				"scvim", {"For help type :SChelp."},
				"scel",  {"For help type C-c C-y."},
				"scapp", {"For help type cmd-d."},
				"scqt", {
					if (Platform.hasQtWebEngine) {
						"For help press %.".format(if(this.platform.name==\osx,"Cmd-D","Ctrl-D"))
					} {
						"For help visit http://doc.sccode.org" // Help browser is not available
					}
			}) ?? {
				(
					osx: "For help type cmd-d.",
					linux: "For help type ctrl-c ctrl-h (Emacs) or :SChelp (vim).",
					windows: "For help press F1.",
					iphone: ""
				).at(platform.name);

			})
		).postln;
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

	// recvOSCmessage, recvRawMessage are invoked from C++ when OSC or raw UDP messages are recieved
	recvOSCmessage { |time, replyAddr, recvPort, msg|
		recvOSCfunc.value(time, replyAddr, msg);
		prRecvOSCFunc.value(msg, time, replyAddr, recvPort); // same order as OSCFunc
		OSCresponder.respond(time, replyAddr, msg);
	}

	recvRawMessage { |time, replyAddr, recvPort, msg|
		recvRawFunc.value(time, replyAddr, msg);
		prRecvRawFunc.value(msg, time, replyAddr, recvPort);
	}

	addOSCRecvFunc { |func| prRecvOSCFunc = prRecvOSCFunc.addFunc(func) }

	removeOSCRecvFunc { |func| prRecvOSCFunc = prRecvOSCFunc.removeFunc(func) }

	replaceOSCRecvFunc { |func, newFunc| prRecvOSCFunc = prRecvOSCFunc.replaceFunc(func, newFunc) }

	addRawRecvFunc { |func| prRecvRawFunc = prRecvRawFunc.addFunc(func) }

	removeRawRecvFunc { |func| prRecvRawFunc = prRecvRawFunc.removeFunc(func) }

	replaceRawRecvFunc { |func, newFunc| prRecvRawFunc = prRecvRawFunc.replaceFunc(func, newFunc) }

	openPorts { ^openPorts.copy } // don't allow the Set to be modified from the outside

	openUDPPort { |portNum, type=\osc|

		var result;
		if(openPorts.includes(portNum), {^true});

		switch (type,
			\osc, {
				result = this.prOpenOSCUDPPort(portNum)

			},
			\raw, {
				result = this.prOpenRawUDPPort(portNum)
			},
			{
				Exception("Cannot open UDP port with type '%' because it doesn't exist (types are \\osc, \\raw).".format(type)).throw
			}
		);

		if(result, { openPorts = openPorts.add(portNum); });
		^result;
	}

	prOpenOSCUDPPort {|portNum|
		_OpenOSCUDPPort
		^this.primitiveFailed;
	}

	prOpenRawUDPPort {|portNum|
		_OpenRawUDPPort
		^this.primitiveFailed;
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
		HelpBrowser.openBrowsePage;
	}
	showHelpSearch {
		HelpBrowser.openSearchPage(this.getCurrentSelection);
	}
	showHelp {
		HelpBrowser.openHelpFor(this.getCurrentSelection);
	}

	showClassBrowser {
		var string, class, method, words;
		string = interpreter.cmdLine;
		class = string.asSymbol.asClass;
		(class ? Object).browse;
	}

	*version { ^"%.%.%%".format(this.scVersionMajor, this.scVersionMinor, this.scVersionPatch, this.scVersionTweak) }

	*scVersionMajor { _SC_VersionMajor ^this.primitiveFailed }
	*scVersionMinor { _SC_VersionMinor ^this.primitiveFailed }
	*scVersionPatch { _SC_VersionPatch ^this.primitiveFailed }
	*scVersionTweak { _SC_VersionTweak ^this.primitiveFailed }

	*versionAtLeast { |maj, min, patch|
		^if((maj == this.scVersionMajor) and: { min.notNil }) {
			if((min == this.scVersionMinor) and: { patch.notNil }) {
				this.scVersionPatch >= patch
			} {
				this.scVersionMinor >= min
			}
		} {
			this.scVersionMajor >= maj
		}
	}

	*versionAtMost { |maj, min, patch|
		^if((maj == this.scVersionMajor) and: { min.notNil }) {
			if((min == this.scVersionMinor) and: { patch.notNil }) {
				this.scVersionPatch <= patch
			} {
				this.scVersionMinor <= min
			}
		} {
			this.scVersionMajor <= maj
		}
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

	*overwriteMsg { _MainOverwriteMsg ^this.primitiveFailed }
}


MethodOverride {
	var <ownerClass, <selector, <activePath, <overriddenPath;

	*new { arg ownerClass, selector, activePath, overriddenPath;
		^super.newCopyArgs(ownerClass, selector, activePath, overriddenPath)
	}

	*fromLine { arg string;
		var parts = string.split(Char.tab);
		var class, selector;
		#class, selector = parts[0].split($:);
		^this.new(class.asSymbol.asClass, selector, parts[1], parts[2])
	}

	openFiles {
		var path2 = if(overriddenPath.beginsWith("/Common")) {
			Platform.classLibraryDir +/+ overriddenPath
			} { overriddenPath };
		activePath.openDocument;
		path2.openDocument;
	}


	*all {
		var msg = Main.overwriteMsg.drop(-1); // drop last newline
		var lines = msg.split(Char.nl);
		^lines.collect { |line| this.fromLine(line) };
	}

	*printAll {
		var all = this.all;
		var classes = all.collect(_.ownerClass).as(Set);
		if(all.isEmpty) { "There are no overwritten methods in class library".postln; ^this };
		("Overwritten methods in class library:".underlined ++ "\n\n").post;
		classes.do { |class|
			class.asString.underlined.postln;
			all.select { |x| x.ownerClass == class }.do { |x|
				var activePath = x.activePath;
				var overriddenPath = x.overriddenPath;
				("\t" ++ x.ownerClass.name ++ ":" ++ x.selector).postln;
				("\t\t" ++ activePath).postln;
				("\t\t" ++ overriddenPath).postln;
			};
			"\n".post;
		}
	}

	*printAllShort {
		var all = this.all;
		var classes = all.collect(_.ownerClass).as(Set);
		if(all.isEmpty) { "There are no overwritten methods in class library".postln; ^this };
		("Overwritten methods in class library:".underlined ++ "\n").post;
		classes.do { |class|
			all.select { |x| x.ownerClass == class }.collect { |x| x.selector }.as(Set).do { |x|
				postf("\t%:%\n", class, x);
			}
		}
	}

}
