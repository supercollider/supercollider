

StaticSpec : ControlSpec {

	canKr { ^false }
	rate { ^\scalar }

}

AudioInputSpec : Spec {

	var <>numChannels=1;
	*new { arg numChannels=1;
		^super.new.numChannels_(numChannels)
	}
	*initClass {
		var mono,stereo;
		mono=AudioInputSpec.new;
		stereo=AudioInputSpec(2);
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
	default { ^0.0 }
	storeParamsOn { arg stream;
		stream << "(" <<< numChannels << ")"
	}
	rate { ^\audio }

}

SymbolSetSpec : Spec {
	
	var <>symbols;
	
	*new { arg symbols;
		^super.new.symbols_(symbols)
	}

	default { ^symbols.first }
	*initClass {}
	storeParamsOn { arg stream;
		stream << "(" <<< symbols << ")"
	}

}

TrigSpec : ControlSpec {
	
	default { ^0.0 } // forced ?  minval is used anyway
	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\trig -> this.new,
			\gate -> this.new
			];
		)
	}
}

EnvSpec : Spec { // this is dodgy, not fully worked out

	var <>selector;
	var <>prototype;
	
	// level limitiations ?	
	
	*new { arg prototype;
		^super.new.prototype_(prototype)
	}

	default { ^prototype.deepCopy }
	canKr { ^false }
	
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


SampleSpec : Spec {

	*initClass {
		specs.putAll(
		 IdentityDictionary[
			\sample -> this.new			
			];
		)
	}
	rate { ^\scalar }
	default { ^Sample.newClear(16) } // silent sample

}
