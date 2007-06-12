
ControlPrototypes {
	
	classvar <registery;
	
	*initClass {
		registery=IdentityDictionary.new;
		
		// define at least for each spec (by class)
		this.define(
			// a function that returns a list of prototypes
			ControlSpec -> {[
							KrNumberEditor.new
					]},
			StaticSpec -> {[
							NumberEditor.new
						]},
						
			EnvSpec -> {[
						EnvEditor.new(Env.adsr)
					]},
			
			AudioSpec -> {[
						Silence(0.0) // silence
							]},
			TrigSpec -> {[	
						BeatClockPlayer.new(8)
					]},
			SampleSpec -> {[
						Sample.new
						]},
			TempoSpec -> {[
							TempoPlayer.new
						]}
						
						//,
			//StreamSpec -> {[ AsStream( NumberEditor.new ) ]}
		);
	}
	
	*define { arg ... assns;
		registery.addAll(assns);
	}
	
	*at { arg key; // returns a list of possibles
		^registery.at(key).value // could/should be a function to create list of possibles
	}
	
	*firstAt { arg key;
		var func,proto;
		func=registery.at(key);
		^if(func.notNil,{ 
			proto=func.value.first;
			// could wipe out any data we set for it
			//proto.tryPerform('spec_',spec);
			//proto.tryPerform('value_',spec.default);
			proto
		},{
			nil
		})
	}
	
	*chooseAt { arg key;
		var func;
		func=registery.at(key);
		^if(func.notNil,{ 
			func.value.choose
			//proto.tryPerform('spec_',spec);
			//proto.tryPerform('value_',spec.default);
		},{
			nil
		})
	}
	*forSpec { arg spec,argName;
		// by spec name
				^(	//this.firstAt(spec.name)
					// by instrument arg name
					//?? {
					this.firstAt(argName)
					//}
					// by the class of the spec
					?? {this.firstAt(spec.class)}
				)	
	}				
}	
			

