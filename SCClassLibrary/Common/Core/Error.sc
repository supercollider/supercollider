Exception {
	classvar <>handling = false;
	classvar <>debug = false;
	classvar <>inProtectedFunction = false;

	var <>what, <>protectedBacktrace, <>path;

	*new { arg what;
		var protectedBacktrace, instance;
		if (debug || inProtectedFunction, {
			protectedBacktrace = this.getBackTrace;
			inProtectedFunction = false;
		});
		^super.newCopyArgs(what ? this.name, protectedBacktrace, thisProcess.nowExecutingPath);
	}
	errorString {
		^"EXCEPTION: " ++ what
	}
	reportError {
		this.errorString.postln;
		if(protectedBacktrace.notNil, { this.postProtectedBacktrace });
		DebugFramePrinter(this.getBackTrace, "  ").postln;
		// this.adviceLink.postln;
		"^^ The preceding error dump is for %\n".postf(this.errorString);
	}
	adviceLink {
		^("For advice: [http://supercollider.sf.net/wiki/index.php/%]"
			.format(this.adviceLinkPage));
	}
	adviceLinkPage {
		^this.errorString.tr($ , $_).tr($\n, $_);
	}

	postProtectedBacktrace {
		"\nPROTECTED CALL STACK:".postln;
		DebugFramePrinter(protectedBacktrace, "  ").postln;
	}

	isException { ^true }
}

Error : Exception {
	errorString {
		^"ERROR: " ++ what
	}
	errorPathString {
		^if(path.isNil) { "" } { "PATH:" + path ++ "\n" }
	}
}

MethodError : Error {
	var <>receiver, <>realBackTrace;

	*new { arg what, receiver;
		^super.new(what)
		.receiver_(receiver)
		.realBackTrace_(this.getBackTrace) // skip this frame and this call sight.
	}
	reportError {
		this.errorString.postln;
		"RECEIVER:\n".post;
		receiver.dump;
		this.errorPathString.post;
		if(protectedBacktrace.notNil, { this.postProtectedBacktrace });
		ErrorPrinter.printDebugStack(realBackTrace).postln;
		// this.adviceLink.postln;
		"^^ The preceding error dump is for %\nRECEIVER: %\n\n\n".postf(this.errorString, receiver);
	}
	adviceLinkPage {
		^this.class.name
	}

}

PrimitiveFailedError : MethodError {
	var <>failedPrimitiveName;

	*new { arg receiver;
		^super.new(Thread.primitiveErrorString, receiver)
		.failedPrimitiveName_(thisThread.failedPrimitiveName)
	}
	errorString {
		^"ERROR: Primitive '%' failed.\n%".format(failedPrimitiveName, what ? "")
	}
}

SubclassResponsibilityError : MethodError {
	var <>method, <>class;
	*new { arg receiver, method, class;
		^super.new(nil, receiver).method_(method).class_(class)
	}
	errorString {
		^"ERROR: '" ++ method.name ++ "' should have been implemented by " ++ class.name ++ "."
	}
}

ShouldNotImplementError : MethodError {
	var <>method, <>class;
	*new { arg receiver, method, class;
		^super.new(nil, receiver).method_(method).class_(class)
	}
	errorString {
		^"ERROR: '" ++ method.ownerClass.name ++ "-" ++ method.name
		++ "' Message not valid for this subclass: " ++ class.name ++ "."
	}
}

