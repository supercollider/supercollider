+ String {
	runInTerminal {
		"String:runInTerminal - sorry this method has not yet been implemented for windows.".error;
	}

	openOS {|aPath|
		("start " ++ this.escapeChar($ )).systemCmd
	}
}
