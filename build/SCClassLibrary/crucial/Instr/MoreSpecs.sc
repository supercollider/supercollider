

AudioSpec : Spec {

	var <>numChannels=1;
	*new { arg numChannels=1;
		^super.new.numChannels_(numChannels)
	}
	*initClass {
		var mono,stereo;
		mono=AudioSpec.new;
		stereo=AudioSpec(2);
		specs.addAll(
		 [
			\audio -> mono,
			\audio1 -> mono,
			\audio2 -> mono,
			\stereo -> stereo,
			\mono -> mono
			];
		)
	}
	defaultControl{ 
		^PlayerInputProxy.new(this)
	}
	storeArgs { ^[numChannels] }
	rate { ^\audio }
	canAccept { arg thing;
		^(thing.isKindOf(AbstractPlayer) and: { thing.spec == this })
	}
}

MultiTrackAudioSpec : AudioSpec {
	var <>tracks;
	*new { arg tracks=2,numChannels=2;
		^super.new(numChannels).tracks_(tracks)
	}
	storeArgs { ^[tracks,numChannels] }
}



TrigSpec : ControlSpec {
	
	defaultControl { ^BeatClockPlayer.new(4.0) }
	*initClass {
		specs.addAll(
		 [
			\trig -> this.new,
			\gate -> this.new
			];
		)
	}
	canAccept { arg thing;
		// could accept 0 or 1 but useless
		^(thing.isKindOf(AbstractPlayer) and: { thing.spec == this })
	}
}


TempoSpec : Spec {
	defaultControl { ^TempoPlayer.new }
	rate { ^\control }
	*initClass {
		specs.addAll(
		 [
			\tempo -> this.new
			];
		)
	}
}

NoLagControlSpec : ControlSpec {

	*initClass {
		specs.addAll(
		 [
		 	\binary -> this.new,
			\loop -> this.new,
			\paused -> this.new
			];
		)
	}
	defaultControl { arg val; 
		^KrNumberEditor.new(this.constrain(val ? this.default),this).lag_(nil) 
	}

}

StaticSpec : NoLagControlSpec { 
	// also a scalar spec, but better to inherit ControlSpec

	canKr { ^false }
	rate { ^\scalar }
	defaultControl { arg val;
		^NumberEditor.new(this.constrain(val ? this.default),this) 
	}
}
StaticIntegerSpec : StaticSpec {
	defaultControl { arg val;
		^IntegerEditor(this.constrain(val ? this.default),this)
	}
	*initClass {
		specs.addAll(
		 [
		 	\quantity -> this.new(1,128),
			\qnty -> this.new(1,128)
			];
		)
	}
}

ScalarSpec : Spec {
	// SendTrig etc. output a 0.0
	// this is a scalar spec.
	canKr { ^false }
	rate { ^\scalar }
}

EnvSpec : ScalarSpec {

	var <>prototype;
	
	// use a level spec ?	
	
	*new { arg prototype;
		^super.new.prototype_(prototype ?? {Env.asr})
	}
	storeArgs { ^[prototype] }
	defaultControl {
		^EnvEditor.new(prototype.copy)
	}
	
	*initClass {
		specs.addAll(
			[ // from the common forms.
				\env -> this.new(Env.new([0,1,0],[1,1])),
				\envlinen -> this.new(Env.linen),
				\envtriangle -> this.new(Env.triangle),
				\envsine -> this.new(Env.sine),
				\envperc -> this.new(Env.perc),
				\envadsr -> this.new(Env.adsr),
				\envasr -> this.new(Env.asr),
				\env3 -> this.new(Env.newClear(3)),
				\env3sustain -> this.new(Env.newClear(3,2)),
				\fenv -> this.new(Env.new([ 0, 1, 0.2, 0 ], [ 0.04, 0.4, 0.3 ], [ -6.31, 1.1, -2 ], nil, nil)),
				\rqenv -> this.new(  Env.new([ 0.194444, 0.0810185, 0.0648148, 0.444444 ], [ 0.01, 0.111111, 0.0833333 ], [ -0.583333, 3.33333, 1.66667 ], nil, nil)),
				\envpercshort -> this.new(Env.new([ 0, 1, 1, 0.444444, 0 ], [ 0.166667, 1, 0.805556, 0.777778 ], [ -7.16667, -2, 2, -2 ], nil, nil))
				
			]
		)
	}	
	canAccept { arg thing;
		^thing.isKindOf(Env)
	}
}

BufferProxySpec : ScalarSpec {

	var <>numFrames=44100,<>numChannels=1,<>sampleRate=44100.0;
	
	*new { arg numFrames=44100,numChannels=1,sampleRate=44100.0;
		^super.new.numFrames_(numFrames).numChannels_(numChannels).sampleRate_(sampleRate)
	}
	storeArgs { ^[numFrames,numChannels,sampleRate] }
	*initClass {
		specs.addAll(
		 [
			\buffer -> this.new( 44100,2 ),
			\bufferProxy -> this.new( 44100,2 ),
			\fftbuff -> this.new( 2048 , 1)
		];
		)
	}

	//defaultControl { ^prototype.deepCopy } // this caused a deepCopy crash !?
	defaultControl { ^BufferProxy(numFrames,numChannels,sampleRate) }
}

SampleSpec : ScalarSpec {

	*initClass {
		specs.addAll(
		 [
			\sample -> this.new,
			\arrayBuffer -> this.new			
			];
		)
	}
	defaultControl { ^Sample.newClear(16) } // silent sample
	canAccept { arg ting; ^ting.isKindOf(Sample) }

}
HasItemSpec : ScalarSpec {
	var <>itemSpec;
	*new { arg itemSpec;
		^super.new.itemSpec_(itemSpec.asSpec)
	}
}
ArraySpec : HasItemSpec {
	*initClass {
		specs.addAll(
		 [
			\array -> this.new(\degree),
			\scale -> this.new(StaticSpec(-100,100,\linear))
		];
		)
	}
	canAccept { arg ting;
		^ting.isArray // ... and every is in
	}
}

StreamSpec : HasItemSpec {
	*initClass {
		specs.addAll(
		 [
		 	\stream -> this.new,
			\degreeStream -> this.new(StaticIntegerSpec(-100,100,\linear,1)),
			\durationStream -> this.new(StaticSpec(2 ** -6, 2 ** 8))
		];
		)
	}
	defaultControl { ^itemSpec.defaultControl }
	default { ^itemSpec.default }
	constrain { arg value; ^itemSpec.constrain(value) }
	canAccept { arg ting;
		^(ting.rate == \stream or: {itemSpec.canAccept(ting) })
	}
	rate { ^\stream }
}

PlayerSpec : HasItemSpec {
	*initClass {
		specs.addAll(
		 [
			\player -> this.new(\audio)
		];
		)
	}
	defaultControl { ^itemSpec.defaultControl }
	canAccept { arg ting;
		^(ting.isKindOf(AbstractPlayer) and: {ting.spec == itemSpec})
	}
}
