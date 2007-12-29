
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



/*
	this is a placeholder Player object.
	if you try to load something and there is nothing at the path,
	this returns a silent player to take its place.
	this solves the problem when you are trying to load many players at the same time
	and its annoying when one of them is missing, but should not be fatal.
	eg. when you are playing a live show.
*/

ObjectNotFound : Silence {

	classvar <>somethingMissing;

	var <>missing;

	*new { arg path;
		somethingMissing = path;
		("ObjectNotFound : " ++ path ).warn;
		^super.new(0.0).missing_(path).path_(path)
	}
	storeArgs { ^[missing] }

}

