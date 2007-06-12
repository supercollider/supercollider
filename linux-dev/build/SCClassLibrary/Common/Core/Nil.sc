Nil {
	*new { ^this.shouldNotImplement(thisMethod) }
	*newClear { ^this.shouldNotImplement(thisMethod) }
	
	isNil { ^true }
	notNil { ^false }
	? { arg obj; ^obj }
	?? { arg obj; ^obj.value }
	
	// support a nil Environment
	push { arg function; ^function.value }
	
	// support a nil Spawn event function
	spawn {}
	
	// nil parent view
	asView {}

	// support a nil Plug
	source {}
	source_ {}
	// nil Synth support
	setPlug {} 
	fadeEnd {}
	
	do {}
	doReverse {}
	collect {}
	select {}
	reject {}
	detect { ^false }
	
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

	
	// graphical support
	draw{}
	click { ^nil }
	fontID { ^nil }
	asRect { ^Rect.new }
	asArray { ^[] }
	
	// ControlView support
	set {}
	get { arg prevVal; ^prevVal }
}
