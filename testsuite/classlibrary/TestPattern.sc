
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


	test_pattern_zero_length {
		var func, patterns;

		func = { |pat|
			var val = Pseq([pat, 1]).asStream.next(());
			this.assert(val == 1,
				"% : a pattern of length zero should return nothing but pass control (returned %)".format(pat, val)
			)
		};
		patterns = [
			Pfuncn({ 2 }, 0),
			Pseries(length:0),
			Pgeom(length:0),
			Pbrown(length:0),
			Pgbrown(length:0),
			Pwhite(length:0),
			Pmeanrand(length:0),
			Plprand(length:0),
			Phprand(length:0),
			Pexprand(length:0),
			Ppoisson(length:0),
			Pcauchy(length:0),
			Pbeta(length:0),
			Pgauss(length:0),
			Pprob([1, 2, 3], length:0),
			Ptime(repeats:0),
			Pkey(repeats:0),
			Pseq([20, 30], repeats:0),
			Pseg([20, 30], repeats:0),
			Pstep([20, 30], repeats:0),
			Pser([20, 30], repeats:0),
			Pshuf([20, 30], repeats:0),
			Prand([20, 30], repeats:0),
			Pxrand([20, 30], repeats:0),
			Pwrand([20, 30], repeats:0),
			Pfsm([20, 30], repeats:0),
			Pdfsm([20, 30], repeats:0),
			Ptuple([20, 30], repeats:0),
			Place([20, 30], repeats:0),
			Ppatlace([20, 30], repeats:0),
			Pslide([20, 30], repeats:0),
			Pindex(repeats:0),
			Pevent(Pget(\a, 20, repeats:0), (eventScope: ())),
			Pgate(repeats:0),
			Pn(repeats:0),
			Pdict((a: 20), Pn(\a), repeats:0),
			Peventmod({ ~a = ~a + 1 }, (a: 20), repeats:0),
			Ppar([Pbind.new], repeats:0),
			Pgpar([Pbind.new], repeats:0),
			Ptpar([0.0, Pbind.new], repeats:0),
			Pfpar([Pbind.new], repeats:0),

		];

		patterns.do(func)

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

