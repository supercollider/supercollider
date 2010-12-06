UI {
	classvar resetActions, shutdownActions;

	initClass {
		resetActions = Array.new;
		shutdownActions = Array.new;
	}

	*shutdown {
		shutdownActions.do({ arg function; function.value });
	}
	*reset {
		resetActions.do({ arg function; function.value });
	}

	*registerForReset {
		arg function;
		resetActions = resetActions.add(function);
	}
	*registerForShutdown {
		arg function;
		shutdownActions = shutdownActions.add(function);
	}
}
