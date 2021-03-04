// See CallOnce help for details on argument handling

CallOnce : Thunk {

	value { |... args| ^this.prEvaluate(\valueArray, args) }
	valueArray { |... args| ^this.prEvaluate(\valueArray, *args) }
	valueEnvir { |... args| ^this.prEvaluate(\valueArrayEnvir, args) }
	valueArrayEnvir { |... args| ^this.prEvaluate(\valueArrayEnvir, *args) }

	prEvaluate { |selector, args|
		^value ?? {
			value = function.performList(selector, args);
			function = nil;
			value
		}
	}
}


// Cleanup functions are passed a flag.
// The flag is set false if nodes have already been freed by CmdPeriod
EventStreamCleanup {

	var <>functions;		// cleanup functions from child streams and parent stream

	*new { ^super.new.clear }

	copy {
		^this.class.new.functions_(this.functions.copy)
	}

	clear {
		functions = IdentitySet.new;
	}

	addFunction { |event, function |
		if(event.isKindOf(Dictionary)) {
			function = CallOnce(function);
			functions.add(function);
			event[\addToCleanup] = event[\addToCleanup].add(function);
		};
		^function
	}

	addNodeCleanup { |event, function |
		if(event.isKindOf(Dictionary)) {
			function = CallOnce(function);
			functions.add(function);
			event[\addToNodeCleanup] = event[\addToNodeCleanup].add(function);
		};
		^function
	}

	update { | event |
		if(event.isKindOf(Dictionary)) {
			functions.addAll(event[\addToNodeCleanup]);
			functions.addAll(event[\addToCleanup]);
			functions.removeAll(event[\removeFromCleanup]); // backwards compat.
			functions = functions.reject(_.didEvaluate);
		};
		^event
	}

	exit { | event, freeNodes = true |
		if(event.isKindOf(Dictionary)) {
			this.update(event);
			functions.do(_.value(freeNodes));
			this.clear;
		};
		^event
	}

	terminate { | freeNodes = true |
		functions.do(_.value(freeNodes));
		this.clear
	}

}
