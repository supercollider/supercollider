+ OSXPlatform {

	startupFiles {
//		var filename = "startup.rtf";
//		^[this.systemAppSupportDir +/+ filename, this.userAppSupportDir +/+ filename];
			// look for startup files inside the app Contents directory
		var filename = "startup.*";
		^(Platform.resourceDir +/+ filename).pathMatch;
	}

	startup {
		openPorts = Set[NetAddr.langPort];
		Document.implementationClass.startup;
		// make a server window for the internal if you like
		Server.internal.makeWindow;
//		Server.local.makeWindow;
		// uncomment if you use a startup file
//		this.loadStartupFiles;
		// uncomment if you have multiple help files
//		Help.addToMenu;
	}
}

+ Main {
	startup {
		platform = this.platformClass.new;
		platform.initPlatform;

		super.startup;

		GUI.fromID( this.platform.defaultGUIScheme );

		// Set Server.default and the 's' interpreter variable to the internal server.
		// You should use the internal server for standalone applications --
		// otherwise, if your application has a problem, the user will
		// be stuck with a process, possibly making sound, that he won't know
		// how to kill.
		Server.default = Server.internal;
		interpreter.s = Server.default;

			// some folder paths that should point inside the app's Contents folder
		SynthDef.synthDefDir = Platform.resourceDir +/+ "synthdefs/";
		Archive.archiveDir = Platform.resourceDir;

		this.platform.startup;

		// from here on, you should customize what should happen...

		StartUp.run;

		// One can boot the server, then use .load to evaluate a file
		// OR - put things into a class... like the SCSA_Demo

		"Welcome to Standalone Demo made with SuperCollider, type cmd-d for help.".postln;

		Server.default.boot;

		Server.default.waitForBoot({
			SCSA_Demo.new("The Cheese Stands Alone", Rect(400, 400, 300, 200), interpreter.s).front;
//			(Platform.resourceDir.dirname ++ "/MFBSD.rtf").load;
		});
		// close post window if user should not have it
//		Document.listener.close
	}

}
