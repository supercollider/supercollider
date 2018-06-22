TestInteger : UnitTest {
	test_forby_with_positive_integer_arguments {
		var forby_arguments = List.new;
		10.forBy(20, 3, { | i, j | forby_arguments.add([i, j]); });
		this.assertEquals(forby_arguments, List[[10, 0], [13, 1], [16, 2], [19, 3]],
			"forBy: should start from this and increment by stepval until value exceeds provided endval");
	}

	test_forby_with_negative_integer_arguments {
		var forby_arguments = List.new;
		forBy(3, -7, -2, { | i, j | forby_arguments.add([i, j]); });
		this.assertEquals(forby_arguments, List[[3, 0], [1, 1], [-1, 2], [-3, 3], [-5, 4], [-7, 5]],
			"forBy: should start from this and decrement by stepval until value less than provided endval");
	}

	test_forby_with_positive_float_arguments {
		var forby_arguments = List.new;
		forBy(-6, -4, 0.5, { | i, j | forby_arguments.add([i, j]); });
		this.assertEquals(forby_arguments, List[[-6.0, 0], [-5.5, 1], [-5.0, 2], [-4.5, 3], [-4.0, 4]],
			"forBy: floating point increments should survive integer truncation");
	}

	test_forby_with_negative_float_arguments {
		var forby_arguments = List.new;
		21.forBy(14.5, -3.125, { | i, j | forby_arguments.add([i, j]); });
		this.assertEquals(forby_arguments, List[[21.0, 0], [17.875, 1], [14.75, 2]],
			"forBy: floating point decrements should survive integer truncation");
	}
}