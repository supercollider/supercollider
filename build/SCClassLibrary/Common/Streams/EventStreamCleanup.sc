//EventStreamCleanup {
//	var <>allFunctions;		// cleanup functions from child streams and parent stream
//	var <>addedFunctions;		// cleanup functions from parent stream
//	*new { ^super.new.clear }
//
//	clear { 
//		allFunctions = IdentitySet.new;
//		addedFunctions = IdentitySet.new;
//	}
//	
//	add { |event, function |
//		addedFunctions = addedFunctions.add(function);
//		allFunctions = allFunctions.add(function);
//		event[\addToCleanup] = event[\addToCleanup].add(function);
//	}
//	
//	update { | event |
//		allFunctions = allFunctions.addAll(event[\addToCleanup]);
//		allFunctions = allFunctions.removeAll(event[\removeFromCleanup]);
//		^event
//	}
//	
//	cleanup { | event |
//		this.update(event);
//		allFunctions.do(_.value);
//		event[\removeFromCleanup] = event[\removeFromCleanup].addAll(allFunctions);
//		this.clear;
//		^event
//	}
//	
//	exit { | event |
//		addedFunctions.do(_.value);
//		event[\removeFromCleanup] = event[\removeFromCleanup].addAll(allFunctions);
//		this.clear;
//		^event
//	}
//	
//	terminate {
//		allFunctions.do(_.value);
//		this.clear	
//	}
//}
//
EventStreamCleanup {
	var <>functions;		// cleanup functions from child streams and parent stream
	*new { ^super.new.clear }

	clear { 
		functions = IdentitySet.new;
	}
	
	addFunction { |event, function |
		functions = functions.add(function);
		event[\addToCleanup] = event[\addToCleanup].add(function);
	}

	addOff { |event, function |
		functions = functions.add(function);
		event[\addToNodeCleanup] = event[\addToNodeCleanup].add(function);
	}
	
	update { | event |
		functions = functions.addAll(event[\addToNodeCleanup]);
		functions = functions.addAll(event[\addToCleanup]);
		functions = functions.removeAll(event[\removeFromCleanup]);
		^event
	}
	
	exit { | event |
		this.update(event);
		functions.do{ | f |  f.play};
		event[\removeFromCleanup] = event[\removeFromCleanup].addAll(functions);
		this.clear;
		^event
	}
		
	terminate {
		functions.do(_.value);
		this.clear	
	}
	
}


EventStreamPlayerCleanup : EventStreamCleanup {
	var <>nodeFunctions;		// cleanup functions from child streams and parent stream

	// the EventSreamPlayer must not do node cleanups when it is stopped by CmdPeriod
	// so we keep node cleanups separate in the event, but only honor the distinction
	// in the 	
	update { | event |
		functions = functions.addAll(event[\addToCleanup]);
		functions = functions.removeAll(event[\removeFromCleanup]);
		nodeFunctions = nodeFunctions.addAll(event[\addToNodeCleanup]);
		nodeFunctions = nodeFunctions.removeAll(event[\removeFromCleanup]);
		^event
	}

	terminateNodeFunctions {
		nodeFunctions.do(_.value);
	
	}
	clear { 
		functions = IdentitySet.new;
		nodeFunctions = IdentitySet.new;
	}
}

/*
cases:
	1. stream terminates and cleans up after itself - it executes all cleanup routines and appends them for removal
	to the event
	2. substream terminates, caller updates


*/