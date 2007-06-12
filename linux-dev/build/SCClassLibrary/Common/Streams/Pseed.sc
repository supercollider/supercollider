
//allows to reproduce enclosed randomized pattern
//by setting the random seed of the resulting routine//julian rohrhuber, september 01
Pseed : FilterPattern {	var <>randSeed;	*new { arg randSeed, pattern;		^super.new(pattern).randSeed_(randSeed)	}	asStream {		var seedStream;		seedStream = randSeed.asStream;		^Routine({ arg inval;			var seed;			while({				seed = seedStream.next;				seed.notNil				}, {				thisThread.randSeed = seed;				pattern.embedInStream(inval);			})		});	}}