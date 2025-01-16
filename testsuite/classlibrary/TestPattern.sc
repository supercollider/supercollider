
TestPattern : UnitTest {
	var zeroLength;

	setUp {
		zeroLength = [
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
	}

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
			Pbind(x: 7, y: 8),
			Pbind(\x, 7, y: 8),
			Pbind(\y, 8, x: 7),
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

		[identical, identical].allTuples.do { |pair|
			assertUnfolded.(*pair)
		};


	}


	test_pattern_zero_length {
		var func;

		func = { |pat|
			var val = Pseq([pat, 1]).asStream.next(());
			this.assert(val == 1,
				"% : a pattern of length zero should return nothing but pass control (returned %)".format(pat, val)
			)
		};

		zeroLength.do(func)
	}

	test_pattern_embedInStream_returns_inval {
		var stream, proto = Event.new;

		zeroLength.do { |pat|
			stream = Routine { |inval|
				// yield the return value from embedInStream
				// for checking outside the Routine
				pat.embedInStream(inval).yield;
			};
			this.assert(stream.next(proto.copy) == proto,
				"Zero length % should return inval from embedInStream".format(pat.class.name)
			);
		};
	}

	// test bug #3851: Pstretch applied to Ppar should operate on deltas, not only dur
	// bug: Ppar sets event deltas.
	// Pstretch used to miss this and change only the output event's dur, not delta.
	// After Pstretch, dur and delta should be the same.
	test_PstretchDeltas {
		var pattern = Pbind(\dur, Pn(1, 1)),
		ppar = Ppar([pattern], 1),
		pstretch = Pstretch(0.5, ppar),
		event = pstretch.asStream.next(());

		this.assert(
			event.delta == event[\dur] and: { event.delta == 0.5 },
			"Pstretch should stretch deltas of Ppar"
		);
	}

	// the bug did not occur in Pstretchp, but let's test it anyway
	test_PstretchpDeltas {
		var pattern = Pbind(\dur, Pn(1, 1)),
		ppar = Ppar([pattern], 1),
		pstretch = Pstretchp(0.5, ppar),
		event = pstretch.asStream.next(());

		this.assert(
			event.delta == event[\dur] and: { event.delta == 0.5 },
			"Pstretchp should stretch deltas of Ppar"
		);
	}


	test_Pfindur_endsInTime {
		var p, q, x;
		p = Pbind(\dur, 1, \count, Pseries());
		q = Pfindur(3.5, p);
		x = Pevent(q).asStream.all;
		this.assert(x.last[\count] == 3, "Pfindur should end inner pattern after dur");
		this.assert(x.sum { |x| x.delta } == 3.5, "Pfindur with filler should end no sooner than after dur");
	}

	test_Psync_endsInTime {
		var p, q, x;
		p = Pbind(\dur, 1, \count, Pseries());
		q = Psync(p, 3.5, 3.5);
		x = Pevent(q).asStream.all;
		this.assert(x.last[\count] == 3, "Pfindur should end inner pattern after maxdur");
		this.assert(x.sum { |x| x.delta } == 3.5, "Pfindur  with maxdur = quant should end no sooner than after dur");
	}

	test_Psync_fillsRemainingTimeWithSilence {
		var p, q, x;
		p = Pbind(\dur, 1, \count, Pseries(0, 1, 3));
		q = Psync(p, 4.5, 4.5);
		x = Pevent(q).asStream.all;
		this.assert(x.sum { |x| x.delta } == 4.5, "Psync with maxdur = quant should end no earlier than after dur");
	}

	test_Pfset_evaluates_init_and_cleanup_on_empty_stream {
		var x = 0, y = 0, inEvent = (), cleanup = EventStreamCleanup.new;
		var outEvent = Pfset({ x = 1 }, p{}, { y = 2 }).asStream.next(inEvent);
		this.assert(x == 1, "Pfset on nil stream should still call the initializer function");
		this.assert(y == 2, "Pfset on nil stream should still call the cleanup function");
		this.assert(outEvent.isNil, "Pfset on nil stream should return nil");
		// The present implementation of EventStreamCleanup.update first does the add(s) then the remove(s),
		// so it "cancels out" matched add-remove pairs in the same event. But let's check that too...
		cleanup.update(inEvent);
		this.assert(cleanup.functions.size == 0, "Pfset on nil stream should have no effect on a cleanup-functions set");
	}

	test_Pfindur_maintains_final_rest {
		var stream = Pfindur(4, Pbind(\delta, Rest(4))).asStream;
		var event = stream.next(Event.new);
		this.assert(event.isRest, "The final event of a Pfindur, if it was originally a rest, should still be a rest");
	}

	test_Pwalk_boundary_folding {
		var stream = Pwalk((1..6), 2, Pseq([1, -1], inf)).asStream;
		var values = stream.nextN(8);
		this.assertEquals(values, [1, 3, 5, 3, 1, 3, 5, 3],
			"Pwalk applies directionPattern correctly at boundaries"
		);
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
