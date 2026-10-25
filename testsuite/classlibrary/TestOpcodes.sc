TestOpcodeReciever {
	meow { |...args| ^args }
	reciprocal { |...args| ^args }
}

TestOpcodes : UnitTest {
	test_int_do {
		var c = 0;

		10.do {|n, i| this.assertEquals(n, i) };
		10.do {|n| c = c + 1; };
		this.assertEquals(c, 10);
		c = 0;
		0.do {|n| this.assert(false) };

		this.assertEquals(10.do(nil), 10);
		this.assertEquals(-1.do, -1);
		c = \notCalled;
		-1.do { c = 10 };
		this.assertEquals(c, \notCalled);
	}

	test_int_reverse_do {
		var c = 0;

		10.reverseDo {|n, i| this.assertEquals(10 - i - 1, n) };
		10.reverseDo {|n| c = c + 1; };
		this.assertEquals(c, 10);
		0.reverseDo {|n| this.assert(false) };

		this.assertEquals(10.reverseDo(nil), 10);
		this.assertEquals(-1.reverseDo, -1);
		c = \notCalled;
		-1.reverseDo { c = 10 };
		this.assertEquals(c, \notCalled);
	}

	test_special_msg {
		var f = { |...args| args };
		this.assertEquals([], f.());
		this.assertEquals([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13], f.(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13));
	}

	test_msg {
		var f = TestOpcodeReciever();
		this.assertEquals([], f.meow());
		this.assertEquals([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13], f.meow(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13));
	}

	// A unary operator written as a binary operator is a normal message send.
	test_unary_op_as_binary_op {
		this.assertFloatEquals({ |a, b| a exp: b }.value(0, 7), 1);
		this.assertFloatEquals({ |a, b| a reciprocal: b }.value(4, 0), 0.25);
		this.assertFloatEquals({ |a, b| a midicps: b }.value(69, 1), 440);
		this.assertFloatEquals({ |a, b| a cosh: b; 2 }.value(0, 1), 2);
		this.assertFloatEquals({ |a, b| a cosh: b }.value(0, 1), 1);
		this.assertEquals(TestOpcodeReciever() reciprocal: 5, [5]);
		this.assertEquals(TestOpcodeReciever() reciprocal:.x 5, [5, \x]);
	}

}
