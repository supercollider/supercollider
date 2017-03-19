
TestPattern : UnitTest {

	test_patternAsEventFilters {
		var n = 8;
		var removeCleanup = { |event| event !? { event.removeAt(\addToCleanup) }; event };
		var unfold = { |pat|
			Pevent(pat).asStream.nextN(n).collect(removeCleanup)
		};

		var assertUnfolded = { |x, y|
			var xa, ya;
			xa = unfold.(x);
			ya = unfold.(y);
			this.assert(xa == ya,
				"the following patterns should return the same events:\n%\n%\n"
				"They returned:\n%\n%"
				.format(x.cs, y.cs, xa, ya),
				false

			)
		};
		var identical = [
			Pbind(\x, 7) <> (y:  8),
			Pbind(\y, 8) <> (x:  7),
			Pbind(\x, 7, \y, 8),
			Pn((x: 7, y: 8)),
			Pbindf((y:  8), \x, 7),
			Pbindf((y:  8), \x, Pn(7, 1)).loop,
			Pseq([(x: 7, y: 8)], inf),
			Pfset({ ~x = 7 }, Pbind(\y, 8)),
			Pfset({ ~y = 8 }, Pbind(\x, 7)),
			Pbind(\x, 7).collect { |event| event.put(\y, 8) },
		];

		[identical, identical].allTuples.postln.do { |pair|
			assertUnfolded.(*pair)
		};


	}

/*
	test_storeArgs {
		Pattern.allSubclasses.do({ |class|
			this.assert( class.findMethod('storeArgs').notNil

		})
	}
*/

/*
	var exceptions;
	exceptions = [PatternProxy,PbindProxy,Pbind,PFork,Pprob,Pflow,PlazyEnvirN,ListPattern,Pmono,Pfx,Pbindf,Pslide,Pwrand,Pseq,Pwalk,Pdfsm];

	Pattern.allSubclasses.reject(IsIn(exceptions)).do({ |class|
		var constructor,argNames,decoys,new;
		var storeArgs,asCompileString, recompile;
		// list a few exception classes here

		"".postln;

	//	num of args in constructor,
		constructor = class.class.findMethod('new');
		if(constructor.isNil,{
			"no constructor".debug(class)
		},{
			argNames = constructor.argNames;
			decoys = argNames.copyRange(1,argNames.size-1).as(Array);
			decoys.debug(class);
			if(constructor.argNames.notNil,{
				//yep, we just fed it its own argNames
				new = class.performList('new',decoys);
			});

		//	do storeArgs
			storeArgs = new.storeArgs;
			if((storeArgs == decoys),{
				"TRUE!".debug;
			},{
				storeArgs.debug("failed storeArgs");
			})

	//	numbers should come back the same

	//	do asCompileString
	//	recompile
	//
	//	should be the same
		})

	})
*/


}

