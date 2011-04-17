// Cleanup functions are passed a flag.
// The flag is set false if nodes have already been freed by CmdPeriod
// This caused a minor change to TempoClock:clear and TempoClock:cmdPeriod

EventStreamCleanup {
	var <>functions;		// cleanup functions from child streams and parent stream
	*new { ^super.new.clear }

	clear {
		functions = IdentitySet.new;
	}

	addFunction { |event, function |
		if(event.isKindOf(Dictionary)) {
			functions = functions.add(function);
			event[\addToCleanup] = event[\addToCleanup].add(function);
		};
	}

	addNodeCleanup { |event, function |
		if(event.isKindOf(Dictionary)) {
			functions = functions.add(function);
			event[\addToNodeCleanup] = event[\addToNodeCleanup].add(function);
		};
	}

	update { | event |
		if(event.isKindOf(Dictionary)) {
			functions = functions.addAll(event[\addToNodeCleanup]);
			functions = functions.addAll(event[\addToCleanup]);
			functions = functions.removeAll(event[\removeFromCleanup]);
		};
		^event
	}

	exit { | event, freeNodes = true |
		if(event.isKindOf(Dictionary)) {
			this.update(event);
			functions.do(_.value(freeNodes) );
			event[\removeFromCleanup] = event[\removeFromCleanup].addAll(functions);
			this.clear;
		};
		^event
	}

	terminate { | freeNodes = true |
		functions.do(_.value(freeNodes));
		this.clear
	}

}
