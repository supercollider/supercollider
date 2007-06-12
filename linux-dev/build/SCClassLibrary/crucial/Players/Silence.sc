
Silence : SynthlessPlayer { 
	
}


PlayerInputProxy : Silence { // audio

	var <>numChannels,<>rate;
	var inBus,nullBus;
	
	*new { arg numChannels=1,rate=\audio;
		^super.new.pipinit(numChannels,rate)
	}
	pipinit { arg n,r;
		numChannels = n;
		rate = r;
	}
	setInputBus { arg bus;
		if(nullBus.notNil and: {nullBus.index.notNil},{ nullBus.free });
		inBus = bus;
	}
	synthArg {
		var b;
		b = (inBus ? nullBus);
		// called during synth def build just for the default value
		// when actually played, we have a nullBus
		^if(b.notNil,{b.index},nil);
	}
	prepareToBundle { arg group,bundle;
		if(inBus.isNil,{ 
			// should share this
			nullBus = Bus.performList(this.rate,group.server,this.numChannels) 
		});
		super.prepareToBundle(group,bundle);
	}
	
}




ObjectNotFound : Silence {
	
	classvar <>somethingMissing;
	
	var <>missing;
	
	*new { arg path;
		somethingMissing = path;
		^super.new(0.0).missing_(path)
	}

}