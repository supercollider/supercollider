TestPerformWithMethod {
	meth {|a, b, c, d ...e|
		var i = 0;
		var j = 1;

		^[a, b, c, d, e]
	}
}

Test_ValueWithKeys {
	value {|one, two, three ...rest| ^["called value"] ++ [one, two, three] ++ rest }
}

TestValueWithKeys : UnitTest {
	test_value_with_keys {
		this.assertEquals(
			Test_ValueWithKeys().(1, 2, 3, 4, 5),
			Test_ValueWithKeys().(1, 2, three: 3, rest: [4, 5]),
			"keywords should work on value"
		);
		this.assertEquals(
			Test_ValueWithKeys().value(1, 2, 3, 4, 5),
			Test_ValueWithKeys().valueWithKeys([1, 2], [\three: 3, \rest: [4, 5]]),
			"keywords should work with explicit call to valueWithKeys"
		);
	}
}

TestPerformWithKeys : UnitTest {
	test_method_args{
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [\a, \b, \c, \d]),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_keyword_args{
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [], (a: \a, b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_args_and_kw {
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [\a], (b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_var_args_expand {
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [\a], (b: \b, c: \c, d: \d, e: [\e, \f])),
			[\a, \b, \c, \d, [\e, \f]]
		);
	}

	test_method_many_normal_args {
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [\a, \b, \c, \d, \e, \f, \g]),
			[\a, \b, \c, \d, [\e, \f, \g]]
		);
	}

	test_method_varArgKeyword {
		this.assertEquals(
			TestPerformWithMethod().performWithKeys(\meth, [], (e: [\a, \b, \c, \d])),
			[nil, nil, nil, nil, [\a, \b, \c, \d]]
		);
	}


	test_func_args{
		var f = {|a, b, c, d ...e|
			var i = 0;
			var j = 1;
			[a, b, c, d, e]
		};
		this.assertEquals(
			f.performWithKeys(\value, [\a, \b, \c, \d]),
			[\a, \b, \c, \d, []]
		);
	}

	test_func_keyword_args{
		var f = {|a, b, c, d ...e|
			var i = 0;
			var j = 1;
			[a, b, c, d, e]
		};
		this.assertEquals(
			f.performWithKeys(\value, [], (a: \a, b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_func_args_and_kw {
		var f = {|a, b, c, d ...e|
			var i = 0;
			var j = 1;
			[a, b, c, d, e]
		};
		this.assertEquals(
			f.performWithKeys(\value, [\a], (b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_func_var_args_expand {
		var f = {|a, b, c, d ...e|
			var i = 0;
			var j = 1;
			[a, b, c, d, e]
		};
		this.assertEquals(
			f.performWithKeys(\value, [\a], (b: \b, c: \c, d: \d, e: [\e, \f])),
			[\a, \b, \c, \d, [\e, \f]]
		);
	}
}

// functionPerformWithKeys is tested over in TestIdentityDictionaryObjectPrototyping
