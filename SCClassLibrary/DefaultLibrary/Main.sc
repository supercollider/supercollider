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
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		this.platform.startup;
		openPorts = Set[NetAddr.langPort];
		StartUp.run;

		("Welcome to SuperCollider %.".format(Main.version)
			+ (Platform.ideName.switch(
				"scvim", {"For help type :SChelp."},
				"scel",  {"For help type C-c C-y."},
				"sced",  {"For help type ctrl-U."},
				"scapp", {"For help type cmd-d."},
				"scqt", {"For help press %.".format(if(this.platform.name==\osx,"Cmd-D","Ctrl-D"))}
			) ?? {
				(
					osx: "For help type cmd-d.",
					linux: "For help type ctrl-c ctrl-h (Emacs) or :SChelp (vim) or ctrl-U (sced/gedit).",
				 	windows: "For help press F1.",
					iphone: ""
				 ).at(platform.name);

			})
		).postln;

		Main.overwriteMsg.split(Char.nl).drop(-1).collect(_.split(Char.tab)).do {|x|
			if(x[2].beginsWith(Platform.classLibraryDir) and: {x[1].contains(""+/+"SystemOverwrites"+/+"").not}
			) {
				warn("Extension in '%' overwrites % in main class library.".format(x[1],x[0]));
				didWarnOverwrite = true;
			}
		};
		if(didWarnOverwrite) {
			inform("\nIntentional overwrites must be put in a 'SystemOverwrites' subfolder.")
		}
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

	recvOSCbundle { arg time, replyAddr, recvPort ... msgs;
		// this method is called when an OSC bundle is received.
		msgs.do({ arg msg;
			this.recvOSCmessage(time, replyAddr, recvPort, msg);
		});
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
		^false
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

	*version {^[this.scVersionMajor, ".", this.scVersionMinor, this.scVersionPostfix].join}

	*scVersionMajor   { _SC_VersionMajor }
	*scVersionMinor   { _SC_VersionMinor }
	*scVersionPostfix { _SC_VersionPatch }

	*versionAtLeast { |maj, min|
		^if((maj==this.scVersionMajor) and:{min.notNil}){
			this.scVersionMinor >= min
		}{
			this.scVersionMajor >= maj
		};
	}

	*versionAtMost { |maj, min|
		^if((maj==this.scVersionMajor) and:{min.notNil}){
			this.scVersionMinor <= min
		}{
			this.scVersionMajor <= maj
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

	setDeferredTaskInterval { |interval| platform.setDeferredTaskInterval(interval) }

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
		activePath.openTextFile;
		path2.openTextFile;
	}

	*simplifyPath { arg path;
		var extDir, sysExtDir, quarkDir;
		extDir = Platform.userExtensionDir;
		sysExtDir = Platform.systemExtensionDir;
		quarkDir = LocalQuarks.globalPath;
		path = path.replace("'" ++ extDir, "Platform.userExtensionDir ++ '");
		path = path.replace("'" ++ sysExtDir, "Platform.systemExtensionDir ++ '");
		path = path.replace("'" ++ quarkDir, "LocalQuarks.globalPath ++ '");
		^path

	}

	*all {
		var msg = Main.overwriteMsg.drop(-1); // drop last newline
		var lines = msg.split(Char.nl);
		^lines.collect { |line| this.fromLine(line) };
	}

	*printAll { arg simplifyPaths = true;
		var all = this.all;
		var classes = all.collect(_.ownerClass).as(Set);
		if(all.isEmpty) { "There are no overwritten methods in class library".postln; ^this };
		("Overwritten methods in class library:".underlined ++ "\n\n").post;
		classes.do { |class|
			class.asString.underlined.postln;
			all.select { |x| x.ownerClass == class }.do { |x|
				var activePath = x.activePath;
				var overriddenPath = x.overriddenPath;
				if(simplifyPaths) {
					activePath = this.simplifyPath(x.activePath);
					overriddenPath = this.simplifyPath(x.overriddenPath);
				};
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
