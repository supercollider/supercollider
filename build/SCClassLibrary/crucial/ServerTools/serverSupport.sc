

+ Object {

	// sc server support
	writeDefFile {}
	loadDefFile {}
	
//	play { ^ScalarPatchOut(this) }
	isPlaying { ^false }

	// floats only ?
	readyForPlay { ^true }
	prepareForPlay { ^true } // i am ready

	spawn { ^ScalarPatchOut(this) }
	free {}
}
//
//+ Nil {
//
//	isPlaying { ^false }
//}

