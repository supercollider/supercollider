
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

+ AudioInputSpec {
	
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numChannels" -> this.numChannels
		]	
	}
}

+ ArOutputSpec {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numChannels" -> this.numChannels
		]	
	}
}
+ MultiTrackArOutSpec {
	asPropertyList {
		^Dictionary[
			"class" -> this.class.name,
			"numTracks" -> this.tracks,
			"numChannels" -> this.numChannels
		]	
	}
}
//Kr inherits


// EnvSpec
