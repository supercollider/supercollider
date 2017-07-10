TestAbstractFunction : UnitTest {

	test_rest_binop_return {
		var args = [[1, Rest(1)], [Rest(1), 1], [Rest(1), Rest(1)], [1, Rest()], [Rest(), Rest()]];
		var funcs = [
			{ |a, b| a * b },
			{ |a, b| a + b },
			{ |a, b| a - b },
			{ |a, b| a / b }
		];

		funcs.do { |func|
			var passed = args.every { |pair|
				var res = func.valueArray(pair);
				var ok = res.isKindOf(Rest);
				if(ok.not) {
					this.failed(thisMethod, "Calling % on % should return a Rest instance".format(func, pair));
				};
				ok
			};

			this.assert(passed, "Rest should combine to Rest with objects and other Rest instances under binary ops");
		}

	}


	test_rest_binop_values {
		var args = [[2, Rest(3)], [Rest(2), 3], [Rest(2), Rest(3)]];
		var funcs = [
			{ |a, b| a * b },
			{ |a, b| a + b },
			{ |a, b| a - b },
			{ |a, b| a / b }
		];

		funcs.do { |func|
			var passed = args.every { |pair|
				var res = func.valueArray(pair);
				var res2 = func.valueArray(pair.collect(_.value));
				var ok = res.value == res2;
				if(ok.not) {
					this.failed(thisMethod, "Calling % on % should have the same result as its values wold have".format(func, pair));
				};
				ok
			};

			this.assert(passed, "Rest should combine under binary ops to the same values as the values would");
		}

	}

	test_rest_comparisons {

		var func = { |op, a, b| a.perform(op, b) == a.value.perform(op, b.value) };
		var all = [(0..2), (0..2).collect(Rest(_))].allTuples;
		all = all ++ all.collect(_.reverse);
		['<', '>', '<=', '>='].do { |op|
			all.do { |args|
				this.assert(func.(op, *args), "% should return a boolean for %".format(args, op));
			}
		}
	}

	test_rest_event_delta {

		this.assertEquals(Rest(),  Rest(1), "Rest default value should be 1");

		this.assertEquals(
			(degree: Rest(0), parent: Event.default.parent).use { ~freq.value },
			Rest(60.midicps),
			"rest passes through event calculations"
		);

		this.assertEquals(
			(degree: Rest(7), octave: Rest(4), parent: Event.default.parent).delta,
			1,
			"delta is not touched by venet calculations"
		);

		this.assertEquals((dur: 1).delta, 1, "delta matches dur");

		this.assertEquals((dur: Rest(1)).delta, 1, "delta matches dur with rest");

		this.assertEquals((dur: Rest(1), degree: Rest(7)).delta, 1, "delta matches dur with rest if other keys are rest, too");


		this.assertEquals((dur: 1, stretch: 2).delta, 2, "delta matches dur when stretched");

		this.assertEquals((dur: Rest(1)).delta, 1, "delta matches dur with rest");

		this.assertEquals((dur: 1, stretch: Rest(2)).delta, 2, "delta matches dur with rest when stretched with number");

		this.assertEquals((dur: Rest(1), stretch: Rest(2)).delta, 2, "delta matches dur with rest when stretched with rest");

		this.assertEquals((dur: 1, stretch: Rest(2)).delta, 2, "delta number matches dur when stretched with rest");

		this.assertEquals((delta: Rest(1)).delta, 1, "delta matches dur with rest");

		this.assert((dur: nil).delta.isNil, "event delta matches dur value when dur is nil");

		this.assert((delta: nil).delta.isNil, "event delta matches delta value when dur is nil");

		this.assertEquals((delta: Rest(1), stretch: 2).delta, 1,  "stretch applies to dur, not delta");



		try { (delta: Point(0, 0)).delta } { |err|
			this.assert(err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." },
				"using a Point for delta should fail")
		};



		try { (dur: Point(0, 0)).delta } { |err|
			this.assert(err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." },
				"using a Point for dur should fail")
		};



		try { (delta: Rest("boo!")).delta } { |err|
			this.assert(err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." },
				"using a Rest with a string for delta should fail")
		};



		try { (dur: Rest("boo!")).delta } { |err|
			this.assert(err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." },
				"using a Rest with a string for dur should fail")
		};
	}

	test_rest_event_isRest {

		this.assertEquals((dur: 1).isRest, false, "event with dur = 1 should return false for isRest");

		this.assertEquals((dur: Rest(1)).isRest, true, "event with dur = Rest(1) should return true for isRest");

		this.assertEquals((type: \rest).isRest, true, "event with dur = \rest should return true for isRest");

		this.assertEquals((degree: \).isRest, true, "event with an empty symbol as dur should return true for isRest");

		this.assertEquals((x: \).isRest, true, "event with an empty symbol in arbitrary key should return true for isRest");

		this.assertEquals((x: \r).isRest, true, "event with an 'r' symbol in arbitrary key should return true for isRest");

		this.assertEquals((x: \rest).isRest, true, "event with an 'rest' symbol in arbitrary key should return true for isRest");

		#[\dur, \degree, \paraplui, \type].do { |key|
			var a, b, c, f;

			a = Pbind(key, Pseries());
			b = Pcollect({ |e| e[key] = Rest(e[key]) }, a);
			c = Pbindf(b, key, Pfunc { |e| e[key].value });

			f = { |p| Pevent(Pmul(key, 2, p)) };

			this.assert(Pevent(b).asStream.nextN(8).every(_.isRest), "a rest in key '%' should be a rest".format(key));
			this.assertEquals(f.(a).asStream.nextN(8), f.(c).asStream.nextN(8),
				"rest values remain intact for key '%' after binary op".format(key));
		};

	}

}

