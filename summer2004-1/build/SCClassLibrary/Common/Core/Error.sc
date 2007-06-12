Error {
	classvar <>handling = false;
	classvar <>debug = false;
	
	var <>what, <>backtrace;

	*new { arg what;
		var backtrace;
		if (Error.debug) { backtrace = this.getBackTrace.caller };
		^super.newCopyArgs(what ? this.name, backtrace)
	}
	errorString {
		^"ERROR: " ++ what
	}
	reportError {
		this.errorString.postln;
		this.dumpBackTrace;
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
		args.dump;
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

BinaryOpFailureError : DoesNotUnderstandError {
	errorString {
		^"ERROR: binary operator '" ++ selector ++ "' failed."
	}
}


