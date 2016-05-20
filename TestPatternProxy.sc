TestPatternProxy : UnitTest {

	test_transparency {

		var runTest = { |f, source, firstSource|
			var n = 16;
			var a, b, proxy, pattern, errorString;
			var resultA, resultB;
			proxy = PatternProxy.new;
			a = f.(proxy).asStream;
			if(firstSource.notNil) {
				proxy.source = firstSource;
				n.do { a.next }
			};
			proxy.source = source;
			b = f.(source).asStream;
			resultA = Array.fill(n, { a.next });
			resultB = Array.fill(n, { b.next });
			errorString = "The function % should behave the same for a PatternProxy and its source:\n%\n".format(f.cs, source.cs);
			if(firstSource.notNil) { errorString = errorString ++ "firstSource: " ++ firstSource.cs };
			errorString = errorString ++ "\ncompare results:\n%\n%\n\n".format(resultA, resultB);

			this.assert(resultA == resultB, errorString, false)

		};

		var functions = [
			{ |x| x },
			{ |x| Pseq([x, 2, 3]) },
			{ |x| Pn(x, 3) },
			{ |x| Pswitch([10, 20, 30], x) },
			{ |x| Pswitch([10, x, 30], Pseq([0, 1, 2], 2)) },
			{ |x| Pfin(6, x) },
			{ |x| Pn(Pfin(4, x)) },
		];

		var sources = [
			1,
			Pseq([10], 2)
		];

		var firstSources = [
			nil,
			//Pseq([-1], 3), // for these, tests need to be thought through
			//Pseq([-1], inf)
		];

		functions.do { |f|
			sources.do { |source|
				firstSources.do { |firstSource|
					runTest.(f, source, firstSource)
				}
			}
		}

	}


	test_transparency_withEvents {

		var runTest = { |f, source|
			var n = 16;
			var a, b, proxy, pattern, errorString;
			var resultA, resultB;
			var removeCleanup = { |event| event !? { event.removeAt(\addToCleanup) }; event };

			proxy = EventPatternProxy.new;
			proxy.source = source;
			a = Pevent(f.(proxy), Event.default).asStream;
			b = Pevent(f.(proxy.source), Event.default).asStream;
			resultA = Array.fill(n, { a.next }).collect(removeCleanup);
			resultB = Array.fill(n, { b.next }).collect(removeCleanup);
			errorString = "The function % should behave the same for a PatternProxy and its source:\n%\n".format(f.cs, source.cs);
			errorString = errorString ++ "\ncompare results:\n%\n%\n\n".format(resultA, resultB);

			this.assert(resultA == resultB, errorString, false)

		};

		/*
		the ones commented out need to be checked
		Pcollect crashes the interpreter
		*/

		var functions = [
			{ |x| x },
			{ |x| Pseq([x, x]) },
			{ |x| Pseq([x, (y: 10)]) },
			//{ |x| Pselect({ |event| event[\zz].notNil }, x) },
			{ |x| Pcollect({ |event| event[\zz] = 100 }, x) },
			{ |x| Pfset({ ~gg = 8; ~zz = 9; }, x) },
			{ |x| Psetpre({ ~gg = 8; ~zz = 9; }, x) },
			{ |x| Ppar([x, x]) },
			{ |x| Pfin(3, x) },
			{ |x| Pfin(6, x) },
			{ |x| Pchain(x, Pseq([(zz: 9), (yy: 10), (zz: 10, yy: 2)], inf)) },
			{ |x|
				Pchain(
					Pbindf(x,
						\xx, Pseq([1, 2]),
						\yy, Pfunc { |event| event[\zz] }
					),
					Pbind(\zz, Pseries())
				)
			},
			{ |x| Pfindur(2, x).loop },
			{ |x| Ppar([x, Plazy { x }]) },
			{ |x| Psym(Pseq([\a, \b], 2), (a: x), (b: Pfin(2, x))) },
			{ |x| Psym1(Pseq([\a, \b], 2), (a: x), (b: Pfin(2, x))) }
		];

		var sources = [
			(x: 9),
			Pbind.new,
			Pbind(\x, Pseq([1, 2, 3]), \dur,  Pseq([1, 0.5, 0.5], inf)),
			Pseq([(), Event.default, (zz: 300), (zz: 500)], 2),
			Pseq([(zz: 300), (zz: 500, yy: 9, dur: 0.5)], 1),
			Plazy({ Plazy({ (zz: 99) }, 4) })
		];

		functions.do { |f|
			sources.do { |source|
				runTest.(f, source)
			}
		}

	}

}
