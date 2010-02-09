
Exception {
	classvar <>handling = false;
	classvar <>debug = false;

	var <>what, <>backtrace;

	*new { arg what;
		var backtrace;
		if (debug) { backtrace = this.getBackTrace.caller };
		^super.newCopyArgs(what ? this.name, backtrace)
	}
	errorString {
		^"EXCEPTION: " ++ what
	}
	reportError {
		this.errorString.postln;
		this.dumpBackTrace;
	}
	isException { ^true }
}

Error : Exception {
	errorString {
		^"ERROR: " ++ what
	}
}

MethodError : Error {
	var <>receiver;

	*new { arg what, receiver;
		^super.new(what).receiver_(receiver)
	}
	reportError {
		this.errorString.postln;
		"RECEIVER:\n".post;
		receiver.dump;
		this.dumpBackTrace;
	}
}

PrimitiveFailedError : MethodError {
	var <>failedPrimitiveName;

	*new { arg receiver;
		^super.new(Thread.primitiveErrorString, receiver)
			.failedPrimitiveName_(thisThread.failedPrimitiveName)
	}
	errorString {
		^"ERROR: Primitive '" ++ failedPrimitiveName ++ "' failed.\n" ++ what
	}
}

SubclassResponsibilityError : MethodError {
	var <>method, <>class;
	*new { arg receiver, method, class;
		^super.new(nil, receiver).method_(method).class_(class)
	}
	errorString {
		^"ERROR: '" ++ method.name ++ "' should have been implemented by "
			++ class.name ++ "."
	}
}

ShouldNotImplementError : MethodError {
	var <>method, <>class;
	*new { arg receiver, method, class;
		^super.new(nil, receiver).method_(method).class_(class)
	}
	errorString {
		^"ERROR: '" ++ method.ownerClass.name ++ "-" ++ method.name
			++ "' Message not valid for this subclass: "
			++ class.name ++ "."
	}
}

DoesNotUnderstandError : MethodError {
	var <>selector, <>args;
	*new { arg receiver, selector, args;
		^super.new(nil, receiver).selector_(selector).args_(args)
	}
	errorString {
		^"ERROR: Message '" ++ selector ++ "' not understood."
	}
	reportError {
		this.errorString.postln;
		"RECEIVER:\n".post;
		receiver.dump;
		"ARGS:\n".post;
		args.dumpAll;
		this.dumpBackTrace;
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

DeprecatedError : Error {
	var <>receiver;
	var <>method, <>class, <>alternateMethod;

	*new { arg receiver, method, alternateMethod, class;
		^super.new(nil, receiver).method_(method).alternateMethod_(alternateMethod).class_(class)
	}
	errorString {
		var string;
		string = "ERROR: Method '" ++ method.name ++ "' of class " ++ class.name ++ " is deprecated and will be removed.";
		if(alternateMethod.notNil, {
			string = string + "Use '" ++ alternateMethod.ownerClass ++ ":" ++ alternateMethod.name ++ "' instead.";
		});
		^string;
	}

	reportError {
		this.errorString.postln;
	}

	throw {
		Error.handling = true;
		this.reportError;
		if (Error.debug) {
			this.dumpBackTrace;
			Error.handling = false;
			this.halt;
		} {
			Error.handling = false;
		};

	}
}

