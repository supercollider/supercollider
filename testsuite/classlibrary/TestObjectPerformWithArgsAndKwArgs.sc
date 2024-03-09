TestObjectPerformWithArgsAndKwArgsTester {
	instanceMethod{|a, b, c, d| ^[a, b, c, d] }
	*classMethod{|a, b, c, d| ^[a, b, c, d] }
}

TestObjectPerformWithArgsAndKwArgs : UnitTest {
	// instances
	test_instance_method_exact_match {
		var obj = TestObjectPerformWithArgsAndKwArgsTester();
		this.assertEquals(
			obj.performWithArgsAndKwArgs(\instanceMethod, [1, 2], (\c: 3, \d: 4)),
			[1, 2, 3, 4],
			"PerformWithArgsAndKwArgs should exactly map to correct args."
		)
	}

	test_instance_method_inexact_match {
		var obj = TestObjectPerformWithArgsAndKwArgsTester();
		this.assertEquals(
			obj.performWithArgsAndKwArgs(\instanceMethod, [1], (\c: 3, \d: 4)),
			[1, nil, 3, 4],
			"PerformWithArgsAndKwArgs should map to correct args, reflecting what was omited from call."
		)
	}

	test_instance_method_bad_args_throw {
		this.assertException(
			{
				TestObjectPerformWithArgsAndKwArgsTester()
				.performWithArgsAndKwArgs(\instanceMethod, [], (\c: 3, \e: 4))
			},
			Error,
			"PerformWithArgsAndKwArgs should throw with incorrect args."
		)
	}
	test_instance_method_args_collide_throw {
		this.assertException(
			{
				TestObjectPerformWithArgsAndKwArgsTester()
				.performWithArgsAndKwArgs(\instanceMethod, [1, 2, 3, 4], (\a: 3))
			},
			Error,
			"PerformWithArgsAndKwArgs should throw when arguments collide."
		)
	}

	// class
	test_class_method_exact_match {
		var obj = TestObjectPerformWithArgsAndKwArgsTester;
		this.assertEquals(
			obj.performWithArgsAndKwArgs(\classMethod, [1, 2], (\c: 3, \d: 4)),
			[1, 2, 3, 4],
			"PerformWithArgsAndKwArgs should exactly map to correct args."
		)
	}

	test_class_method_inexact_match {
		var obj = TestObjectPerformWithArgsAndKwArgsTester;
		this.assertEquals(
			obj.performWithArgsAndKwArgs(\classMethod, [1], (\c: 3, \d: 4)),
			[1, nil, 3, 4],
			"PerformWithArgsAndKwArgs should map to correct args, reflecting what was omited from call."
		)
	}

	test_class_method_bad_args_throw {
		this.assertException(
			{
				TestObjectPerformWithArgsAndKwArgsTester
				.performWithArgsAndKwArgs(\instanceMethod, [], (\c: 3, \e: 4))
			},
			Error,
			"PerformWithArgsAndKwArgs should throw with incorrect args."
		)
	}
	test_class_method_args_collide_throw {
		this.assertException(
			{
				TestObjectPerformWithArgsAndKwArgsTester
				.performWithArgsAndKwArgs(\instanceMethod, [1, 2, 3, 4], (\a: 3))
			},
			Error,
			"PerformWithArgsAndKwArgs should throw when arguments collide."
		)
	}
}