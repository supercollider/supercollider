Main : Process {
	var <platform, argv;
	var recvOSCfunc, prRecvOSCFunc;
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

		openPorts = Set[NetAddr.langPort];
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
				"sced",  {"For help type ctrl-U."},
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
					linux: "For help type ctrl-c ctrl-h (Emacs) or :SChelp (vim) or ctrl-U (sced/gedit).",
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

	recvOSCmessage { arg time, replyAddr, recvPort, msg;
		// this method is called when an OSC message is received.
		recvOSCfunc.value(time, replyAddr, msg);
		prRecvOSCFunc.value(msg, time, replyAddr, recvPort); // same order as OSCFunc
		OSCresponder.respond(time, replyAddr, msg);
	}

	addOSCRecvFunc { |func| prRecvOSCFunc = prRecvOSCFunc.addFunc(func) }

	removeOSCRecvFunc { |func| prRecvOSCFunc = prRecvOSCFunc.removeFunc(func) }

	replaceOSCRecvFunc { |func, newFunc| prRecvOSCFunc = prRecvOSCFunc.replaceFunc(func, newFunc) }

	openPorts { ^openPorts.copy } // don't allow the Set to be modified from the outside

	openUDPPort {|portNum|
		var result;
		if(openPorts.includes(portNum), {^true});
		result = this.prOpenUDPPort(portNum);
		if(result, { openPorts = openPorts.add(portNum); });
		^result;
	}

	prOpenUDPPort {|portNum|
		_OpenUDPPort
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
