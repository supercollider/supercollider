

AudioSpec : Spec {

	var <>numChannels=1;
	*new { arg numChannels=1;
		^super.new.numChannels_(numChannels)
	}
	*initClass {
		var mono,stereo;
		mono=AudioSpec.new;
		stereo=AudioSpec(2);
		specs.putAll(
		 IdentityDictionary[
			\audio -> mono,
			\audio1 -> mono,
			\audio2 -> mono,
			\stereo -> stereo,
			\mono -> mono
			];
		)
	}
	defaultControl{ 
		^PlayerInputProxy.new(this.numChannels,this.rate)
	}
	storeArgs { ^[numChannels] }
	rate { ^\audio }
}

MultiTrackAudioSpec : AudioSpec {
	var <>tracks;
	*new { arg tracks=2,numChannels=2;
		^super.new(numChannels).tracks_(tracks)
	}
	
}



TrigSpec : ControlSpec {
	
	defaultControl { ^BeatClockPlayer.new(4.0) }
	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\trig -> this.new,
			\gate -> this.new
			];
		)
	}
}


TempoSpec : ControlSpec {
	defaultControl { ^TempoPlayer.new }
	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\tempo -> this.new
			];
		)
	}
}

StaticSpec : ControlSpec { 
	// also a scalar spec, but better to inherit ControlSpec

	canKr { ^false }
	rate { ^\scalar }
	defaultControl { arg val=0.0; 
		^NumberEditor.new(this.constrain(val ? this.default),this) 
	}
}
StaticIntegerSpec : StaticSpec {
	defaultControl { arg val=0;
		^IntegerEditor(this.constrain(val ? this.default),this)
	}
}

ScalarSpec : Spec {
	// SendTrig etc. output a 0.0
	// this is a scalar spec.
	// so is Env, Sample
	canKr { ^false }
	rate { ^\scalar }

}

EnvSpec : ScalarSpec { // this is dodgy, not fully worked out

	var <>selector;
	var <>prototype;
	
	// level limitiations ?	
	
	*new { arg prototype;
		^super.new.prototype_(prototype ?? {Env.asr})
	}

	defaultControl {
		^EnvEditor.new(prototype.copy)
	}
	
	*initClass {
		specs.putAll(
			IdentityDictionary[ // from the common forms.
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
}

SymbolSetSpec : ScalarSpec {
	
	var <>symbols;
	
	*new { arg symbols;
		^super.new.symbols_(symbols)
	}

	defaultControl { ^symbols.first }
	*initClass {}
	storeArgs { ^[symbols] }
}

BufferProxySpec : ScalarSpec {

	var <>prototype;
	
	*new { arg prototype;
		^super.new.prototype_(prototype)
	}
	
	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\buffer -> this.new( BufferProxy(44100,2) ),
			\bufferProxy -> this.new( BufferProxy(44100,2) )
			//\bufnum -> this.new( BufferProxy(44100,2) )
		];
		)
	}

	//defaultControl { ^prototype.deepCopy }
	defaultControl { ^BufferProxy(44100,2) }
}

SampleSpec : ScalarSpec {

	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\sample -> this.new,
			\arrayBuffer -> this.new			
			];
		)
	}
	defaultControl { ^Sample.newClear(16) } // silent sample

}

