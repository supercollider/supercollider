LinuxPlatform : UnixPlatform
{
	name { ^\linux }
	startupFiles {
		var deprecated = #["~/.sclang.sc"];
		Platform.deprecatedStartupFiles(deprecated);
		^(deprecated ++ super.startupFiles)
	}
	startup {

		helpDir = this.systemAppSupportDir++"/Help";

		// Server setup
		Server.program = "exec scsynth";

		// Score setup
		Score.program = Server.program;

		// default jack port hookup
		"SC_JACK_DEFAULT_INPUTS".setenv("system");
		"SC_JACK_DEFAULT_OUTPUTS".setenv("system");

		// automatically start jack when booting the server
		// can still be overridden with JACK_NO_START_SERVER
		"JACK_START_SERVER".setenv("true");

		// load user startup file
		this.loadStartupFiles;
	}
	defaultHIDScheme { ^\linux_hid }

	initPlatform {
		super.initPlatform;
		this.declareFeature(\unixPipes); // pipes are possible (can't declare in UnixPlatform since IPhonePlatform is unixy yet can't support pipes)
	}
}
