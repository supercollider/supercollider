
// allows to reproduce enclosed randomized pattern
// by setting the random seed of the resulting routine

Pseed : FilterPattern {
	var <>randSeed;
	*new { arg randSeed, pattern;
		^super.new(pattern).randSeed_(randSeed)
	}
	storeArgs { ^[randSeed,pattern] }

	embedInStream { arg inval;
		var seedStream;
		var seed, thread;
		seedStream = randSeed.asStream;

		while {
			seed = seedStream.next(inval);
			seed.notNil
		}{
			thread = pattern.asStream;
			thread.randSeed = seed;
			inval = thread.embedInStream(inval);
		};
		^inval
	}

}