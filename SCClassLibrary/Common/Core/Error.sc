Exception {
	classvar <>handling = false;
	classvar <>reporting = false;
	classvar <>debug = false;
	classvar <>inProtectedFunction = false;


	var <>what; 
	var	<>callFrameAnnotations; 
	var <>methodBeforeBacktraceStart;
	var <>methodBacktraceEnd;
	
	var <backtrace;

	// This represents the nowExecutingPath, it doesn't represent what file the code was written in!
	// It is kept only for backwards compatibility and should no longer be used, instead use the backtrace.
	var <>path;

	*new { |what(""), callFrameAnnotations(#[]), methodBeforeBacktraceStart, methodBacktraceEnd|
		var thisThrow; // used to create the methodBeforeBacktraceStart

		if (Exception.reporting) {
			"Attempting to construct and error while reporting one. This is not allowed, please file a bug report.".error;
			this.halt; // Just quit the thread here, we might get stuck in an infinite loop.
		}; 

		^this.newCopyArgs(
			what: what.asString, 
			callFrameAnnotations: callFrameAnnotations,
			methodBeforeBacktraceStart: methodBeforeBacktraceStart ?? {
				thisThrow = this.class.findMethod(\throw);
				{ |method| method === thisThrow or: { method === thisThrow } }
			},
			methodBacktraceEnd: methodBacktraceEnd ?? {
				// Skip all the interpreter stuff, that isn't useful for this error (or if it is, there is an issue in the class library).
				{ |method| method.ownerClass === Interpreter or: {method.ownerClass == Function and: { method.name === 'protect' or: {method.name == 'try'} or: {method.name == 'prTry'} }} }
			},
			path: thisProcess.nowExecutingPath // backwards compatible.
		)
	}

	throw {
		backtrace ?? { backtrace = this.getBackTrace };
		super.throw;
	}

	
	reportStage1 { |stream, prefix| 
		backtrace.backtracePrintOnto(
			stream, 
			prefix: prefix,
			callFrameAnnotations: callFrameAnnotations,
			methodBeforeBacktraceStart: methodBeforeBacktraceStart,
			methodBacktraceEnd: methodBacktraceEnd,
			maxVerboseFrames: 3,
		) 
	}

	reportStage2 { |stream, prefix| 
		stream << prefix << what << $\n;
	} 

	// Just does what.error. 
	// This probably shouldn't be used anymore.
	errorString { ^what.error }

	// Do not override this! Instead use reportStage1 and reportStage2.
	// To change where we print (to a file perhaps?) set the stream argument.
	// The prefix can be used to set the indentation if this is used as a part of some other text.
	reportError { |stream(Post), prefix("")|
		var oldReporting = Exception.reporting;

		Exception.reporting = true;

		stream << prefix << "────────────────────────────────────────────────────────────────────────────────\n";
		stream << prefix << "ERROR: " << this.what << $\n;
		this.reportStage1(stream, prefix);
		stream << $\n << prefix;

		this.reportStage2(stream, prefix);

		stream << $\n << prefix << "────────────────────────────────────────────────────────────────────────────────\n";

		Exception.reporting = oldReporting;
		^stream;
	}

	// in the class library it is equivalent to x.isKindOf(Exception), so this is useless, don't use.
	isException { ^true }
}

Error : Exception { }

// Is used to wrap an existing error, very useful when you want to append more information to an error inside a try catch, and then rethrow it.
ErrorWrapper : Error {
	var <>wrapped;

	*new { |wrapped, what, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd|
		^super
			.new(what, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd)
			.wrapped_(wrapped)
	}

	reportStage1 { |stream, prefix|
		super.reportStage2(stream, prefix)
	}

	reportStage2{ |stream, prefix|
		stream << prefix << "Wrapped error: " << what << $\n;
		wrapped.reportStage1(stream, prefix ++ "     ");
		wrapped.reportStage2(stream, prefix ++ "     ");
		stream << prefix << what;
		super.reportStage1(stream, prefix)
	}
}

MethodError : Error {
	var <>receiver;

	*new { |what, receiver, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd| 
		^super.new(what, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd).receiver_(receiver)
	}

	reportStage2 { |stream, prefix|
		stream << prefix << what << $\n;
	}
}

PrimitiveFailedError : MethodError {
	var <>failedPrimitiveName;

	*new { |receiver, failedPrimitive(thisThread.failedPrimitiveName), errorString(Thread.primitiveErrorString)|
		var thisThrow = this.class.findMethod(\new);
		^super.new(
			what: errorString 
				!? { "Primitive '%' failed with message : '%'.".format(failedPrimitive, errorString) }
				?? { "Primitive '%' failed.".format(failedPrimitive) },
			receiver: receiver,
			callFrameAnnotations: [nil, errorString],
			methodBeforeBacktraceStart: { |m| m === thisThrow or: { m.ownerClass === Object and: { m.name === 'primitiveFailed' } } }
		)
	}
}

SubclassResponsibilityError : MethodError {
	var <>method; 
	var <>class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		var thisThrow = this.class.findMethod(\new);
		^super.new(
			what: "'%' should have been implemented by %.".format(method.name, class.name), 
			receiver: receiver,
			callFrameAnnotations: [nil, "Please implement this method for the class '%'".format(class.name)],
			methodBeforeBacktraceStart: { |m| 
				m === thisThrow or: {m.ownerClass === Object and: {m.name === 'subclassResponsibility'}} 
			}
		)
			.method_(method)
			.class_(class)
	}
}

