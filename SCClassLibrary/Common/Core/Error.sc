Exception {
	classvar <>handling = false;
	classvar <>debug = false;

	var <>what, <>backtrace, <>path;

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
		this.adviceLink.postln;
	}
	adviceLink {
		^("For advice: [http://supercollider.sf.net/wiki/index.php/%]"
			.format(this.errorString.tr($ , $_).tr($\n, $_)));
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
	var <>receiver;

	*new { arg what, receiver;
		^super.new(what).receiver_(receiver)
	}
	reportError {
		this.errorString.postln;
		"RECEIVER:\n".post;
		receiver.dump;
		this.errorPathString.post;
		this.dumpBackTrace;
		this.adviceLink.postln;
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
		this.errorPathString.post;
		this.dumpBackTrace;
		this.adviceLink.postln;
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
			var c = m.ownerClass;
			var str = c.name.asString;
			if(c.isMetaClass)
				{ str = str.drop( str.indexOf($_) + 1 ) ++ ":*" ++ m.name }
				{ str = str ++ ":-" ++ m.name };
			str;
		};
		var string;
		string = "WARNING: Method" + methodSignature.value(method)+ "is deprecated and will be removed.";
		if(alternateMethod.notNil, {
			string = string + "Use" + methodSignature.value(alternateMethod) + "instead.";
		});
		^string;
	}

	reportError {
		this.errorString.postln;
		this.adviceLink.postln;
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
