TestAbstractFunction : UnitTest {

	test_rest_binop {
		var func;
		var args = [[1, Rest(1)], [Rest(1), 1], [Rest(1), Rest(1)], [1, Rest()], [Rest(), Rest()]];
		var funcs = [
			{ |a, b| a * b },
			{ |a, b| a + b },
			{ |a, b| a - b },
			{ |a, b| a / b }
		];
		var all = args.collect { |pair|
			funcs.collect { |f| f.valueArray(pair) }
		};
		all.postln;

		this.assert(
			all.flat.every(_.isKindOf(Rest)),
			"All binary math operations on Rest should return Rest.\n"
		);

		this.assert(
			all.as(Set).postln.size == 1,
			"All binary math operations on Rest should have equal value for equivalent instance values. Default value should be 1\n"
		);

		func = { |op, a, b| a.perform(op, b) == a.value.perform(op, b.value) };
		all = [(0..2), (0..2).collect(Rest(_))].allTuples;
		all = all ++ all.collect(_.reverse);
		all.postln;
		this.assert(
			['<', '>', '<=', '>='].every { |op| all.every { |args| func.(op, *args) } },
			"Instances of Rest should return a boolean for ordering operators"
		);


	}

	test_rest_event {

		this.assert(Rest() == Rest(1), "Rest default value should be 1");

		this.assert(
			(degree: Rest(0), parent: Event.default.parent).use { ~freq.value } == Rest(60.midicps),
			"rest passes through event calculations"
		);

		this.assert(
			(degree: Rest(7), octave: Rest(4), parent: Event.default.parent).delta == 1,
			"delta is not touched by venet calculations"
		);

		this.assert((dur: 1).delta == 1, "delta matches dur");

		this.assert((dur: Rest(1)).delta == 1, "delta matches dur with rest");

		this.assert((dur: Rest(1), degree: Rest(7)).delta == 1, "delta matches dur with rest if other keys are rest, too");


		this.assert((dur: 1, stretch: 2).delta == 2, "delta matches dur when stretched");

		this.assert((dur: Rest(1)).delta == 1, "delta matches dur with rest");

		this.assert((dur: 1, stretch: Rest(2)).delta == 2, "delta matches dur with rest when stretched with number");

		this.assert((dur: Rest(1), stretch: Rest(2)).delta == 2, "delta matches dur with rest when stretched with rest");

		this.assert((dur: 1, stretch: Rest(2)).delta == 2, "delta number matches dur when stretched with rest");

		this.assert((delta: Rest(1)).delta == 1, "delta matches dur with rest");

		this.assert((dur: nil).delta.isNil, "event delta matches dur value when dur is nil");

		this.assert((delta: nil).delta.isNil, "event delta matches delta value when dur is nil");

		this.assert((delta: Rest(1), stretch: 2).delta == 1,  "stretch applies to dur, not delta");

		this.assert(
			try { (delta: Point(0, 0)).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			},
			"using a Point for delta should fail"
		);

		this.assert(
			try { (dur: Point(0, 0)).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			},
			"using a Point for dur should fail"
		);

		this.assert(
			try { (delta: Rest("boo!")).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			},
			"using a Rest with a string for delta should fail"
		);

		this.assert(
			try { (dur: Rest("boo!")).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			},
			"using a Rest with a string for dur should fail"
		);

		this.assert((dur: 1).isRest == false, "event with dur = 1 should return false for isRest");

		this.assert((dur: Rest(1)).isRest == true, "event with dur = Rest(1) should return true for isRest");

		this.assert((type: \rest).isRest == true, "event with dur = \rest should return true for isRest");

		this.assert((degree: \).isRest == true, "event with an empty symbol as dur should return true for isRest");

		// this.assert((degree: Rest).isRest)  // currently NOT working

	}

}

