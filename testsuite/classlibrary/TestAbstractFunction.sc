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
			boolean:(degree: Rest(0), parent: Event.default.parent).use { ~freq.value } == Rest(60.midicps)
		);

		this.assert((dur: 1).delta == 1);

		this.assert((dur: 1, stretch: 2).delta == 2);

		this.assert((dur: Rest(1)).delta == 1);

		this.assert((dur: 1, stretch: Rest(2)).delta == 2);

		this.assert((dur: Rest(1), stretch: Rest(2)).delta == 2);

		this.assert((delta: Rest(1)).delta == 1);

		this.assert((dur: nil).delta.isNil);

		this.assert((delta: nil).delta.isNil);

		this.assert((delta: Rest(1), stretch: 2).delta == 1,   "stretch applies to dur, not delta");

		this.assert(
			try { (delta: Point(0, 0)).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			}
		);

		this.assert(
			try { (dur: Point(0, 0)).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			}
		);

		this.assert(
			try { (delta: Rest("boo!")).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			}
		);

		this.assert(
			try { (dur: Rest("boo!")).delta } { |err|
				err.isKindOf(PrimitiveFailedError) and: { err.what == "Wrong type." }
			}
		);

		this.assert((dur: 1).isRest == false);

		this.assert((dur: Rest(1)).isRest == true);

		this.assert((type: \rest).isRest == true);

		this.assert((degree: \).isRest == true);

		// this.assert((degree: Rest).isRest)  // currently NOT working

	}

}

