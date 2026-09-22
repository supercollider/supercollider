// This is how you should invoke this file.
// sclang -a --include-path SCClassLibrary/ --include-path testsuite/sclangErrorRegressions testsuite/sclangErrorRegressions/generateError.scd testsuite/sclangErrorRegressions/regression.txt


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

	stream
};


try {
	ErrorGenerator()
} { |er|
	ErrorWrapper(er, "ErrorGenerator was not defined. You haven't included the correct class library file").throw;
};

try {

	if (thisProcess.argv[0].notNil) {
		var str = action.(CollStream()).collection;
		str = str.split($\n).collect{ |line|
			line.replaceRegexp("(\\s*)[0-9]+ │", "│").replaceRegexp("(\\s*)file:/.*$", "file:/")
		}.join($\n);

		File.use(thisProcess.argv[0], "w", {|f|
			f << str
		});
	} {
		action.(Post)
	};

	0.exit;

} {
	1.exit;
}
