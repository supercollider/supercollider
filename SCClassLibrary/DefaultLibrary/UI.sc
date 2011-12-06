UI {
	classvar resetActions, shutdownActions;

	initClass {
		resetActions = Array.new;
		shutdownActions = Array.new; // braindead extensions may rely on these
	}

	*registerForReset {|aFunction|
		this.deprecated(thisMethod, Reset.class.findMethod(\add));
		Reset.add(aFunction)
	}
	*registerForShutdown {|aFunction|
		this.deprecated(thisMethod, ShutDown.class.findMethod(\add));
		ShutDown.add(aFunction)
	}
}
