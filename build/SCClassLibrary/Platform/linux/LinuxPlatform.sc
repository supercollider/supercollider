LinuxPlatform : UnixPlatform
{
	name { ^\linux }
	startupFiles { ^#["~/.sclang.sc"] }
	startup {
		// Document setup
		if ( this.hasFeature( \emacs ) ) {
				Document.implementationClass.startup;
			};
		
		helpDir = this.systemAppSupportDir++"/Help";

		// Server setup
		Server.program = "scsynth";

		// Score setup
		Score.program = Server.program;

		// default jack port hookup
		"SC_JACK_DEFAULT_INPUTS".setenv("system:capture_1,system:capture_2");
		"SC_JACK_DEFAULT_OUTPUTS".setenv("system:playback_1,system:playback_2");

		// automatically start jack when booting the server
		// can still be overridden with JACK_NO_START_SERVER
		"JACK_START_SERVER".setenv("true");

		// load user startup file
		this.loadStartupFiles;
	}
	defaultHIDScheme { ^\linux_hid }

	recompile{
		if ( this.hasFeature( \emacs ) ) {
			Emacs.evalLispExpression( "(sclang-start)" );
		};		
	}
}
