
ControlPrototypes {

	classvar <registery;

	*initClass {
		registery=IdentityDictionary.new;
	}

	*define { arg ... assns;
		registery.addAll(assns);
	}

	*at { arg key,spec; // returns a list of possibles
		^registery.at(key).value(key,spec) // could/should be a function to create list of possibles
	}
	*forSpec { arg argName,spec;
		// by spec name
		^(	// by instrument arg name
			//?? {
			this.firstAt(argName,spec)
			//}
			// by the class of the spec
			?? {this.firstAt(spec.class,spec)}
		)
	}

	*firstAt { arg argName,spec;
		var func,proto;
		func=registery.at(argName);
		^if(func.notNil,{
			proto=func.value(argName,spec).first;
			// could wipe out any data we set for it
			//proto.tryPerform('spec_',spec);
			//proto.tryPerform('value_',spec.default);
			proto
		},{
			nil
		})
	}

	*chooseAt { arg argName,spec;
		var func;
		func=registery.at(argName);
		^if(func.notNil,{
			func.value(argName,spec).choose
			//proto.tryPerform('spec_',spec);
			//proto.tryPerform('value_',spec.default);
		},{
			nil
		})
	}
	*chooseForSpec { arg argName,spec;
		// by spec name
				^(	//this.firstAt(spec.name)
					// by instrument arg name
					//?? {
					this.chooseAt(argName,spec)
					//}
					// by the class of the spec
					?? {this.chooseAt(spec.class,spec)}
					?? {spec.defaultControl}
				)
	}
	*listForSpec { arg argName,spec;
		var try,class,specname;
		try = this.at(argName,spec);
		if(try.notNil,{ ^try });

		specname = Spec.specs.findKeyForValue(spec);
		if(specname.notNil,{
			try = this.at(specname,spec);
			if(try.notNil, { ^try });
		});

		class = spec.class;

		while({
			try.isNil and: class != Object
		},{
			try = this.at(class,spec);
			class = class.superclass;
		});
		^spec.defaultControl
	}
}


