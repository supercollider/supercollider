+ String {
	runInTerminal {
		format("start \"SuperCollider runInTerminal\" cmd.exe /k %", this.quote).unixCmd;
	}

	openOS {
		// start "title" "command"
		["start", "SuperCollider".quote, this.quote].unixCmd;
	}
}
