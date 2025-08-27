TestUniqueMethod : UnitTest {
	test_simple {
		var a = 5;
		a.addUniqueMethod(\increment, _ + 1);
		this.assertEquals(a.increment, 6, "Number unique method should work");
	}

	test_args {
		var a = 5;
		a.addUniqueMethod(\myAdd, _ + _);
		this.assertEquals(a.myAdd(2), 7, "Number unique method should work with args");
	}

	test_kwargs {
		var a = 5;
		a.addUniqueMethod(\myAdd, { |self, foo| self + foo });
		this.assertEquals(a.myAdd(foo: 2), 7, "Number unique method should work with keyword args");
	}

	test_complex_args {
		var a = 5;
		a.addUniqueMethod(\myAdd, { |self, foo ...args, kwargs|
			(self: self, foo: foo, args: args, kwargs: kwargs)
		});
		this.assertEquals(
			a.myAdd(2, \a, \b, bar: 10, car: 20),
			(self: 5, foo: 2, args: [\a, \b], kwargs: [\bar, 10, \car, 20]),
			"Number unique method should work with complex args and keyword args"
		);
	}
}
