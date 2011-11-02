+ String {
	runInTerminal {
		("xterm -hold -e" + this).systemCmd;
	}

	openOS {
		("xdg-open " ++ this.escapeChar($ )).systemCmd
	}
}
