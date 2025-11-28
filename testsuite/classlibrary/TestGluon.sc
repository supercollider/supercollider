TestGluon : UnitTest{
	test_addition {
		var l = Gluon.inbuilt(\gluonTestV1);
		this.assertEquals(1.2 + 5.2, l.addition_test(1.2, 5.2), "Basic addition test");
	}

	test_callback {
		var l = Gluon.inbuilt(\gluonTestV1);

		var c = CondVar();
		var c_test = false;

		l.callback_test(0.2, callback: {
			c_test = true;
			c.signalAll;
		});

		// do something to trigger the garbage collector.
		2000.do { () };

		c.waitFor(1.0) { c_test };

		this.assert(c_test, "Basic callback should work");
	}

	test_callback_with_args {
		var l = Gluon.inbuilt(\gluonTestV1);

		var c = CondVar();
		var c_test = false;
		var a;

		l.callback_with_args_test(0.2, callback: { |...args|
			c_test = true;
			a = args;
			c.signalAll;
		});

		// do something to trigger the garbage collector.
		2000.do { () };

		c.waitFor(1.0) { c_test };

		this.assertEquals(a, [2.1, false], "Callback with arguments should work");
	}


	test_many_callback {
		var l = Gluon.inbuilt(\gluonTestV1);

		var c = CondVar();
		var count = 5;
		var c_count = 0;

		l.many_callback_test(0.1, count, callback: {
			c_count = c_count + 1;
			c.signalAll;
		});

		// do something to trigger the garbage collector.
		2000.do { () };

		c.waitFor(1.0 * count) { c_count == count };
		this.assert(c_count == count, "Many callbacks should work");
	}

	test_param_array {
		var l = Gluon.inbuilt(\gluonTestV1);
		var a = [1, 1.1, 2.0, -3.2];
		this.assertEquals(
			l.array_sum(a),
			a.sum,
			"Parameter arrays should work"
		);
	}

	test_return_array {
		var l = Gluon.inbuilt(\gluonTestV1);
		this.assertEquals(
			l.return_array,
			[0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
			"Returning heap allocated param arrays should work"
		);
	}
}
