

+ Object {

	// sc server support
	//writeDefFile {}
	loadDefFileToBundle {}
	
//	play { ^ScalarPatchOut(this) }
	patchOut { ^ScalarPatchOut(this) }
	isPlaying { ^false }

	// floats only ?
	readyForPlay { ^true }
	prepareForPlay {  }
	
	spawnToBundle {}
	//spawn { ^ScalarPatchOut(this) }
	
	initDefArg {}
	free {}
}

//
//+ Nil {
//
//	isPlaying { ^false }
//}

