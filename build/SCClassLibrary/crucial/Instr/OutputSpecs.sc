
/*  just use AudioSpec
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
*/

/* just use ControlSpec
KrOutputSpec : ArOutputSpec {
	rate { ^\control }
	*initClass {
		specs = specs.addAll([
			\krOut -> KrOutputSpec(1)
		]);
	}
}
*/


