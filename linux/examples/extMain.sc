+ Main {
	startup {
		super.startup;
		// set the 's' interpreter variable to the default server.
		interpreter.s = Server.local;
		Server.local.options.numOutputBusChannels = 8;
		Server.local.options.numInputBusChannels = 8;
		Server.local.options.memSize = 8192;
	}

	shutdown { // at recompile, quit
		Server.quitAll;
		super.shutdown;
	}
	
	newSCWindow {
	}
}
