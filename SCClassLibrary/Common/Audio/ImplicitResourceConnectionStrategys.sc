// Used to create edges between UGens so the order they access implicit resources in is preserved.
ImplicitResourceConnectionStrategy {
	classvar allImplicitResourceStrategies;

	// All final classes must call this in initClass.
	*register { allImplicitResourceStrategies = allImplicitResourceStrategies.add(this) }

	// Used to create instances of these strategies for each synthdef.
	*createNewInstances { ^allImplicitResourceStrategies.collect({|m| m -> m.new }).asEvent }

	// Interface called from inside SynthDef.

	// Add UGen to the strategy, the strategy may create new edges.
	// 'accessType' is an optional symbol used to describe how the ugen accesses the resource.
	add { |ugen, accessType|  this.subclassResponsibility(thisMethod) }

	// Called when the UGen doesn't explicitly state it touches no implicit resources.
	// Connect the UGen to all resources to ensure correct ordering.
	connectToAll { this.subclassResponsibility(thisMethod) }
}

// Simply connect each UGen that touches this resource together in the order they are defined in the synthdef.
ImplicitResourceConnectionStrategySimple : ImplicitResourceConnectionStrategy {
	var previousUGen;

	*new { ^super.new }

	add { |ugen|
		previousUGen !? ( _.createWeakConnectionTo(ugen) );
		previousUGen = ugen;
	}

	connectToAll { |ugen| this.add(ugen) }
}

// Allow certain groups of UGens with similar access modes to be reordered.
// For example, it doesn't matter what order a sequential block of In.ar UGens are written in.  e.g....
//   var a = In.ar;
//   var b = In.ar;
//   var c = In.ar;
//   var d = In.ar;
//   var o = Out.ar;
// Above it doesn't matter what order a, b, c, or d appear, just so long as the Out.ar happens last.
// In this case, the access mode of the In.ar ugen is non-causal.
ImplicitResourceConnectionStrategyWithNonCausalModes : ImplicitResourceConnectionStrategy {
	classvar nonCausalModes;
	var activeGroup;
	var previousGroup;
	var currentMode; // nil is the initial state here.

	*new { ^super.new }

	*register { |modes|
		nonCausalModes = modes;
		super.register;
	}

	add { |ugen, accessMode|

		accessMode ?? { Error("All UGens with causal modes resources must specify an access mode").throw };

		if (currentMode.isNil) {
			currentMode = accessMode;
			activeGroup = [ugen];
			^nil
		};

		if (currentMode == \connectToAll) {
			activeGroup.do( _.createWeakConnectionTo(ugen) );
			currentMode = accessMode;
			activeGroup = [ugen];
			^nil
		};

		nonCausalModes.do({ |m|
			if(currentMode == m and: {accessMode == m}){
				previousGroup.do( _.createWeakConnectionTo(ugen) );
				activeGroup = activeGroup.add(ugen); // no edge needed here as we haven't left the active non-causal group.
				^nil
			}
		});

		// add connections for everything else.
		activeGroup.do( _.createWeakConnectionTo(ugen) );

		previousGroup = activeGroup;

		activeGroup = [ugen];
		currentMode = accessMode;
	}

	connectToAll { |ugen|
		activeGroup.do( _.createWeakConnectionTo(ugen) );
		activeGroup = [ugen];
		currentMode = \connectToAll;
	}
}

MessageConnectionStrategy : ImplicitResourceConnectionStrategySimple {
	*initClass { this.register }
}

DoneConnectionStrategy : ImplicitResourceConnectionStrategySimple {
	*initClass { this.register }
}

BufferConnectionStrategy : ImplicitResourceConnectionStrategyWithNonCausalModes {
	*initClass { this.register(#[\read]) }
}

BusConnectionStrategy : ImplicitResourceConnectionStrategyWithNonCausalModes {
	*initClass { this.register(#[\readAr, \writeAr, \blendAr, \readKr, \writeKr, \blendKr]) }
}

// Because you can only have one LocalIn and LocalOut, their ordering is simple.
LocalBusConnectionStrategy : ImplicitResourceConnectionStrategySimple {
	*initClass { this.register }
}

RandomStateConnectionStrategy : ImplicitResourceConnectionStrategyWithNonCausalModes {
	*initClass { this.register(#[\gen]) }
}

AnalogStateConnectionStrategy : ImplicitResourceConnectionStrategyWithNonCausalModes {
	*initClass { this.register(#[\read]) }
}

DiskStateConnectionStrategy : ImplicitResourceConnectionStrategySimple {
	*initClass { this.register(this) }
}
