TestPerformWithMethod {
	meth {|a, b, c, d ...e|
		var i = 0;
		var j = 1;

		^[a, b, c, d, e]
	}
}

TestPerformWith : UnitTest {
	test_method_args{
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [\a, \b, \c, \d]),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_keyword_args{
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [], (a: \a, b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_args_and_kw {
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [\a], (b: \b, c: \c, d: \d)),
			[\a, \b, \c, \d, []]
		);
	}

	test_method_var_args_expand {
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [\a], (b: \b, c: \c, d: \d, e: [\e, \f])),
			[\a, \b, \c, \d, [\e, \f]]
		);
	}

	test_method_many_normal_args {
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [\a, \b, \c, \d, \e, \f, \g]),
			[\a, \b, \c, \d, [\e, \f, \g]]
		);
	}

	test_method_varArgKeyword {
		this.assertEquals(
			TestPerformWithMethod()
			.performWith(\meth, [], (e: [\a, \b, \c, \d])),
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
			f.performWith(\value, [\a, \b, \c, \d]),
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
			f.performWith(\value, [], (a: \a, b: \b, c: \c, d: \d)),
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
			f.performWith(\value, [\a], (b: \b, c: \c, d: \d)),
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
			f.performWith(\value, [\a], (b: \b, c: \c, d: \d, e: [\e, \f])),
			[\a, \b, \c, \d, [\e, \f]]
		);
	}


}