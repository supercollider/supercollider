EventStreamCleanup {
	var <>allFunctions;		// cleanup functions from child streams and parent stream
	var <>addedFunctions;		// cleanup functions from parent stream
	*new { ^super.new.clear }

	clear { 
		allFunctions = IdentitySet.new;
		addedFunctions = IdentitySet.new;
	}
	
	add { |event, function |
		addedFunctions = addedFunctions.add(function);
		allFunctions = allFunctions.add(function);
		event[\addToCleanup] = event[\addToCleanup].add(function);
	}
	
	update { | event |
		allFunctions = allFunctions.addAll(event[\addToCleanup]);
		allFunctions = allFunctions.removeAll(event[\removeFromCleanup]);
		^event
	}
	
	cleanup { | event |
		allFunctions.do(_.value);
		event[\removeFromCleanup] = event[\removeFromCleanup].addAll(allFunctions);
		this.clear;
		^event
	}
	
	exit { | event |
		addedFunctions.do(_.value);
		event[\removeFromCleanup] = event[\removeFromCleanup].addAll(allFunctions);
		this.clear;
		^event
	}
	
	terminate {
		allFunctions.do(_.value);
		this.clear	
	}
}


