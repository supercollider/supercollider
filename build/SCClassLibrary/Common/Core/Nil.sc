Nil {
	*new { ^this.shouldNotImplement(thisMethod) }
	*newClear { ^this.shouldNotImplement(thisMethod) }
	
	isNil { ^true }
	notNil { ^false }
	? { arg obj; ^obj }
	?? { arg obj; ^obj.value }
	
	// support a nil Environment
	push { arg function; ^function.value }
	appendStream { arg stream; ^stream }
	pop {}
		
	// support a nil Plug
	source {}
	source_ {}
		
	// rate access support
	rate { ^nil }
	numChannels { ^nil }
	
	do {}
	doReverse {}
	collect {}
	select {}
	reject {}
	detect { ^false }
	collectAs {}
	selectAs {}
	rejectAs {}
	
	// dependancy operators are no-ops
	dependants { 			
		^IdentitySet.new 
	}
	changed {}
	addDependant {}
	removeDependant {}
	release {}
	update {}

	// nil Event support
	transformEvent { arg event;
		^event
	}
	awake {}
	play {}
	
	swapThisGroup{}
	
	performMsg {}
	
	printOn { arg stream;
		stream.putAll(this.asString);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}
	
	// Array support
	add { arg value; 
		// This makes it unecessary to check for array.isNil when doing:
		// array = array.add(thing);     Instead, it just works.
		^[value] 
	}
	addAll { arg array; ^array }
	++ { arg array; ^array }
	asCollection { ^[] }

	// ControlView support
	set {}
	get { arg prevVal; ^prevVal }
	
	
	// if Main-startup fails then AppClock scheduler may be nil. If that happens an
	// endless cascade of doesNotUnderstand messages gets printed in response to each clock tick
	// unless we do this.
	seconds_ {}
}
