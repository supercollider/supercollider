TestFunctionProxy : UnitTest {

	test_kwargs_value {
		var p, val;
		p = Maybe.new;
		val = p.value(1, y:7);
		this.assertEquals(val, nil, "empty Maybe should return nil on value");
		p.source = { |x, y| x + y };
		val = p.value(1, y:7);
		this.assertEquals(val, 1 + 7, "function in Maybe should receive both args and keyword args");
	}

	test_kwargs_unaryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.neg;
		val = p.value(1, y:7);
		this.assertEquals(val, neg(1 + 7), "function in UnaryOpFunctionProxy should receive both args and keyword args");
	}

	test_kwargs_binaryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p * p;
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7) * (1 + 7), "function in BinaryOpFunctionProxy should receive both args and keyword args");
	}

	test_kwargs_naryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0, 100, 0, -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 100, 0, -100), "function in NOpFunctionProxy should receive both args and keyword args");
	}

	test_kwargs_naryOp_function_args {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0, { |x, y| 2 * y }, 0, -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 7 * 2, 0, -100), "function arguments in NOpFunctionProxy should receive both args and keyword args");
	}


	test_kwargs_naryOpKWArgs {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0, 100, outMin: 0, outMax: -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 100, 0, -100), "method called with keyword args should keep them in the function proxy");
	}

	test_kwargs_naryOpKWArgs_function_args {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0,inMax: { |x, y| 2 * y }, outMin: 0, outMax: -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 7 * 2, 0, -100), "function arguments in NOpFunctionProxy should receive both args and keyword args, also when the method is called with keyword arguments.");
	}

	test_kwargs_doesNotUnderstand {
		var p, val, event;
		event = (thisIsSpecial: { |self, x, y| x + y });
		p = Maybe(event);
		p = p.thisIsSpecial(1, y:2);
		val = p.value;
		this.assertEquals(val, 1 + 2, "a method which is not understood should be passed on, and keyword args should be passed when called");
	}
	test_kwargs_apply {
		var p, val;
		p = Maybe { |x| if(x == 1) { 1 } { x * p.apply(x: x - 1) } };
		val = p.value(12);
		this.assertEquals(val, 479001600, "apply should pass on keyword argument");
	}

	test_large_combination_of_ops {
		var p, q, r, val, f;
		p = Maybe.new;
		q = Maybe.new;
		f = { |a, b| a.squared * b + a.mod(b) + a.linlin(0, 1, 0, b) };
		r = f.(p, q);
		p.source = 1; q.source = 5; val = r.value;
		this.assertEquals(val, f.(p.value, q.value), "a complicated term of maybes is evaulated like their sources would");
		p.source = -10;  val = r.value;
		this.assertEquals(val, f.(p.value, q.value), "updated source values of maybes return correctly");
		p.source = { |x| x + 1 };  val = r.value(5);
		this.assertEquals(val, f.(p.value(5), q.value), "function source values of maybes take arguments correctly");
		p.source = { |x, y| x + y };  val = r.value(5, y: 20);
		this.assertEquals(val, f.(p.value(5, y: 20), q.value), "function source values of maybes take keyword arguments correctly");
	}
}
