
ControlPrototypes {
	
	classvar <registery;
	
	*initClass {
		registery=IdentityDictionary.new;
	}
	
	*define { arg ... assns;
		registery.addAll(assns);
	}
	
	*at { arg key; // returns a list of possibles
		^registery.at(key).value // could/should be a function to create list of possibles
	}
	*forSpec { arg spec,argName;
		// by spec name
		^(	// by instrument arg name
			//?? {
			this.firstAt(argName)
			//}
			// by the class of the spec
			?? {this.firstAt(spec.class)}
		)	
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
	*chooseForSpec { arg spec,argName;
		// by spec name
				^(	//this.firstAt(spec.name)
					// by instrument arg name
					//?? {
					this.chooseAt(argName)
					//}
					// by the class of the spec
					?? {this.chooseAt(spec.class)}
					?? {spec.defaultControl}
				)	
	}		
}	
			

