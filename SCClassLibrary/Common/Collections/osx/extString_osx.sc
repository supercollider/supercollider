+ String {
	// some things (e.g. SVN certificate acceptance, for Quarks) can only be achieved in an interactive prompt
	runInTerminal { | shell="/bin/bash" |
		var fpath, file;

		// At present we only know how to do this when /usr/bin/open exists, which unfortunately means OSX-only.
		if(File.exists("/usr/bin/open").not){
			"String:runInTerminal - /usr/bin/open not found, unable to run. Usually this means you're not using Mac OSX, which is at present the only platform which supports String:runInTerminal (sorry).".error;
		};

		fpath = PathName.tmp ++ "SCStringForTerminal" ++ (this.hash.asString) ++ ".command";
		file = File(fpath, "w");

		if(file.isOpen.not) {"String:runInTerminal was unable to create the script file".error} {
			file.write("#!"++shell++"\n"
					++this++"\n"
					++"rm"+fpath.quote+"\n");
			file.close;
			("/bin/chmod +x" + fpath).systemCmd;
			("/usr/bin/open -a Terminal" + fpath).systemCmd;
		};
	}

	openOS {
		("open " ++ this.escapeChar($ )).systemCmd
	}
}
