UGenResourceManager {
	add { |ugen| this.subclassResponsibility(thisMethod) }
	panic { |ugen| this.subclassResponsibility(thisMethod) }
	getAll { this.subclassResponsibility(thisMethod) }
}

UGenResourceManagerSimple : UGenResourceManager {
	var previousStateDependentUGen;
	var all;

	*new { ^super.newCopyArgs(nil, Set.new) }
	panic { |ugen| this.add(ugen) }
	getAll { ^all }
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
	var currentMode;
	var all;

	var nonCausalModes;
	var nameOfModeGetter;

	*new { |nonCausalModes, nameOfModeGetter|
		^super.newCopyArgs(nil, nil, \initial, Set.new, nonCausalModes.asArray, nameOfModeGetter.asSymbol)
	}

	add { |ugen|
		if (currentMode == \initial) {
			currentMode = this.prTryGettingMode(ugen);
			^previousStateDependentUGens = [ugen];
		};

		if (currentMode == \panic) {
			previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
			currentMode = this.prTryGettingMode(ugen);
			previousStateDependentUGens.do {|p| all.add(p) }
			^previousStateDependentUGens = [ugen];
		};

		nonCausalModes.do({ |m|
			if(currentMode == m and: {ugen.perform(nameOfModeGetter) == m}){
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

	panic { |ugen|
		previousStateDependentUGens.do( _.createWeakConnectionTo(ugen) );
		previousStateDependentUGens.do {|p| all.add(p) };
		previousStateDependentUGens = [ugen];
		currentMode = \panic;
	}

	getAll { ^all }

	prTryGettingMode { |ugen|
        var out;
	    try {
	        out = ugen.perform(nameOfModeGetter)
	    } { |e|
	        if (e.isKindOf(DoesNotUnderstandError)){
	            Error("% does not implement method % needed by %".format(ugen.class, nameOfModeGetter, this.class)).throw
	        } {
                e.throw
	        }
	    };
	    ^out;
	}
}

UGenMessageResourceManager : UGenResourceManagerSimple {}

UGenBufferResourceManager : UGenResourceManagerWithNonCausalModes {
	*new { ^super.new([\read], \bufferAccessType) }
}

UGenBusResourceManager : UGenResourceManagerWithNonCausalModes {
	*new { ^super.new([\read, \write], \busAccessType) }
}

UGenRandomResourceManager : UGenResourceManagerWithNonCausalModes {
	*new { ^super.new([\gen], \randomAccessType) }
}

UGenAnalogResourceManager : UGenResourceManagerWithNonCausalModes {
	// here, writes are sequential because I am assuming they overwrite their output
	// --- this is not mentioned in the help file.
	*new { ^super.new([\read], \analogAccessType) }
}

UGenDiskResourceManager : UGenResourceManagerSimple {}
