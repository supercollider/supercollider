+ OSXPlatform {

	startupFiles {
//		var filename = "startup.rtf";
//		^[this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
		^[]
	}
	
	startup {
		Document.implementationClass.startup;
		// make server window - just the one used
		Server.default.makeWindow;
//		Server.internal.makeWindow;
//		Server.local.makeWindow;
//		this.loadStartupFiles;
		Help.addToMenu;
	}
}

+ Main { 
	startup {
		// setup the platform first so that class initializers can call platform methods.
		// create the platform, then intialize it so that initPlatform can call methods
		// that depend on thisProcess.platform methods.
		platform = this.platformClass.new;
		platform.initPlatform;
	
		super.startup;
		
		// set the 's' interpreter variable to the default server.
		GUI.fromID( this.platform.defaultGUIScheme );
		GeneralHID.fromID( this.platform.defaultHIDScheme );


		// from here on, you should customize what should happen...
		// choose just one server to use
		Server.default = Server.internal; 
		interpreter.s = Server.default;
		this.platform.startup;
		
		StartUp.run;

		// I like to boot, then use thisProcess.interpreter.executeFile
		// OR - put things into a class... like the SCSA_Demo

		"Welcome to Standalone ... made with SuperCollider, type cmd-d for help".postln;
		
		Server.default.boot;
		
		Server.default.waitForBoot({
			SCSA_Demo.new("The Cheese Stands Alone", Rect(400, 400, 300, 200), interpreter.s).front;
//			thisProcess.interpreter.executeFile(rootdir ++ "MFBSD.rtf");
		});
	}
}
