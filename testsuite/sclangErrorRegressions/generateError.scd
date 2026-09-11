var action = {|stream|
	var catch = { |action|
		try {
			action.()
		} { |er|
			er.reportError(stream);
			stream << "\n";
		}
	};

	catch.value {
		ErrorGenerator.doesNotUnderstandErrorMethod
	};

	catch.value {
		ErrorGenerator.doesNotUnderstandErrorFunction
	};

	catch.value {
		ErrorGenerator.error
	};

	catch.value {
		ErrorGenerator.errorWrapper
	};


	catch.value {
		ErrorGenerator.mustBeBoolean
	};

	catch.value {
		ErrorGenerator.notYetImplementedError
	};

	catch.value {
		ErrorGenerator.immutable
	};

	catch.value {
		ErrorGenerator.subclassResponsibilityError
	};

	catch.value {
		ErrorGenerator.shouldNotImplementError
	};

	catch.value {
		ErrorGenerator.primitiveErr
	};
};


try {
	ErrorGenerator()
} { |er|
	ErrorWrapper(er, "ErrorGenerator was not defined. You haven't included the correct class library file").throw;
};

if (thisProcess.argv[0].notNil) {
	File.use(thisProcess.argv[0], "w", action);
} {
	action.(Post)
};

0.exit;
