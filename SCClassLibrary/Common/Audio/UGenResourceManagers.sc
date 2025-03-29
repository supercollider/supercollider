UGenResourceManager {
	classvar allResourceManagers;
	*register { |resourceManager| allResourceManagers = allResourceManagers.add(resourceManager) }
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
	var previousStateDependentUGens;
	var previousLastGroup;
	var currentMode = \initial;
	var <all;

	*new { ^super.newCopyArgs(all: Set.new) }

	nonCausalModes { this.subclassResponsibility(thisMethod) } // Returns an Array of Symbols
	accessMessage { this.subclassResponsibility(thisMethod) } // Returns a Symbol

	add { |ugen|
		if (currentMode == \initial) {
			currentMode = this.prTryGettingMode(ugen);
			^previousStateDependentUGens = [ugen];
		};

		if (currentMode == \connectToAll) {
			previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
			currentMode = this.prTryGettingMode(ugen);
			previousStateDependentUGens.do {|p| all.add(p) }
			^previousStateDependentUGens = [ugen];
		};

		this.nonCausalModes.do({ |m|
			if(currentMode == m and: {ugen.perform(this.accessMessage) == m}){
				previousLastGroup.do( _.createWeakConnectionTo(ugen) );
				^previousStateDependentUGens = previousStateDependentUGens.add(ugen); // no edge needed here.
			}
		});

		// add connections for everything else.
		previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
		previousStateDependentUGens.do {|p| all.add(p) };

		previousLastGroup = previousStateDependentUGens;

		previousStateDependentUGens = [ugen];
		currentMode = this.prTryGettingMode(ugen);
	}

	connectToAll { |ugen|
		previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
		previousStateDependentUGens.do {|p| all.add(p) };
		previousStateDependentUGens = [ugen];
		currentMode = \connectToAll;
	}

	prTryGettingMode { |ugen|
		var out;
		try {
			out = ugen.perform(this.accessMessage)
		} { |e|
			if (e.isKindOf(DoesNotUnderstandError)){
				Error("% does not implement method % needed by %".format(ugen.class, this.accessMessage, this.class)).throw
			} {
				e.throw
			}
		};
		^out;
	}
}

UGenMessageResourceManager : UGenResourceManagerSimple {
	*initClass { UGenResourceManager.register(this) }
}

UGenDoneResourceManager : UGenResourceManagerSimple {
	*initClass { UGenResourceManager.register(this) }
}

UGenBufferResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { UGenResourceManager.register(this) }
	nonCausalModes { ^#[\read] }
	accessMessage { ^\bufferAccessType }
}

UGenBusResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { UGenResourceManager.register(this) }
	nonCausalModes { ^#[\readAr, \writeAr, \readKr, \writeKr] }
	accessMessage { ^\busAccessType }
}

UGenLocalBusResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { UGenResourceManager.register(this) }
	nonCausalModes { ^#[\readAr, \writeAr, \readKr, \writeKr] }
	accessMessage { ^\localBusAccessType }
}

UGenRandomResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { UGenResourceManager.register(this) }
	nonCausalModes { ^#[\gen] }
	accessMessage { ^\randomAccessType }
}

UGenAnalogResourceManager : UGenResourceManagerWithNonCausalModes {
	*initClass { UGenResourceManager.register(this) }
	// here, writes are sequential because I am assuming they overwrite their output
	// --- this is not mentioned in the help file.
	nonCausalModes { ^#[\read] }
	accessMessage { ^\analogAccessType }
}

UGenDiskResourceManager : UGenResourceManagerSimple {
	*initClass { UGenResourceManager.register(this) }
}
