+ String {
	runInTerminal {
		format("start \"SuperCollider runInTerminal\" cmd.exe /k %", this.quote).unixCmd;
	}

	openOS {
 		// start "title" "command"
     	if (PathName(this).isFolder) {
 			// workaround for folders, see https://github.com/supercollider/supercollider/issues/6312
 			// NB: Explorer.exe does not accept path names with forward slashes!
 			["cmd", "/c", "start", "SuperCollider".quote, "Explorer.exe", this.replace("/", "\\").quote].unixCmd;
 			} {
 			// files or URLs
			["cmd", "/c", "start", "SuperCollider".quote, this.quote].unixCmd;
     	}
	}
}
