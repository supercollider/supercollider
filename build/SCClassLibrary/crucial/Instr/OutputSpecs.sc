

ArOutputSpec : Spec {
	var <numChannels;
	*new { arg numChannels=1;
		^super.newCopyArgs(numChannels)
	}
	rate { ^\audio }
	*initClass {
		specs = specs.addAll([
			\stereoOut -> ArOutputSpec(2),
			\monoOut -> ArOutputSpec(1)
		]);
	}
}

MultiTrackArOutSpec : ArOutputSpec {
	var <>tracks;
	*new { arg tracks=2,numChannels=2;
		^super.new(numChannels).tracks_(tracks)
	}
	
}

KrOutputSpec : ArOutputSpec {
	rate { ^\control }
}

