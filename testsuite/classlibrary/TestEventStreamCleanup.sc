TestEventStreamCleanup : UnitTest {

	test_cleanupsNotDuplicatedOrLost {

		var tryExhaustPstream = { |pattern, inEvent = (())|
			var cleanup = EventStreamCleanup.new, outEvent, stream = pattern.asStream;
			var hist = List().add(cleanup.functions.size);
			while {
				outEvent = stream.next(inEvent);
				cleanup.update(inEvent);
				hist.add(cleanup.functions.size);
				hist.size < 100 && outEvent.notNil // well, exhaust if it's not infinite
			};
			[hist, inEvent] // retval not used presently (except in manual debugging)
		};
		var injectAndCountCleanupsVsInits = { |patFunc, inEvent = (())|
			var initCount = 0, cleanupCount = 0;
			var injector = Pfset(
				{ initCount = initCount + 1 },
				p { |ev| ev.yield },
				{ cleanupCount = cleanupCount + 1 });
			tryExhaustPstream.(patFunc.(injector), inEvent);
			[initCount, cleanupCount]
		};
		// pattern-generating functions, x substituted for cleanup injector
		var patternGenFuncs = [
			{ |x| Pn(x, 1) },
			{ |x| Pn(x, 3) },
			{ |x| Pn(x, 0) },
			{ |x| Pchain(x) },
			{ |x| Pchain(x, x) },
			{ |x| Pcollect({ |ev| ev }, x) },
			{ |x| Pcollect({ |ev| ev.copy }, x) },
			{ |x| Pseq([x]) },
			{ |x| Pseq([x, x]) },
			{ |x| Pseq([x, (y: 10)]) },
			{ |x| Psetpre(\gg, 8, x) },
			// the following expected to fail until Psetpre is fixed
			//{ |x| Psetpre(\hh, Pseq([nil]), x) },
			//{ |x| Psetpre(\xx, x, Pseq([nil])) },
			//{ |x| Psetpre(\zz, x, Pseq([ () ])) },
		];
		var patternGenFuncsDur = [ // these need a duration pre-set externally
			{ |x| Ppar([x, Pseq([ () ])]) },
			{ |x| Ppar([x, x]) },
			{ |x| Ppar([x, Plazy { x }]) },
		];

		patternGenFuncs.do { |pgf|
			var cntPairs = injectAndCountCleanupsVsInits.(pgf);
			this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
				pgf.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
		};
		patternGenFuncs.do { |pgf|
			patternGenFuncs.do { |pgf2|
				var cntPairs = injectAndCountCleanupsVsInits.(pgf <> pgf2);
				this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
					// (pgf <> pgf2).cs doesn't print anything useful
					pgf.cs + "<>" + pgf2.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
			};
		};
		patternGenFuncsDur.do { |pgf|
			var cntPairs = injectAndCountCleanupsVsInits.(pgf, (dur: 0.2));
			this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
				pgf.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
		};
		patternGenFuncsDur.do { |pgf|
			patternGenFuncs.do { |pgf2|
				var cntPairs = injectAndCountCleanupsVsInits.(pgf <> pgf2, (dur: 0.2));
				this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
					pgf.cs + "<>" + pgf2.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
			};
		};
		patternGenFuncs.do { |pgf|
			patternGenFuncsDur.do { |pgf2|
				var cntPairs = injectAndCountCleanupsVsInits.(pgf <> pgf2, (dur: 0.2));
				this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
					pgf.cs + "<>" + pgf2.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
			};
		};
		patternGenFuncsDur.do { |pgf|
			patternGenFuncsDur.do { |pgf2|
				var cntPairs = injectAndCountCleanupsVsInits.(pgf <> pgf2, (dur: 0.2));
				this.assertEquals(cntPairs[0], cntPairs[1], "Cleanup propagation" +
					pgf.cs + "<>" + pgf2.cs + "inits:" + cntPairs[0] ++ ", cleanups:" + cntPairs[1]);
			};
		};
	}
}
