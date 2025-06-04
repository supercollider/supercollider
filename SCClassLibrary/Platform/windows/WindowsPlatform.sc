WindowsPlatform : Platform {
	name { ^\windows }
	version { ^"[System.Environment]::OSVersion.Version".unixCmdGetStdOut.replace($\n, "") }

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

	killProcessByID { |pid, force = true, subprocesses = true|
		"taskkill % % /pid %".format(force.if({"/f"}, {""}), subprocesses.if({"/t"}, {""}), pid).unixCmd;
	}

	killAll { |cmdLineArgs, force = true|
		"taskkill % /im %".format(force.if({"/f"}, {""}), cmdLineArgs).unixCmd;
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
