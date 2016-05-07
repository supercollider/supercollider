OSXPlatform : UnixPlatform {
	var <>preferencesAction; // Warning: scapp only
	var <>sleepAction, <>wakeAction, <>isSleeping=false;

	initPlatform {
		super.initPlatform;
		recordingsDir = "~/Music/SuperCollider Recordings".standardizePath;
		this.declareFeature(\unixPipes); // pipes are possible (can't declare in UnixPlatform since IPhonePlatform is unixy yet can't support pipes)
	}

	name { ^\osx }

	startupFiles {
		var filename = "startup.rtf";
		var deprecated = [this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
		Platform.deprecatedStartupFiles(deprecated);
		^(deprecated ++ super.startupFiles)
	}

	startup {
		Server.program = "exec %/scsynth".format((String.scDir +/+ "../Resources").shellQuote);

		Score.program = Server.program;

		if(Platform.ideName == "scapp") {
			Document.implementationClass.startup;
		};
		this.loadStartupFiles;
		if(Platform.ideName == "scapp") {
			// make server window
			Server.internal.makeWindow;
			Server.local.makeWindow;
		};
	}
	shutdown {
		if(Platform.ideName == "scapp"){
			CocoaMenuItem.clearCustomItems;
		};
	}

	defaultGUIScheme { ^\qt }

	findHelpFile { | string |
		^string.findHelpFile;
	}

	// for now just write syntax colours. Could be other things.
	writeClientCSS {
		var theme, file, string;
		theme = Document.theme;
		SCDoc.helpTargetDir.mkdir;
		file = File.open(SCDoc.helpTargetDir ++ "/frontend.css", "w");
		string = ".str { color: %; } /* strings */
.kwd { color: %; } /* special values like true, nil.. */
.com { color: %; } /* comments */
.typ { color: %; } /* class names */
.lit { color: %; } /* numbers and character literals */
.pun { color: %; } /* punctuation */
.pln { color: %; } /* plain text, methods and variable names */
.tag { color: %; } /* special variables like super, thisProcess... */
.dec { color: %; } /* declarations like var, const... */
.atn { color: %; } /* symbols */
.atv { color: %; } /* environment vars */".format(
			theme.stringColor.hexString,
			theme.specialValsColor.hexString,
			theme.commentColor.hexString,
			theme.classColor.hexString,
			theme.numberColor.hexString,
			theme.puncColor.hexString,
			theme.textColor.hexString,
			theme.specialVarsColor.hexString,
			theme.declColor.hexString,
			theme.symbolColor.hexString,
			theme.environColor.hexString
		);
		file.putString(string);
		file.close;
	}
}
