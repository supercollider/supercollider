+Main {
	startup {
		super.startup;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.default;
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );
		this.platform.startup;
		"startup.sc".loadPaths;
		StartUp.run;
		(
			osx: "Welcome to SuperCollider, type cmd-d for help",
			linux: "Welcome to SuperCollider, type ctrl-c ctl-h for help from Emacs",
		 	windows: "Welcome to SuperCollider, press F1 for help"
		 ).at(platform.name).postln;
	}
}