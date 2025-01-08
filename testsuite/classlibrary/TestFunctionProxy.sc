TestFunctionProxy : UnitTest {

	test_kwargs_value {
		var p, val;
		p = Maybe.new;
		val = p.value(1, y:7);
		this.assertEquals(val, nil, "empty Maybe should return nil on value");
		p.source = { |x, y| x + y };
		val = p.value(1, y:7);
		this.assertEquals(val, 1 + 7, "function in Maybe should receive both args and keword args");
	}

	test_kwargs_unaryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.neg;
		val = p.value(1, y:7);
		this.assertEquals(val, neg(1 + 7), "function in UnaryOpFunctionProxy should receive both args and keword args");
	}

	test_kwargs_binaryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p * p;
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7) * (1 + 7), "function in BinaryOpFunctionProxy should receive both args and keword args");
	}

	test_kwargs_naryOp {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0, 100, 0, -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 100, 0, -100), "function in NOpFunctionProxy should receive both args and keword args");
	}

	test_kwargs_naryOp_function_args {
		var p, val;
		p = Maybe { |x, y| x + y };
		p = p.linlin(0, { |x, y| 2 * y }, 0, -100);
		val = p.value(1, y:7);
		this.assertEquals(val, (1 + 7).linlin(0, 7 * 2, 0, -100), "function arguments in NOpFunctionProxy should receive both args and keword args");
	}


}
