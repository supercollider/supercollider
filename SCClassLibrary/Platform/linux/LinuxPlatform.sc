LinuxPlatform : UnixPlatform {

	classvar <>runInTerminalCmd;

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
		// use "system" as default when env vars haven't been set by user
		if("SC_JACK_DEFAULT_INPUTS".getenv.isNil, {
			"SC_JACK_DEFAULT_INPUTS".setenv("system")
		});

		if("SC_JACK_DEFAULT_OUTPUTS".getenv.isNil, {
			"SC_JACK_DEFAULT_OUTPUTS".setenv("system")
		});

		// automatically start jack when booting the server
		// can still be overridden with JACK_NO_START_SERVER
		"JACK_START_SERVER".setenv("true");

		// load user startup file
		this.loadStartupFiles;
	}

	initPlatform {
		super.initPlatform;
		this.declareFeature(\unixPipes); // pipes are possible (can't declare in UnixPlatform since IPhonePlatform is unixy yet can't support pipes)
	}

	*getTerminalEmulatorCmd {
		"LinuxPlatform: searching for a supported terminal emulator".postln;
		[
			"urxvt -T % -e %",
			"termite --title % -e %",
			"rxvt -T % -e %",
			"terminator -T % -e %",
			"xterm -T % -e %",
			"kitty -T % -e %",
			"alacritty -t % -e %",
			// DE-specific terminals last: avoid problems if not on GNOME or KDE but term installed
			"xfce4-terminal -T % -e %",
			"mate-terminal -t % -e %",
			"gnome-terminal -t % -- %",
			"konsole --title % -e %",
		].do{ |cmd|
			if("command -v % > /dev/null".format(cmd.split($ ).first).systemCmd == 0) {
				^cmd
			};
		};
		^nil
	}
}
