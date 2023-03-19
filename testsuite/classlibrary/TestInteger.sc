TestInteger : UnitTest {
	test_forBy_positiveIntegerArguments {
		var forByArguments = List.new;
		10.forBy(20, 3, { | i, j | forByArguments.add([i, j]); });
		this.assertEquals(forByArguments, List[[10, 0], [13, 1], [16, 2], [19, 3]]);
	}

	test_forBy_negativeIntegerArguments {
		var forByArguments = List.new;
		forBy(3, -7, -2, { | i, j | forByArguments.add([i, j]); });
		this.assertEquals(forByArguments, List[[3, 0], [1, 1], [-1, 2], [-3, 3], [-5, 4], [-7, 5]]);
	}

	test_forBy_positiveFloatArguments {
		var forByArguments = List.new;
		forBy(-6, -4, 0.5, { | i, j | forByArguments.add([i, j]); });
		this.assertEquals(forByArguments, List[[-6.0, 0], [-5.5, 1], [-5.0, 2], [-4.5, 3], [-4.0, 4]]);
	}

	test_forBy_negativeFloatArguments {
		var forByArguments = List.new;
		21.forBy(14.5, -3.125, { | i, j | forByArguments.add([i, j]); });
		this.assertEquals(forByArguments, List[[21.0, 0], [17.875, 1], [14.75, 2]]);
	}

	test_forBy_nonIntegerOrFloatArguments_exception {
		this.assertException({10.forBy("yellow", 1, { | i, j | i.postln; })}, PrimitiveFailedError);
		this.assertException({1.forBy(10, [ 5 ], { | i, j | j.postln; })}, PrimitiveFailedError);
	}

	test_forBy_zeroStepValue_exception {
		this.assertException({-1.forBy(20, 0, { | i, j | i.postln; })}, PrimitiveFailedError);
		this.assertException({100.forBy(200, 0.0, { | i, j | j.postln; })}, PrimitiveFailedError);
	}
}
