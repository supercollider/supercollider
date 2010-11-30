WindowsPlatform : Platform
{
	name { ^\windows }
	startupFiles { ^["startup.sc", "~\\SuperCollider\\startup.sc".standardizePath] }	
	startup {
		// Server setup
		Server.program = "scsynth.exe";

		// Score setup
		Score.program = Server.program;

		// load user startup file
		this.loadStartupFiles;
	}
	
	defaultGUIScheme { ^\swing }
	defaultHIDScheme { ^nil }
	pathSeparator { ^$\\ }
	isPathSeparator { |char|
		^#[$\\, $/].includes(char)
	}
	clearMetadata { |path|
		path = path.splitext[0].do({ |chr, i| if(chr == $/) { path[i] = $\\.asAscii } });
		"del %%.*meta%".format(34.asAscii, path, 34.asAscii).systemCmd;
	}
}
