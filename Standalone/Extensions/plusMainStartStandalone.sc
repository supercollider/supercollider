+ Main {

	// modify to do any standalone specific classlib startup
	startStandalone {
		var path;
		"Starting SuperCollider Standalone".postln;
		path = Platform.standaloneDir +/+ "startup.scd";
		if(File.exists(path), { path.load });
	}

}