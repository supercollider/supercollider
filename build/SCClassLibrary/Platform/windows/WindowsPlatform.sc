WindowsPlatform : Platform
{
	name { ^\windows }
	startupFiles { ^#["startup.sc", "~\\SCStartup.sc"] }	
	startup {
		// Server setup
		Server.program = "scsynth.exe";

		// Score setup
		Score.program = Server.program;

		// load user startup file
		this.loadStartupFiles;
	}
}
