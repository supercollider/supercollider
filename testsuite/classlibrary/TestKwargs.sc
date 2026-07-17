TestKwargs_Obj {
	foo { |a, b...args, kwargs|
		var x = 100;
		var y = 101;
		^(a: a, b: b, args: args, kwargs: kwargs)
	}
	doesNotUnderstand { |selector ...args, kwargs|
		^(selector: selector, args: args, kwargs: kwargs)
	}
}

TestKwargs : UnitTest {
	test_basic_arg_passing {
		this.assertEquals(
			TestKwargs_Obj().foo(1),
			(a: 1, args: [], kwargs: [])
		);

		this.assertEquals(
			TestKwargs_Obj().foo(1, 2, 3, 4),
			(a: 1, b: 2, args: [3, 4], kwargs: [])
		);

		this.assertEquals(
			TestKwargs_Obj().foo(a: 1, b: 2),
			(a: 1, b: 2, args: [], kwargs: [])
		);

		this.assertEquals(
			TestKwargs_Obj().foo(1, 2, 3, e: 1, f: 2),
			(a: 1, b: 2, args: [3], kwargs: [\e, 1, \f, 2])
		);
	}

	test_collision_with_args {
		this.assertEquals(
			TestKwargs_Obj().foo(args: 2),
			(args: [], kwargs: [args: 2]),
			"Should be able to pass 'args' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().b(args: 2),
			(selector: \b, args: [], kwargs: [args: 2]),
			"Should be able to pass 'args' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().foo(1, b: 2, args: 2),
			(a: 1, b: 2, args: [], kwargs: [args: 2]),
			"Should be able to pass 'args' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().foo(1, b: 2, args: 2, kwargs: 20),
			(a: 1, b: 2, args: [], kwargs: [args: 2, kwargs: 20]),
			"Should be able to pass 'args' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().foo(args: 2, kwargs: 20),
			(args: [], kwargs: [args: 2, kwargs: 20]),
			"Should be able to pass 'args' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().foo(1, 2, 3, args: 2, kwargs: 20),
			(a: 1, b: 2, args: [3], kwargs: [args: 2, kwargs: 20]),
			"Should be able to pass 'args' along to the method"
		);
	}

	test_collision_with_args_block {
		var f = { |a, b ...args, kwargs|
			(a: a, b: b, args: args, kwargs: kwargs);
		};
		this.assertEquals(
			f.(1, b: 2, args: 10, kwargs: 20),
			(a: 1, b: 2, args: [], kwargs: [\args, 10, \kwargs: 20]),
			"Should be able to pass 'args' and 'kwargs' to methods"
		);
		this.assertEquals(
			f.(1, b: 2),
			(a: 1, b: 2, args: [], kwargs: []),
			"Should be able to pass 'args' and 'kwargs' to methods"
		)
	}

	test_collision_with_kwargs {
		this.assertEquals(
			TestKwargs_Obj().foo(kwargs: 2),
			(args: [], kwargs: [kwargs: 2]),
			"Should be able to pass 'kwargs' along to the method"
		);
		this.assertEquals(
			TestKwargs_Obj().b(kwargs: 2),
			(selector: \b, args: [], kwargs: [kwargs: 2]),
			"Should be able to pass 'kwargs' along to the method"
		);
	}

	test_doesNotUnderstand {
		this.assertEquals(
			TestKwargs_Obj().bar(1,2, a: 3, b: 4),
			(selector: \bar, args: [1, 2], kwargs: [a: 3, b: 4])
		)
	}

	test_function_perform_list {
		var f = {|a, b, c... args, kwargs| (a: a, b: b, c: c, args: args, kwargs: kwargs) };
		this.assertEquals(
			f.(1, *[2, 3, 4], foo: 10),
			(a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
		);
		this.assertEquals(
			f.performList(\value, 1, [2, 3, 4], foo: 10),
			(a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
		);
		this.assertEquals(
			f.functionPerformList(\value, 1, [2, 3, 4], foo: 10),
			(a: 1, b: 2, c: 3, args: [4], kwargs: [\foo, 10])
		);
	}

	test_wrappers {
		var inlined = { |func|
			{ |...args, kwargs|
				func.performArgs(\value, args, kwargs)
			}
		};
		var asVar = { |func|
			{ |...args, kwargs|
				var val = func.performArgs(\value, args, kwargs);
				val
			}
		};

		this.assertEquals(
			inlined.({ |a, b=100, c| [a, b, c] }).([1, 2]),
			[[1, 2], 100, nil]
		);
		this.assertEquals(
			asVar.({ |a, b=100, c| [a, b, c] }).([1, 2]),
			[[1, 2], 100, nil]
		);

		this.assertEquals(
			inlined.({ |a, b=100, c| [a, b, c] }).([1, 2], 42),
			[[1, 2], 42, nil]
		);
		this.assertEquals(
			asVar.({ |a, b=100, c| [a, b, c] }).([1, 2], 42),
			[[1, 2], 42, nil]
		);

		this.assertEquals(
			inlined.({ |a, b=100, c| [a, b, c] }).([1, 2], 42, c: 23),
			[[1, 2], 42, 23]
		);
		this.assertEquals(
			asVar.({ |a, b=100, c| [a, b, c] }).([1, 2], 42, c: 23),
			[[1, 2], 42, 23]
		);
	}

	test_control_flow {
		var i = 0;
		// These all used to crash.
		this.assertEquals(true.if(trueFunc: {'s'}), 's', "If with truefunc");
		this.assertEquals(true.if(falseFunc: {'s'}), nil, "If with falseFunc");

		this.assertEquals(false.if(trueFunc: {'s'}), nil, "If with trueFunc, but false");
		this.assertEquals(false.if(falseFunc: {'s'}), 's', "If with falseFunc, but false");

		// Posts a warning, but that is fine.
		this.assertEquals(100.loop(xyz: 1).(), 100, "loop");

		this.assertEquals(true.and(that: {1}), 1, "and true");
		this.assertEquals(false.and(that: {1}), false, "and false");

		// Unary messages
		this.assertEquals(true.not(asd: 1), false, "not true");
		this.assertEquals(false.not(asd: 1), true, "not false");

		// Binary, note the unusual syntax needed to call these with keywords!
		this.assertEquals((!?)(1, obj: 2),  2, "binary !? with kwargs");

		// This one isn't inlined, and so actually does the kwargs lookup in doPrimitiveWithKeys
		this.assertEquals((+)(1, aNumber: 10), 11, "binary + with kwargs");

		while({ i < 10 }, body: { i = i + 1 });
		this.assertEquals(i, 10, "while with kwargs works");
	}

	test_block {

		var f = {
			| a=1, b=2 ...args, kwargs|
			var bar = 10;
			this.assertEquals(a, 1);
			this.assertEquals(b, 2);
			this.assertEquals(bar, 10);
			this.assertEquals(args, []);
			this.assertEquals(kwargs, []);
		};
		f.();
	}
}
