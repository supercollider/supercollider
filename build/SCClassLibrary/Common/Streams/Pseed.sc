
//allows to reproduce enclosed randomized pattern
//by setting the random seed of the resulting routine
//julian rohrhuber, september 01

Pseed : FilterPattern {
	var <>randSeed;
	*new { arg randSeed, pattern;
		^super.new(pattern).randSeed_(randSeed)
	}
	storeArgs { ^[randSeed,pattern] }
	asStream {
		var seedStream;
		seedStream = randSeed.asStream;
		^Routine({ arg inval;
			var seed;
			while({
				seed = seedStream.next;
				seed.notNil
				}, {
				thisThread.randSeed = seed;
				pattern.embedInStream(inval);
			})
		});
	}
}

// temporarily putting this here

PdegreeToKey : FilterPattern {
	
	var <>scale,<>stepsPerOctave;
	
	*new { arg pattern,scale, stepsPerOctave=12;
		^super.new(pattern).scale_(scale).stepsPerOctave_(stepsPerOctave)
	}
	storeArgs { ^[pattern,scale,stepsPerOctave ] }
	asStream {
		var size, scaleDegree;
		if(scale.isSequenceableCollection,{
			^Routine({
				var mestream,me;
				size = scale.size;
				mestream = pattern.asStream;
				while({ 
					(me = mestream.next).notNil
				},{
					me = me.asInteger;
					((stepsPerOctave * (me div: size)) + scale.wrapAt(me)).yield
				})
			})
		},{		
			^Routine({
				var mestream,scstream,sc,me;
				mestream = pattern.asStream;
				scstream = scale.asStream;
				while({ 
					me = mestream.next;
					sc = scstream.next;
					me.notNil
				},{
					me.degreeToKey(sc,stepsPerOctave).yield
				})
			})
		});
	}
}
