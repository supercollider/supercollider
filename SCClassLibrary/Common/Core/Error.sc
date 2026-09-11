Exception {
	classvar <>handling = false;
	classvar <>reporting = false;
	classvar <>debug = false;
	classvar <>inProtectedFunction = false;

	var <what, <callFrameAnnotations, <backtrace, <oneBeforeBeginMethod, <endMethod;
	// path should not be needed anymore because what you really want is where the source code was defined.
	var <>path;

	// This is designed to be called with keyword arguments and will pass all of them to newCopyArgs.
	*new { |what("") ... args, kwargs|
		var thisConstructor;
		if (Exception.reporting) {
			"Attempting to construct and error while reporting one. This is not allowed, please file a bug report.".error;
			this.halt; // Just quit the thread here.
		};
		thisConstructor = this.class.findMethod(\new);
		^this.superPerformArgs(
			\newCopyArgs, 
			[], 
			[
				what: what, 
				callFrameAnnotations: #[],
				backtrace: this.getBackTrace,
				// Skips the creation of this class in the back trace.
				oneBeforeBeginMethod: { |method| method === thisConstructor },
				// Skip all the interpreter stuff, that isn't useful for this error (or if it is, there is an issue in the class library).
				endMethod: { |method| method.ownerClass === Interpreter or: {method.ownerClass == Function and: { method.name === 'protect' or: {method.name == 'try'} }} },
				path: thisProcess.nowExecutingPath
			] ++ kwargs
		)
	}

	// Subclasses can override these two methods:
	prReportStage1 { |stream, prefix| 
		backtrace.backtracePrintOnto(
			stream, 
			prefix: prefix,
			callFrameAnnotations: callFrameAnnotations,
			oneBeforeBeginMethod: oneBeforeBeginMethod,
			endMethod: endMethod,
			maxVerboseFrames: 3,
		) 
	}
	prReportStage2 { |stream, prefix| } 

	// Just does what.error. 
	errorString { ^what.error }

	reportError { |stream(Post), prefix("")|
		var oldReporting = Exception.reporting;

		Exception.reporting = true;

		stream << prefix << "──────────────────────────────────────────────────────────────────────────────────\n";
		stream << prefix << "ERROR: " << this.what << "\n";
		this.prReportStage1(stream, prefix);
		stream << "\n" << prefix;

		this.prReportStage2(stream, prefix);

		stream << "\n" << prefix << "──────────────────────────────────────────────────────────────────────────────────\n";

		Exception.reporting = oldReporting;
		^stream;
	}

	// TODO: when we can throw other objects this is useless (in the class library it is equivalent to x.isKindOf(Exception)).
	isException { ^true }
}

Error : Exception { }

ErrorWrapper : Exception {
	var wrapped;

	*new { |wrapped, what|
		^super.new(what: what, wrapped: wrapped)
	}

	prReportStage1 { |stream, prefix|
		super.prReportStage2(stream, prefix)
	}

	prReportStage2{ |stream, prefix|
		stream << prefix << "Wrapped error:";
		wrapped.prReportStage1(stream, prefix ++ "     ");
		wrapped.prReportStage2(stream, prefix ++ "     ");
		stream << prefix << "This error:";
		super.prReportStage1(stream, prefix)
	}

}

MethodError : Error {
	var <receiver;

	*new { |what, receiver...args, kwargs| 
		^this.superPerformArgs(\new, [], kwargs ++ [what: what, receiver: receiver])
	}

	prReportStage2 { |stream, prefix|
		stream << "RECEIVER: " << receiver.class.name << $\n;
	}
}

PrimitiveFailedError : MethodError {
	var <failedPrimitiveName;

	*new { |receiver, failedPrimitive(thisThread.failedPrimitiveName), errorString(Thread.primitiveErrorString)|
		^super.new(
			what: errorString 
				!? { "Primitive '%' failed with message : '%'.".format(failedPrimitive, errorString) }
				?? { "Primitive '%' failed.".format(failedPrimitive) },
			callFrameAnnotations: [nil, errorString],
			receiver: receiver,
			failedPrimitiveName: failedPrimitive,
			oneBeforeBeginMethod: { |m| m === PrimitiveFailedError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'primitiveFailed'}} },
		)
	}
}

