Exception {
	classvar <>handling = false;
	classvar <>reporting = false;
	classvar <>debug = false;
	classvar <>inProtectedFunction = false;

	// Due to backwards compatibility we cannot make these members private...
	// This means any one who sets these must ensure they follow the expected type.
	// This is because when creating an error, we are not always allowed to throw another.

	// A String.
	// Describes the error.
	var <>what; 

	// An Array of (Strings or Nil).
	// Text that is printed next to the backtrace depending on its index, if out of range of the array (or nil), nothing is printed.
	var	<>callFrameAnnotations; 

	// A function accepting a Method that returns a Boolean.
	// A predicate deciding if the method at the bottom of the backtrace should be printed.
	// Typically used to ignore methods like the constructor of this error, and Object.doesNotUnderstand.
	var <>methodBeforeBacktraceStart;

	// A function accepting a Method that returns a Boolean.
	// A predicate deciding if we have reached the end of the backtrace we wish to print.
	// Typically used to ignore all the interpreter stuff, or the stuff above a try/protect block.
	var <>methodBacktraceEnd; // Function[Method -> Boolean]
	
	// Set when the exception is created, shows the call stack that led to this point.
	var <backtrace;

	// This represents the nowExecutingPath, it doesn't represent what file the code was written in!
	// It is kept only for backwards compatibility and should no longer be used, instead use the backtrace.
	var <>path;

	*new { |what(""), callFrameAnnotations([]), methodBeforeBacktraceStart, methodBacktraceEnd|
		var thisConstructor; // used to create the methodBeforeBacktraceStart

		if (Exception.reporting) {
			"Attempting to construct and error while reporting one. This is not allowed, please file a bug report.".error;
			this.halt; // Just quit the thread here, we might get stuck in an infinite loop.
		}; 

		^this.newCopyArgs(
			what: what.asString, 
			callFrameAnnotations: [],
			backtrace: this.getBackTrace,
			methodBeforeBacktraceStart: methodBeforeBacktraceStart ?? {
				thisConstructor = this.class.findMethod(\new);
				{ |method| method === thisConstructor }
			},
			methodBacktraceEnd: methodBacktraceEnd ?? {
				// Skip all the interpreter stuff, that isn't useful for this error (or if it is, there is an issue in the class library).
				{ |method| method.ownerClass === Interpreter or: {method.ownerClass == Function and: { method.name === 'protect' or: {method.name == 'try'} }} }
			},
			path: thisProcess.nowExecutingPath // backwards compatible.
		)
	}

	
	// These two report methods allow subclasses to inject their own printing behavior
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
	reportStage2 { |stream, prefix| } 

	// Just does what.error. 
	// This probably shouldn't be used anymore.
	errorString { ^what.error }

	// Do not override this! Instead use reportStage1 and reportStage2.
	// To change where we print (to a file perhaps?) set the stream argument.
	// The prefix can be used to set the indentation if this is used as a part of some other text.
	reportError { |stream(Post), prefix("")|
		var oldReporting = Exception.reporting;

		Exception.reporting = true;

		stream << prefix << "──────────────────────────────────────────────────────────────────────────────────\n";
		stream << prefix << "ERROR: " << this.what << "\n";
		this.reportStage1(stream, prefix);
		stream << "\n" << prefix;

		this.reportStage2(stream, prefix);

		stream << "\n" << prefix << "──────────────────────────────────────────────────────────────────────────────────\n";

		Exception.reporting = oldReporting;
		^stream;
	}

	// in the class library it is equivalent to x.isKindOf(Exception), so this is useless, don't use.
	isException { ^true }
}

Error : Exception { }

// Is used to wrap an existing error, very useful when you want to append more information to an error inside a try catch, and then rethrow it.
ErrorWrapper : Exception {
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
		stream << prefix << "Wrapped error:";
		wrapped.reportStage1(stream, prefix ++ "     ");
		wrapped.reportStage2(stream, prefix ++ "     ");
		stream << prefix << "This error:";
		super.reportStage1(stream, prefix)
	}
}

MethodError : Error {
	var <>receiver;

	*new { |what, receiver, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd| 
		^super.new(what, callFrameAnnotations, methodBeforeBacktraceStart, methodBacktraceEnd).receiver_(receiver)
	}

	reportStage2 { |stream, prefix|
		stream << "RECEIVER: " << receiver.class.name << $\n;
	}
}

