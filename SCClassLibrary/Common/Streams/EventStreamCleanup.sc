// The "new" cleanup framework: built-in mechanism to avoid duplicate
// execution of cleanups, difficult to avoid otherwise due to their
// replicated nature in a stream graph. Instead of storing bare Functions,
// they are now stored in CleanupThunks.

// Unlike Thunks which take no arguments, CleanupThunks do allow arguments
// to be passed to the function, but the CleanupThunk contract is that any
// subsequent calls into the function would monotonically do the same or
// less (cleanup) work, so that the first call does the maximal amount of
// relevant work, making subsequent calls completely redundant.
// Prototypical example: if a cleanup is called with a "flag: false",
// it is assumed it will *not* be called later again with "flag: true",
// if the latter means that the function would have to do "extra" work
// that was skipped/excluded by "flag: false".
// CleanupThunk is not a general-purpose function memoization class for
// functions with arguments.
CleanupThunk : Thunk {

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
			function = CleanupThunk(function);
			functions.add(function);
			event[\addToCleanup] = event[\addToCleanup].add(function);
		};
		^function
	}

	addNodeCleanup { |event, function |
		if(event.isKindOf(Dictionary)) {
			function = CleanupThunk(function);
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
