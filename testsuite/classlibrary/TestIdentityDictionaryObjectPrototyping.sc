TestIdentityDictionaryObjectPrototyping : UnitTest {
	test_known {
		var ev = (\a: 1, \b: 2);
		this.assertEquals(ev.a, 1);
		this.assertEquals(ev.b, 2)
	}
	test_setters {
		var e = ().know_(true);
		e.a = 0.1;
		this.assertEquals(e, (a: 0.1), "Setting through doesNotUnderstand should create an element in the event.");

	}
	test_known_function_call {
		var ev = (\a: 1, \b: 2, \f: {|self, foo, bar|
			[self, foo, bar];
		});
		var result = ev.f(\foo, \bar);
		this.assertEquals(result[0], ev, "'this' should be passed in first");
		this.assertEquals(result[1], \foo, "foo should be second");
		this.assertEquals(result[2], \bar, "bar should be last");
	}
	test_known_function_call_with_args_exact {
		var ev = (\a: 1, \b: 2, \f: {|self, foo, bar| [self, foo, bar] });
		var result = ev.f(foo: \foo, bar: \bar);
		this.assertEquals(result[0], ev, "'this' should be passed in first");
		this.assertEquals(result[1], \foo);
		this.assertEquals(result[2], \bar);
	}
	test_known_function_call_with_args_mixed_exact {
		var ev = (\a: 1, \b: 2, \f: {|self, foo, bar| [self, foo, bar] });
		var result = ev.f(\foo, bar: \bar);
		this.assertEquals(result[0], ev, "'this' should be passed in first");
		this.assertEquals(result[1], \foo);
		this.assertEquals(result[2], \bar);
	}
	test_known_function_call_forward {
		var ev = (
			\a: 1,
			\b: 2,
			\forward: {|self, selector, a1| [self, selector, a1] }
		);
		var result = ev.fasdfasdfasdfasdf(\foo);
		this.assertEquals(result[0], ev, "'this' should be passed in first");
		this.assertEquals(result[1], \fasdfasdfasdfasdf);
		this.assertEquals(result[2], \foo);
	}
	test_known_function_call_forward_with_keywordArgs {
		var ev = (
			\a: 1,
			\b: 2,
			\forward: {|self, selector, a1| [self, selector, a1] }
		);
		var result = ev.asdfasdfasdff(a1: \foo);
		this.assertEquals(result[0], ev, "'this' should be passed in first");
		this.assertEquals(result[1], \asdfasdfasdff);
		this.assertEquals(result[2], \foo);
	}
	test_known_function_call_with_keywordArgs {
		var ev = (
			\a: 1,
			\f: {|self, a, b ...args, kwargs| (\a: a, \b: b, \args: args, \kwargs: kwargs) }
		);
		var result = ev.f(1, 2, 3, 4, 5, foo: 10, bar: 11);
		this.assertEquals(result.a, 1);
		this.assertEquals(result.b, 2);
		this.assertEquals(result.args, [3, 4, 5]);
		this.assertEquals(result.kwargs, [foo: 10, bar: 11]);
	}
	test_with_array {
		var ev = (x: { |self, data| data });
		var result = ev.x([1, 2, 3, 4]);
		this.assertEquals(result, [1, 2, 3, 4], "Arrays should not be expanded by functionPerformList");
	}
}
