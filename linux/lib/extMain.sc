+ Main {
	startup {
		super.startup;

		// Server setup
		Server.program = "scsynth";
		Server.internal.options.numOutputBusChannels = 8;
		Server.internal.options.numInputBusChannels = 8;
		Server.internal.options.memSize = 8192;
		Server.local.options.numOutputBusChannels = 8;
		Server.local.options.numInputBusChannels = 8;
		Server.local.options.memSize = 8192;
		this.makeServerWindows;

		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;

		// Score setup
		Score.program = Server.program;

		// default jack port hookup
		"SC_JACK_DEFAULT_INPUTS".setenv("alsa_pcm:capture_1,alsa_pcm:capture_2");
		"SC_JACK_DEFAULT_OUTPUTS".setenv("alsa_pcm:playback_1,alsa_pcm:playback_2");

		// automatically start jack when booting the server
		// can still be overridden with JACK_NO_START_SERVER
		"JACK_START_SERVER".setenv("true");

		// load user startup file
		"~/.sclang.sc".loadPaths;
	}

	shutdown { // at recompile, quit
		Server.quitAll;
		super.shutdown;
	}
	
	newSCWindow {
	}

	makeServerWindows {
	}
}
