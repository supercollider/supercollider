
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
		seedStream = randSeed.asStream;

		while {
			seed = seedStream.next;
			seed.notNil
		}{
			thread = pattern.asStream;
			thread.randSeed = seed;
			inval = thread.embedInStream(inval);
		};
		^inval
	}

}

// cx: temporarily putting this here

PdegreeToKey : FilterPattern {
	
	var <>scale,<>stepsPerOctave;
	
	*new { arg pattern,scale, stepsPerOctave=12;
		^super.new(pattern).scale_(scale).stepsPerOctave_(stepsPerOctave)
	}
	storeArgs { ^[pattern,scale,stepsPerOctave ] }
	embedInStream { arg inval;
		var size, scaleDegree;
		var mestream,scstream,sc,me;
		if(scale.isSequenceableCollection,{

			size = scale.size;
			mestream = pattern.asStream;
			while({ 
				(me = mestream.next).notNil
			},{
				me = me.asInteger;
				inval = ((stepsPerOctave * (me div: size)) + scale.wrapAt(me)).yield
			});
			^inval
		},{		
			
			mestream = pattern.asStream;
			scstream = scale.asStream;
			while({ 
				me = mestream.next;
				sc = scstream.next;
				me.notNil
			},{
				inval = me.degreeToKey(sc,stepsPerOctave).yield
			})
			^inval;
		});
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
		var mestream, me, melast, scale, size;
		mestream = pattern.asStream;
		melast = 0;
		while({ 
			(me = mestream.next).notNil
		},{
			me = me.asInteger;
			scale = if(me >= melast, { aroh }, { avaroh });
			melast = me;
			size = scale.size;
			inval = ((stepsPerOctave * (me div: size)) + scale.wrapAt(me)).yield
		});
		^inval

	}
	
	
}



