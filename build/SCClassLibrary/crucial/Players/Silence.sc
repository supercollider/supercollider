
Silence : SynthlessPlayer { 
	
}


PlayerInputProxy : Silence { // audio

	var <>spec,<>initValue;
	var <patchIn;
	var inBus,nullBus;
	
	*new { arg spec;
		^super.new.spec_(spec.asSpec).pipinit
	}
	pipinit {
		patchIn = PatchIn.newByRate(spec.rate);
	}
	setInputBus { arg bus;
		if(nullBus.notNil and: {nullBus.index.notNil},{ nullBus.free });
		inBus = bus;
	}
	setNodeControl { arg nc;
		patchIn.nodeControl = nc;
	}
	synthArg {
		^initValue // ? spec.default
	}
	prepareToBundle { arg group,bundle;
		if(initValue.isNil,{
			// should share this
			nullBus = Bus.performList(this.rate,group.server,this.numChannels);
			initValue = nullBus.index; 
		});
		super.prepareToBundle(group,bundle);
	}
	rate { ^spec.rate }
	numChannels { ^spec.tryPerform(\numChannels) ? 1 }
}




ObjectNotFound : Silence {
	
	classvar <>somethingMissing;
	
	var <>missing;
	
	*new { arg path;
		somethingMissing = path;
		^super.new(0.0).missing_(path)
	}

}