ShouldNotImplementError : MethodError {
	var <>method; 
	var <>class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		var thisThrow = this.class.findMethod(\new);
		^super.new(
			what: "'%-%' is not a valid message for the subclass '%'".format(method.ownerClass.name, method.name, class.name), 
			callFrameAnnotations: [nil, "'%' cannot respond to this message, please remove the call.".format(class.name)],
			receiver: receiver, 
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'shouldNotImplement'}} },
		)
			.method_(method)
			.class_(class)
	}
}

DoesNotUnderstandError : MethodError {
	var <>selector; 
	var <>args; 
	var <>keywordArgumentPairs;

	*new { |receiver, selector, args([]), keywordArgumentPairs([])|
		var thisThrow = this.class.findMethod(\new);
		var msg = "% does not understand the message '%'.".format(receiver.class.name, selector);

		// Note: is it okay to throw in the constructor of an exception, but not in reportError
		selector ?? { Error("'selector' was nil in DoesNotUnderstandError.new").throw };

		^super.new(
			what: msg,
			callFrameAnnotations: [msg],
			// We don't need to print Object.doesNotUnderstand.
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'doesNotUnderstand'}} },
			receiver: receiver
		)
			.selector_(selector.asSymbol)
			.args_(args)
			.keywordArgumentPairs_(keywordArgumentPairs)
	}

	reportStage2 { |stream, prefix| 
		var methodSuggestions = receiver.class.findSimilarSelectors(selector, minSimilarity: 0.5, maxEditDistance: 2);
		var classSuggestions = Object.findRespondingUpperSubclasses(selector).collect(_.name);
		if(methodSuggestions.notEmpty) {
			stream << "Message% with a similar name understood by the receiver:".format( if(methodSuggestions.size > 1) { "s" } { "" } );
			stream << "\n" << prefix << "  ";
			stream << methodSuggestions.join("\n" ++ prefix ++ "  ");
		};
		if(classSuggestions.notEmpty) {
			if(classSuggestions.size < 8) {
				stream << "\n" << prefix << "Objects which respond to the selector '%' derive from:".format(selector);
				stream << "\n" << prefix << "  ";
				stream << classSuggestions.join("\n" ++ prefix ++ "  ");
			} {
				stream << "\n" << prefix << "Many other objects respond to the message '%' (found % superclasses).".format(selector, classSuggestions.size);
			}
		}
	}
}


MustBeBooleanError : MethodError {
	*new { |what, receiver| 
		var thisThrow = this.class.findMethod(\throw);
		^super.new( 
			what: what ?? { "Non boolean in test "}, 
			receiver: receiver,
			callFrameAnnotations: ["Expected this to be either `true` or `false`, instead got %(%).".format(receiver.class, receiver)],
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'mustBeBoolean'}} },
		) 
	}
}

NotYetImplementedError : MethodError {
	*new { |what, receiver| 
		var thisThrow = this.class.findMethod(\throw);
		^super.new( 
			what: what ?? {"Not yet implemented"}, 
			callFrameAnnotations: ["This method has not yet been implemented."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'notYetImplemented'}} },
		) 
	}

 }

OutOfContextReturnError : MethodError {
	var <>method, <>result;
	*new { |receiver, method, result|
		var thisThrow = this.class.findMethod(\throw);
		if (method.isKindOf(Method).not) {
			Error("OutOfContextReturnError excepts a method").throw
		};
		^super.new(
			what: "'%-%' tried to return to a call frame that has expired with a value of: %".format(method.ownerClass),
			callFrameAnnotations: ["Could not complete this return as the parent method is no longer active."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'outOfContextReturn'}} },
		)
			.method_(method) 
			.result_(result)
	}
}

ImmutableError : MethodError {
	var <>value;
	*new { |receiver, value|
		var thisThrow = this.class.findMethod(\throw);
		^super.new(
			what: "Cannot mutate an immutable object",
			callFrameAnnotations: ["Make a copy of this object before mutating it."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m.ownerClass === Object and: {m.name === 'immutableError'}} },
		)
			.value_(value)
	}
}

// This doesn't need to exist.
BinaryOpFailureError : DoesNotUnderstandError { }

DeprecatedError : MethodError {
	var <>method, <>alternateMethod;

	*new { |receiver, method, alternateMethod|
		var thisThrow = this.class.findMethod(\throw);
		^super.new(
			what: "The method '%-%' is deprecated, instead use '%-%'.".format(method.ownerClass.name, method.name, alternateMethod.ownerClass.name, alternateMethod.name),
			callFrameAnnotations: ["Replace this with '*.%".format(alternateMethod.name)],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisThrow or: {m == method} },
		)
			.method_(method)
			.alternateMethod_(alternateMethod)
	}

	// This disables throwing when not in debug mode, but will halt when in debug.
	// This means that DeprecatedErrors are *not* exceptions, despite inheriting from Exception.
	throw {
		this.reportError;
		if (Error.debug) { this.halt }
	}
}
