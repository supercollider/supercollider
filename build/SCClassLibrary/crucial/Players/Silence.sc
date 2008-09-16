/*
Silence is a player that doesn't have any output at all.
*/
Silence : SynthlessPlayer {
	*ar { |...args| ^Silent.ar(*args) }
}


PlayerInputProxy : Silence { // audio

	var <>spec,<>initValue = 0;
	var <patchIn;
	var inBus;

	*new { arg spec=\stereo; // \mono, \stereo
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

	makePatchOut { arg agroup,private = false,bus,bundle;
		// bus is usually given to me via setInputBus
		super.makePatchOut(agroup,private,inBus ? bus,bundle)
		//patchOut = PatchOut(this,nil,inBus);
	}
	rate { ^spec.rate }
	numChannels { ^(spec.numChannels) }
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