PrimitiveFailedError : MethodError {
	var <>failedPrimitiveName;

	*new { |receiver, failedPrimitive(thisThread.failedPrimitiveName), errorString(Thread.primitiveErrorString)|
		var thisConstructor = this.class.findMethod(\new);
		^super.new(
			what: errorString 
				!? { "Primitive '%' failed with message : '%'.".format(failedPrimitive, errorString) }
				?? { "Primitive '%' failed.".format(failedPrimitive) },
			receiver: receiver,
			callFrameAnnotations: [nil, errorString],
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: { m.ownerClass === Object and: { m.name === 'primitiveFailed' } } }
		)
	}
}

SubclassResponsibilityError : MethodError {
	var <>method, <>class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		var thisConstructor = this.class.findMethod(\new);
		^super.new(
			what: "'%' should have been implemented by %.".format(method.name, class.name), 
			receiver: receiver,
			callFrameAnnotations: [nil, "Please implement this method for the class '%'".format(class.name)],
			methodBeforeBacktraceStart: { |m| 
				m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'subclassResponsibility'}} 
			}
		)
			.method_(method)
			.class_(class)
	}
}

ShouldNotImplementError : MethodError {
	var <>method, <>class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		var thisConstructor = this.class.findMethod(\new);
		^super.new(
			what: "'%-%' is not a valid message for the subclass '%'".format(method.ownerClass.name, method.name, class.name), 
			callFrameAnnotations: [nil, "'%' cannot respond to this message, please remove the call.".format(class.name)],
			receiver: receiver, 
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'shouldNotImplement'}} },
		)
			.method_(method)
			.class_(class)
	}
}

DoesNotUnderstandError : MethodError {
	var <>selector, <>args, <>keywordArgumentPairs;

	*new { |receiver, selector, args([]), keywordArgumentPairs([])|
		var thisConstructor = this.class.findMethod(\new);
		var msg = "% does not understand the message '%'.".format(receiver.class.name, selector);

		// Note: is it okay to throw in the constructor of an exception, but not in reportError
		selector ?? { Error("'selector' was nil in DoesNotUnderstandError.new").throw };

		^super.new(
			what: msg,
			callFrameAnnotations: [msg],
			// We don't need to print Object.doesNotUnderstand.
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'doesNotUnderstand'}} },
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
	*new { |receiver| 
		var thisConstructor = this.class.findMethod(\new);
		^super.new( 
			what: "Non boolean in test ", 
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'mustBeBoolean'}} },
		) 
	}
}

NotYetImplementedError : MethodError {
	*new { |receiver| 
		var thisConstructor = this.class.findMethod(\new);
		^super.new( 
			what: "Not yet implemented", 
			callFrameAnnotations: ["This method has not yet been implemented."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'notYetImplemented'}} },
		) 
	}

 }

OutOfContextReturnError : MethodError {
	var <>method, <>result;
	*new { |receiver, method, result|
		var thisConstructor = this.class.findMethod(\new);
		if (method.isKindOf(Method).not) {
			Error("OutOfContextReturnError excepts a method").throw
		};
		^super.new(
			what: "'%-%' tried to return to a call frame that has expired with a value of: %".format(method.ownerClass),
			callFrameAnnotations: ["Could not complete this return as the parent method is no longer active."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'outOfContextReturn'}} },
		)
			.method_(method) 
			.result_(result)
	}
}

ImmutableError : MethodError {
	var <>value;
	*new { |receiver, value|
		var thisConstructor = this.class.findMethod(\new);
		^super.new(
			what: "Cannot mutate an immutable object",
			callFrameAnnotations: ["Make a copy of this object before mutating it."],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m.ownerClass === Object and: {m.name === 'immutableError'}} },
		)
			.value_(value)
	}
}

// This doesn't need to exist.
BinaryOpFailureError : DoesNotUnderstandError { }

DeprecatedError : MethodError {
	var <>method, <>alternateMethod;

	*new { |receiver, method, alternateMethod|
		var thisConstructor = this.class.findMethod(\new);
		^super.new(
			what: "The method '%-%' is deprecated, instead use '%-%'.".format(method.ownerClass.name, method.name, alternateMethod.ownerClass.name, alternateMethod.name),
			callFrameAnnotations: ["Replace this with '*.%".format(alternateMethod.name)],
			receiver: receiver,
			methodBeforeBacktraceStart: { |m| m === thisConstructor or: {m == method} },
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
