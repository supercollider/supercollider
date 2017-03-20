TestAbstractFunction : UnitTest {

	test_rest {

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

}

