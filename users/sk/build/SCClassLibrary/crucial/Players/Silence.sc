
Silence : SynthlessPlayer { 
	
}


PlayerInputProxy : Silence { // audio

	var <>spec,<>initValue = 0,>numChannels;
	var <patchIn;
	var inBus;
	
	*new { arg spec=\audio;
		^super.new.spec_(spec.asSpec).pipinit
	}
	pipinit {
		patchIn = PatchIn.newByRate(spec.rate);
	}
	setInputBus { arg bus;
		inBus = bus;
		initValue = inBus.index;
	}
	setNodeControl { arg nc;
		patchIn.nodeControl = nc;
	}
	synthArg {
		^initValue
	}
//	prepareToBundle { arg group,bundle;
//		if(initValue.isNil,{
//			// should share this
//			nullBus = Bus.performList(this.rate,group.server,this.numChannels);
//			initValue = nullBus.index; 
//		});
//		super.prepareToBundle(group,bundle);
//	}
	makePatchOut {
		// bus is always given to me
		patchOut = PatchOut(this,nil,inBus);
	}
	rate { ^spec.rate }
	numChannels { ^(numChannels ?? {spec.tryPerform(\numChannels) ? 1}) }
}




ObjectNotFound : Silence {
	
	classvar <>somethingMissing;
	
	var <>missing;
	
	*new { arg path;
		somethingMissing = path;
		(path ++ " NOT FOUND !! ").warn;
		^super.new(0.0).missing_(path).path_(path)
	}
	storeArgs { ^[missing] }

}