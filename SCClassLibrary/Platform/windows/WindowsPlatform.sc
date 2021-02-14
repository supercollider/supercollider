WindowsPlatform : Platform {
	name { ^\windows }
	startupFiles {
		var deprecated = ["startup.sc", "~\\SuperCollider\\startup.sc".standardizePath];
		Platform.deprecatedStartupFiles(deprecated);
		^(deprecated ++ super.startupFiles)
	}

	initPlatform {
		super.initPlatform;
		recordingsDir = this.myDocumentsDir +/+ "Recordings";
	}

	startup {
		// Server setup
		Server.program = "scsynth.exe";

		// Score setup
		Score.program = Server.program;

		// load user startup file
		this.loadStartupFiles;
	}

	pathSeparator { ^$\\ }
    pathDelimiter { ^$; }

	isPathSeparator { |char|
		^#[$\\, $/].includes(char)
	}
	clearMetadata { |path|
		path = path.splitext[0].do({ |chr, i| if(chr == $/) { path[i] = $\\.asAscii } });
		"del %%.*meta%".format(34.asAscii, path, 34.asAscii).systemCmd;
	}

	killProcessByID { |pid|
		("taskkill /F /pid " ++ pid).unixCmd;
	}

	killAll { |cmdLineArgs|
		("taskkill /F /IM " ++ cmdLineArgs).unixCmd;
	}

	defaultTempDir {
		// +/+ "" looks funny but ensures trailing slash
		var tmp = this.userAppSupportDir +/+ "";
		^if(File.exists(tmp)) { tmp }
	}

	myDocumentsDir {
		_WinPlatform_myDocumentsDir
		^this.primitiveFailed
	}

	formatPathForCmdLine { |path|
		^path.quote;
	}
}
