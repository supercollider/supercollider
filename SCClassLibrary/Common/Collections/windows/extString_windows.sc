+ String {
	runInTerminal {
		format("start \"SuperCollider runInTerminal\" cmd.exe /k %", this.quote).unixCmd;
	}

	openOS {|aPath|
		// start "title" "command"
		("start" + "\"SuperCollider\"" + this.quote).systemCmd
	}
}
