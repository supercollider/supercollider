UI {
	classvar resetActions, shutdownActions;

	initClass {
		resetActions = Array.new;
		shutdownActions = Array.new;
	}

	*reset {
		resetActions.do({ arg function; function.value });
	}

	*registerForReset {
		arg function;
		resetActions = resetActions.add(function);
	}
	*registerForShutdown {|aFunction|
		this.deprecated(thisMethod, ShutDown.class.findMethod(\add));
		ShutDown.add(aFunction)
	}
}
