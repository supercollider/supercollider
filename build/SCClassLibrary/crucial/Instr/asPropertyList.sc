
+ Spec {	
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name
		]	
	}
}

+ ControlSpec {
	
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"minval" -> this.minval,
			"maxval" -> this.maxval,
			"warp" -> this.warp.asPropertyList,
			"step" -> this.step,
			"default" -> this.default
		]
	}
}

+ Warp {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name
		]
	}	
}

/*
	CurveWarp
*/

+ AudioSpec {
	
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numChannels" -> this.numChannels
		]	
	}
}
/*
+ ArOutputSpec {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numChannels" -> this.numChannels
		]	
	}
}
*/

+ MultiTrackAudioSpec {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numTracks" -> this.tracks,
			"numChannels" -> this.numChannels
		]	
	}
}


//todo: EnvSpec