SubclassResponsibilityError : MethodError {
	var <method, <class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		^super.new(
			what: "'%' should have been implemented by %.".format(method.name, class.name), 
			callFrameAnnotations: [nil, "Please implement this method for the class '%'".format(class.name)],
			receiver: receiver, 
			oneBeforeBeginMethod: { |m| m === SubclassResponsibilityError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'subclassResponsibility'}} },
			method: method,
			class: class
		)
	}
}

ShouldNotImplementError : MethodError {
	var <method, <class;

	*new { |receiver, method(thisMethod), class(SubclassResponsibilityError)|
		^super.new(
			what: "'%-%' is not a valid message for the subclass '%'".format(method.ownerClass.name, method.name, class.name), 
			callFrameAnnotations: [nil, "'%' cannot respond to this message, please remove the call.".format(class.name)],
			receiver: receiver, 
			method: method,
			oneBeforeBeginMethod: { |m| m === ShouldNotImplementError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'shouldNotImplement'}} },
			class: class
		)
	}
}

DoesNotUnderstandError : MethodError {
	var <selector, <args, <keywordArgumentPairs;

	*new { |receiver, selector, args([]), keywordArgumentPairs([])|
		var msg = "% does not understand the message '%'.".format(receiver.class.name, selector);
		// Note: is it okay to throw in the constructor of an exception, but not when reporting it.
		selector ?? { Error("'selector' was nil in DoesNotUnderstandError.new").throw };
		^super.new(
			what: msg,
			callFrameAnnotations: [msg],
			selector: selector.asSymbol,
			// We don't need to print Object.doesNotUnderstand.
			oneBeforeBeginMethod: { |m| m === DoesNotUnderstandError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'doesNotUnderstand'}} },
			receiver: receiver,
			args: args,
			keywordArgumentPairs: keywordArgumentPairs
		);
	}

	prReportStage2 { |stream, prefix| 
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
		^super.new( 
			what: "Non boolean in test ", 
			receiver: receiver,
			oneBeforeBeginMethod: { |m| m === MustMeBooleanError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'mustBeBoolean'}} },
		) 
	}
}

NotYetImplementedError : MethodError {
	*new { |receiver| 
		^super.new( 
			what: "Not yet implemented", 

			callFrameAnnotations: ["This method has not yet been implemented."],
			receiver: receiver,
			oneBeforeBeginMethod: { |m| m === NotYetImplementedError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'notYetImplemented'}} },
		) 
	}

 }

OutOfContextReturnError : MethodError {
	var <method, <result;
	*new { |receiver, method, result|
		if (method.isKindOf(Method).not) {
			Error("OutOfContextReturnError excepts a method").throw
		};
		^super.new(
			what: "'%-%' tried to return to a call frame that has expired with a value of: %".format(method.ownerClass),
			callFrameAnnotations: ["Could not complete this return as the parent method is no longer active."],
			receiver: receiver,
			method: method, 
			oneBeforeBeginMethod: { |m| m === OutOfContextReturnError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'outOfContextReturn'}} },
			result: result
		)
	}
}

ImmutableError : MethodError {
	var <value;
	*new { |receiver, value|
		^super.new(
			what: "Cannot mutate an immutable object",
			callFrameAnnotations: ["Make a copy of this object before mutating it."],
			receiver: receiver,
			oneBeforeBeginMethod: { |m| m === ImmutableError.class.findMethod(\new) or: {m.ownerClass === Object and: {m.name === 'immutableError'}} },
			value: value
		)
	}
}

// This doesn't need to exist.
BinaryOpFailureError : DoesNotUnderstandError { }

DeprecatedError : MethodError {
	var <method, <alternateMethod;

	*new { |receiver, method, alternateMethod|
		^super.new(
			what: "The method '%-%' is deprecated, instead use '%-%'.".format(method.ownerClass.name, method.name, alternateMethod.ownerClass.name, alternateMethod.name),
			callFrameAnnotations: ["Replace this with '*.%".format(alternateMethod.name)],
			receiver: receiver,
			method: method,
			oneBeforeBeginMethod: { |m| m === thisMethod or: {m == method} },
			alternateMethod: alternateMethod
		)
	}

	// This disables throwing when not in debug mode, but will halt when in debug.
	// This means that DeprecatedErrors are *not* exceptions, despite inheriting from Exception.
	throw {
		this.reportError;
		if (Error.debug) { this.halt }
	}
}
