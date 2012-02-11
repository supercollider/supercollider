+ String {
	runInTerminal {|shell="sh"|
		("xterm -hold -T 'SuperCollider runInTerminal' -e" + shell + "-c" + this.shellQuote).unixCmd;
	}

	openOS {
		("xdg-open " ++ this.escapeChar($ )).systemCmd
	}
}
