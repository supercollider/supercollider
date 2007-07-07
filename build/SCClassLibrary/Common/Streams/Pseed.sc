
//allows to reproduce enclosed randomized pattern
//by setting the random seed of the resulting routine
//julian rohrhuber, september 01

Pseed : FilterPattern {
	var <>randSeed;
	*new { arg randSeed, pattern;
		^super.new(pattern).randSeed_(randSeed)
	}
	storeArgs { ^[randSeed,pattern] }
	
	embedInStream { arg inval;
		var seedStream;
		var seed, thread;
		seedStream = randSeed.iter;

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

// classical indian scale pattern. no special pakads (movements) or vakras (twists) are applied.
// the pakad is often a natural consequence of the notes of arohana / avarohana 
// (ascending and descending structures).  This is the purpose of this pattern
// jrh 2003


Pavaroh : FilterPattern {
	
	var <>aroh, <>avaroh, <>stepsPerOctave;
	*new { arg pattern, aroh, avaroh, stepsPerOctave=12;
		^super.newCopyArgs(pattern, aroh, avaroh, stepsPerOctave)
	
	}
	storeArgs { ^[pattern, aroh, avaroh, stepsPerOctave ] }
	
	embedInStream { arg inval;
		var me, melast = 0, scale;
		var mestream = pattern.asStream;

		while {
			(me = mestream.next(inval)).notNil
		} {
			scale = if(me >= melast) { aroh } { avaroh };
			melast = me;
			inval = me.degreeToKey(scale, stepsPerOctave).yield
		};
		^inval

	}
	
}




