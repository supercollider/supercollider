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
		Server.program = (Platform.resourceDir +/+ "scsynth.exe").quote;

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
		// ensure trailing slash due to backwards compatibility
		var tmp = "TEMP".getenv +/+ "supercollider".withTrailingSlash;
		if(File.exists(tmp).not) { tmp.mkdir };
		^tmp;
	}

	myDocumentsDir {
		_WinPlatform_myDocumentsDir
		^this.primitiveFailed
	}

	formatPathForCmdLine { |path|
		^path.quote;
	}
}
