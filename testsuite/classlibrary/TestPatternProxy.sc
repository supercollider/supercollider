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


	test_update {
		var assert = { |what, which|
			this.assert(what, "testing '%' failed.".format(which), false)
		};

		Pdefn.clear;
		Pdefn(\x, Pseq([1, 2, 3]));

		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, nil], 1.1);
		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, nil], 1.2);
		Pdefn(\x, Pseq([1, 2, 3], inf));
		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, 1], 1.3);
		Pdefn(\x, Pseq([1, 2, 3]));
		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, nil], 1.4);

		Pdefn.clear;
		Pdefn(\x, 7);
		assert.(Pdefn(\x).asStream.nextN(2) == [7, 7], 2.1);
		assert.(Pdefn(\x).asStream.nextN(2) == [7, 7], 2.2);
		assert.(Pseq([Pdefn(\x), 8]).asStream.nextN(2) == [7, 8], 2.3);
		assert.(Pnsym(Pseq([\x, \y, \x])).asStream.nextN(4) == [7, Pdefn.default, 7, nil], 2.4);
		assert.(Pdefn(\zz).asStream.nextN(2) == Pdefn.default.dup(2), 2.5);

		Pdefn(\x, 7);
		assert.(Pdefn(\x).asStream.nextN(2) == 7.dup(2), 2.6);
		Pdefn(\x, Pseq([1, 2, 3], inf));
		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, 1], 2.7);
		Pdefn(\x, 8);
		assert.(Pdefn(\x).asStream.nextN(2) == 8.dup(2), 2.8);
		assert.(Pnsym(Pseq([\x, \y, \x])).asStream.nextN(4) == [8, Pdefn.default, 8, nil], 2.9);
		Pdefn(\y, Pseq([1, 2, 3], 1));
		assert.(Pnsym(Pseq([\x, \y, \x])).asStream.nextN(5) == [8, 1, 2, 3, 8], 2.10);

		Pdefn(\y, -1);
		assert.(Pnsym(Pseq([\x, \y], 2)).asStream.nextN(5) == [8, -1, 8, -1, nil], 2.11);
		Pdefn(\y, 1);
		assert.(Pnsym(Pseq([\x, \y], 2)).asStream.nextN(5) == [8, 1, 8, 1, nil], 2.12);

		Pdefn.clear;
		Pdefn(\x, 7);
		Pdefn(\y, Pseq([10, 20], inf));
		assert.(Pnsym1(Pseq([\x, \y, \x, \y], inf)).asStream.nextN(7) == [7, 10, 7, 20, 7, 10, 7], 2.13);
		assert.(Pnsym1(Pseq([\x, \y, \x, \y], 1)).asStream.nextN(7) == [7, 10, 7, 20, nil, nil, nil], 2.14);

		Pdefn.clear;
		Pdefn(\x, Set[1, 2, 3]);
		assert.(Pdefn(\x).asStream.nextN(4) == Set[1, 2, 3].dup(4), 3.1);
		Pdefn(\x, { Pseq([1, 2, 3]) }); // functions expand
		assert.(Pdefn(\x).asStream.nextN(4) == [1, 2, 3, nil], 3.2);
		Pdefn(\x).set(\first, 7);
		Pdefn(\x, { |envir| Pseq([envir[\first], 2, 3]) }); // functions expand and take envir as arg
		assert.(Pdefn(\x).asStream.nextN(4) == [7, 2, 3, nil], 3.3);

		Pdefn.clear;
	}

	test_update_withEvents {

		var x, a;
		var removeCleanup = { |event| event !? { event.removeAt(\addToCleanup) }; event };
		var assert = { |a, b, which|
			//a = a.collect(removeCleanup);
			//b = b.collect(removeCleanup);
			this.assert(a == b, "testing '%' failed.".format(which), false)
		};


		Pdef.clear;
		this.assert(Pdef(\x).player == nil, "an empty Pdef shouldn't have a player", false);

		Pdef(\x, (freq: 700));

		assert.(
			Pevent(Pdef(\x)).asStream.nextN(2),
			[ ( 'freq': 700 ), ( 'freq': 700 ) ],
			1.1
		);


		assert.(
			Pevent(Pdef(\x)).asStream.nextN(2),
			[ ( 'freq': 700 ), ( 'freq': 700 ) ],
			1.2
		);

		Pdef(\x, (freq: 800));

		assert.(
			Pevent(Pdef(\x)).asStream.nextN(2),
			[ ( 'freq': 800 ), ( 'freq': 800 ) ],
			2.1
		);


		Pdef(\y, (freq: 100));


		assert.(
			Pevent(Psym(Pseq([\x, \y]))).asStream.nextN(3),
			[ ( 'freq': 800 ), ( 'freq': 100 ), nil ],
			2.2
		);
		assert.(
			Pevent(Psym(Pseq([\x, \y]))).asStream.nextN(3),
			[ ( 'freq': 800 ), ( 'freq': 100 ), nil ],
			2.3
		);

		Pdef(\y, (freq: 200));
		assert.(
			Pevent(Psym(Pseq([\x, \y]))).asStream.nextN(3),
			[ ( 'freq': 800 ), ( 'freq': 200 ), nil ],
			3.1
		);


		Pdef(\y, Pn((freq: 200)));

		assert.(
			Pevent(Psym(Pseq([\x, \y]))).asStream.nextN(3),
			[ ( 'freq': 800 ), ( 'freq': 200 ),  ( 'freq': 200 ) ],
			3.2
		);

		Pdef(\y, (freq: 200));
		assert.(
			Pevent(Psym(Pseq([\x, \y]))).asStream.nextN(3),
			[ ( 'freq': 800 ), ( 'freq': 200 ), nil ],
			3.3
		);



		// endless:
		Pdef.clear;
		this.assert(Pdef.defaultValue == Event.silent,
			"Pdef default value should be Event.silent", false);
		Pdef(\x, (freq: 800));

		x = Pevent(Pdef(\x).endless).asStream;
		a = Pdef.defaultValue;

		assert.(x.nextN(3), [(freq: 800), a, a], 4.1);

		Pdef(\x, (freq: 900).loop);
		assert.(x.nextN(3), [ (freq: 900), (freq: 900), (freq: 900) ], 4.2);

		Pdefn.clear;
		this.assert(Pdefn.defaultValue == 1, "Pdefn default value should be 1", false);

		Pdefn(\x, 9);
		x = Pdefn(\x).endless.asStream;
		a = Pdefn.defaultValue;
		assert.(x.nextN(3), [9, a, a], "Pdefn should return default value when empty");
		Pdefn(\x, Pseq([1, 2, 3]));
		assert.(x.nextN(3), [1, 2, 3], "Pdefn should update when full");


		// Pbindef
		Pdef.clear;
		Pbindef(\p, \z, 7);
		assert.(
			Pevent(Pbindef(\p)).asStream.nextN(3),
			[ ( 'z': 7 ), ( 'z': 7 ), ( 'z': 7 ) ],
			5.1
		);
		Pbindef(\p, \z, Pn(7, 2));
		assert.(
			Pevent(Pbindef(\p)).asStream.nextN(3),
			[ ( 'z': 7 ), ( 'z': 7 ), nil ],
			5.2
		);
		Pbindef(\p, \z, 7);

		assert.(
			Pevent(Pbindef(\p)).asStream.nextN(3),
			[ ( 'z': 7 ), ( 'z': 7 ), ( 'z': 7 ) ],
			5.3
		);
		Pbindef(\p, \z, nil);

		assert.(
			Pevent(Pbindef(\p)).asStream.nextN(3),
			().dup(3),
			5.4
		);

		Pdefn.clear;

		Pdefn(\x, 9);
		Pdefn(\y, Pseq([1, 2, 3]));
		Pdefn(\z, Pdefn(\x) + Pdefn(\y));
		assert.(
			Pdefn(\z).asStream.nextN(4),
			[ 10, 11, 12, nil ],
			6.1
		);

		Pdefn(\x, -1);
		assert.(
			Pdefn(\z).asStream.nextN(4),
			[ 0, 1, 2, nil ],
			6.2
		);


		// testing quant
		Pdefn.clear;
		Pdef.clear;
		Pdefn(\m, 12);
		Pdefn(\m).quant = 2;
		Pbindef(\x,\midinote, Pseq([60,62,64,65,67,65,64,62], inf) + Pdefn(\m));

		assert.(
			Pevent(Pbindef(\x)).asStream.nextN(3),
			[ ( 'midinote': 72 ), ( 'midinote': 74 ), ( 'midinote': 76 ) ],
			7.1
		);

		Pbindef(\x,\dur, 0.5);
		assert.(
			Pevent(Pbindef(\x)).asStream.nextN(3),
			[ ( 'dur': 0.5, 'midinote': 72 ), ( 'dur': 0.5, 'midinote': 74 ), ( 'dur': 0.5, 'midinote': 76 ) ],
			7.2
		);

		Pdefn(\m, 11);
		assert.(
			Pevent(Pbindef(\x)).asStream.nextN(3),
			[ ( 'dur': 0.5, 'midinote': 71 ), ( 'dur': 0.5, 'midinote': 73 ), ( 'dur': 0.5, 'midinote': 75 ) ],
			7.3
		);

		Pdefn.clear;
		Pdef.clear;



	}
}