DoesNotUnderstandError : MethodError {
	var <>selector, <>args, <>keywordArgumentPairs;
	*new { |receiver, selector, args, keywordArgumentPairs|
		^super.new(nil, receiver)
		.selector_(selector)
		.args_(args)
		.keywordArgumentPairs_(keywordArgumentPairs)
	}

	errorString { ^"ERROR: '%' did not understand the message '%'".format(receiver, selector) }

	reportError {
		"ERROR: a DoesNotUnderstand error has occurred.".postln;
		"% did not understand the message '%'".format(receiver, selector).postln;
		"\nCall Stack".postln;
		"────────────".postln;
		// skip the creation of the error
		ErrorPrinter.printDebugStack(
			realBackTrace,
			indentString: "  ",
			opts: ErrorPrinterOptions.defaultCopy
			    .defToBeginAt_(Meta_DoesNotUnderstandError.findMethod(\new))
			    .defToStopAt_(Interpreter.findMethod(\interpretPrintCmdLine))
		).collection.postln;

		this.errorString.postln;
		this.prSuggestion.postln;

		"\n".post;
	}

	prSuggestion {
		var methodSuggestions, classSuggestions, plural, suggestion = "";
		if(selector.isNil){
			"DoesNotUnderstandError selector for % was nil".format(receiver).warn;
			^suggestion
		};

		methodSuggestions = receiver.class.findSimilarSelectors(selector, minSimilarity: 0.5, maxEditDistance: 2);
		if(methodSuggestions.notEmpty) {
			plural = if(methodSuggestions.size > 1) { "s" } { "" };
			methodSuggestions = methodSuggestions.join("\n\t");
			suggestion = suggestion ++
			"\nMessage% with a similar name understood by the receiver:\n\t%\n".format(plural, methodSuggestions);
		};
		classSuggestions = Object.findRespondingUpperSubclasses(selector).collect(_.name);
		if(classSuggestions.notEmpty) {
			if(classSuggestions.size < 8) {
				classSuggestions = classSuggestions.join("\n\t");
				suggestion = suggestion ++
				"\nObjects which respond to the selector '%' derive from:\n\t%"
				.format(selector, classSuggestions)
			} {
				suggestion = suggestion ++
				"\nMany other objects respond to the message '%' (found % superclasses)."
				.format(selector, classSuggestions.size)
			}
		};
		^suggestion
	}

	adviceLinkPage {
		^"%#%".format(this.class.name, selector)
	}
}


MustBeBooleanError : MethodError {
	errorString {
		^"ERROR: Non Boolean in test."
	}
}

NotYetImplementedError : MethodError {
}

OutOfContextReturnError : MethodError {
	var <>method, <>result;
	*new { arg receiver, method, result;
		^super.new(nil, receiver).method_(method).result_(result)
	}
	errorString {
		^"ERROR: '" ++ method.ownerClass.name ++ "-" ++ method.name
		++ "' Out of context return of value: " ++ result
	}
}

ImmutableError : MethodError {
	var <>value;
	*new { arg receiver, value;
		^super.new(nil, receiver).value_(value)
	}
	errorString {
		^"ERROR: Object is immutable: " ++ receiver
	}
}

BinaryOpFailureError : DoesNotUnderstandError {
	errorString {
		^"ERROR: binary operator '" ++ selector ++ "' failed."
	}
}

DeprecatedError : MethodError {
	var <>method, <>class, <>alternateMethod;

	*new { arg receiver, method, alternateMethod, class;
		^super.new(nil).receiver_(receiver).method_(method).class_(class).alternateMethod_(alternateMethod)
	}
	errorString {
		var methodSignature = { arg m;
			m.ownerClass.name.asString  ++ ":" ++ m.name;
		};
		var searchForCaller = { arg backtrace, m;
			while {
				backtrace.notNil and: {
					backtrace.functionDef !== m
				}
			} {
				backtrace = backtrace.caller;
			};
			// backtrace.caller may now be a FunctionDef,
			// useless for troubleshooting
			// so roll back to the last real method
			while {
				backtrace.notNil and: {
					backtrace = backtrace.caller;
					backtrace.functionDef.isKindOf(Method).not
				}
			};
			if(backtrace.notNil) { backtrace.tryPerform(\functionDef) };
		};
		var caller, string;
		if(protectedBacktrace.notNil) {
			caller = searchForCaller.value(protectedBacktrace, method);
		};
		if(caller.isNil) {
			caller = searchForCaller.value(this.getBackTrace, method);
		};
		if(caller.isNil) {
			caller = "{unknown}"
		} {
			if(caller.isKindOf(Method)) {
				caller = methodSignature.value(caller);
			} {
				caller = caller.asString;
			};
		};
		string = "WARNING: Called from %, method % is deprecated and will be removed.".format(
			caller,
			methodSignature.value(method)
		);
		if(alternateMethod.notNil) {
			string = string + "Use" + methodSignature.value(alternateMethod) + "instead.";
		};

		string = string ++ "\nThe definition of '%' is to be found here: '%'".format(method, method.filenameSymbol);

		^string
	}

	reportError {
		this.errorString.postln;
		this.errorPathString.post;
		// this.adviceLink.postln;
		"\n\n".post;
	}

	throw {
		Error.handling = true;
		this.reportError;
		if (Error.debug) {
			if(protectedBacktrace.notNil, { this.postProtectedBacktrace });
			this.getBackTrace.createBacktraceString.postln;
			Error.handling = false;
			this.halt;
		} {
			Error.handling = false;
		};

	}
	adviceLinkPage {
		^"DeprecatedError"
	}
}
