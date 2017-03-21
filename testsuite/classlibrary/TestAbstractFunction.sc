TestAbstractFunction : UnitTest {

	test_rest_binop {

		var args = [[2, Rest(2)], [Rest(2), 2], [Rest(2), Rest(2)]];
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
			(all.as(Set).size == 1)
			&& (all.flat.every(_.isKindOf(Rest))),
			"All binary math operations on Rest should return Rest, and should have equal value for equivalent instance values"
		)
	}

	test_rest_event {

		this.assert(
			(degree: Rest(0), parent: Event.default.parent).use { ~freq.value } == Rest(60.midicps),
			"rest passes through event calculations"
		);

		this.assert((dur: 1).delta == 1, "delta matches dur");

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

