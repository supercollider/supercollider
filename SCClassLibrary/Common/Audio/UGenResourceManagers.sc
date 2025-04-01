UGenResourceManager {
	classvar allResourceManagers;
	*register { allResourceManagers = allResourceManagers.add(this) }
	*createNewInstances { ^allResourceManagers.collect({|m| m -> m.new }).asEvent }

	// Interface called from inside SynthDef.
	add { |ugen| this.subclassResponsibility(thisMethod) }
	connectToAll { |ugen| this.subclassResponsibility(thisMethod) }
}

UGenResourceManagerSimple : UGenResourceManager {
	var previousStateDependentUGen;
	var all;

	*new { ^super.newCopyArgs(all: Set.new) }

	connectToAll { |ugen| this.add(ugen) }
	add { |ugen|
		previousStateDependentUGen !? { |p|
			p.createWeakConnectionTo(ugen);
			all.add(p);
		};
		previousStateDependentUGen = ugen;
	}
}

UGenResourceManagerWithNonCausalModes : UGenResourceManager {
	classvar nonCausalModes;
	var previousStateDependentUGens;
	var previousLastGroup;
	var currentMode; // nil is the initial state here.

	*new { ^super.new }
	*register { |modes|
		nonCausalModes = modes;
		super.register;
	}

	add { |ugen, accessMode|
		accessMode ?? { Error("All UGens with causal modes resources must specifiy an access mode").throw };
		if (currentMode.isNil) {
			currentMode = accessMode;
			previousStateDependentUGens = [ugen];
			^nil
		};

		if (currentMode == \connectToAll) {
			previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
			currentMode = accessMode;
			previousStateDependentUGens = [ugen];
			^nil
		};

		nonCausalModes.do({ |m|
			if(currentMode == m and: {accessMode == m}){
				previousLastGroup.do( _.createWeakConnectionTo(ugen) );
				^previousStateDependentUGens = previousStateDependentUGens.add(ugen); // no edge needed here.
			}
		});

		// add connections for everything else.
		previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );

		previousLastGroup = previousStateDependentUGens;

		previousStateDependentUGens = [ugen];
		currentMode = accessMode;
	}

	connectToAll { |ugen|
		previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
		previousStateDependentUGens = [ugen];
		currentMode = \connectToAll;
	}
}

UGenMessageResourceManager : UGenResourceManagerSimple {
	*initClass { this.register }
}

UGenDoneResourceManager : UGenResourceManagerSimple {
	*initClass { this.register }
}

UGenBufferResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { this.register(#[\read]) }
}

// TODO: this could be improved by letting 'replace' UGens be grouped depending on their rate.
UGenBusResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { this.register(#[\readAr, \writeAr, \blendAr, \readKr, \writeKr, \blendKr]) }
}

// Because you can only have one LocalIn and LocalOut, their ordering is simple.
UGenLocalBusResourceManager : UGenResourceManagerSimple {
	*initClass { this.register }
}

UGenRandomResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { this.register(#[\gen]) }
}

UGenAnalogResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { this.register(#[\read]) }
}

UGenDiskResourceManager : UGenResourceManagerSimple {
	*initClass { this.register(this) }
}
