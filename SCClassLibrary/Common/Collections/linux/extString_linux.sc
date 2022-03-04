+ String {

	runInTerminal { |shell="/bin/sh"|
		var fpath = PathName.tmp +/+ "SCStringForTerminal%.sh".format(this.hash.asString);
		LinuxPlatform.runInTerminalCmd ?? {
			LinuxPlatform.runInTerminalCmd = LinuxPlatform.getTerminalEmulatorCmd
		};
		if(LinuxPlatform.runInTerminalCmd.isNil) {
			"String:runInTerminal couldn't find a supported terminal emulator. Please set LinuxPlatform.runInTerminalCmd, see Help for details".error;
			^this;
		} {
			File.use(fpath,"w"){|file|
				file.write([
					"#!" ++ shell,
					this,
					"rm" + fpath.shellQuote,
					"printf 'Press ENTER to close this window.'; read _"
				].join($\n));
			};
			("/bin/chmod +x" + fpath).systemCmd;
			LinuxPlatform.runInTerminalCmd.format("SuperCollider runInTerminal".shellQuote, fpath.shellQuote).unixCmd;
		}
	}

	openOS {
		["xdg-open", this].unixCmd;
	}
}
