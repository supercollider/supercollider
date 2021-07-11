TestFunction : UnitTest {

	test_function_scope {
		var generator, list, result, a;
		generator = {
			var x = 0;
			{ |y| a = x = x + y }
		};
		list = 10.collect(generator);
		result = list.collect { |each, i| each.value(i) };
		this.assert(result == (0..9), "lexical function scope should be independent");
		this.assert(a == 9, "outer lexical function scope should recieve side-effect");
	}

	test_classmethod_scope {
		var obj, func;
		obj = CommonTestClass;
		obj.x = 5;
		func = obj.returnLexicalScopeGetterFunc;
		this.assert(func.value == [5, 7], "function scope should be able to access class variables");
		func = obj.returnLexicalScopeSetterFunc;
		func.value(42);
		this.assert(obj.x == 42, "function should be able to set class variable");
		this.assert(obj.y == 42, "function should be able to set class variable that has no setter");
	}

	test_instancemethod_scope {
		var obj, func;
		obj = CommonTestClass.new;
		obj.a = 5;
		func = obj.returnLexicalScopeGetterFunc;
		this.assert(func.value == [5, 3], "function scope should be able to access instance variables");
		func = obj.returnLexicalScopeSetterFunc;
		func.value(42);
		this.assert(obj.a == 42, "function should be able to set instance variable");
		this.assert(obj.b == 42, "function should be able to set instance variable that has no setter");
	}

	test_plot {
		{ |x| DC.ar(x) }.asBuffer(duration: 0.01, action: { |b|
			b.get(3, { |val|
				this.assertEquals(val, 0, "unspecified function arguments should pass as 0 when function is written to a buffer");
				b.free;
			})
		})
	}

	test_argumentString_without_defaultArguments {
		var function = { |a, b, c| };
		var arguments = [[true, true], [true, false], [false, true], [false, false]];
		var strings = arguments.collect { |pair|
			function.def.argumentString(withDefaultValues: pair[0], withEllipsis: pair[1])
		};
		var results = [
			"a, b, c",
			"a, b, c",
			"a, b, c",
			"a, b, c"
		];
		strings.do { |x, i|
			this.assertEquals(x, results[i], "argument string should match")
		}
	}
	test_argumentString_with_defaultArguments {
		var function = { |a = 1, b = 2, c| };
		var arguments = [[true, true], [true, false], [false, true], [false, false]];
		var strings = arguments.collect { |pair|
			function.def.argumentString(withDefaultValues: pair[0], withEllipsis: pair[1])
		};
		var results = [
			"a = 1, b = 2, c",
			"a = 1, b = 2, c",
			"a, b, c",
			"a, b, c"
		];
		strings.do { |x, i|
			this.assertEquals(x, results[i], "argument string should match")
		}
	}

	test_argumentString_with_ellipsisArguments {
		var function = { |a, b ... c| };
		var arguments = [[true, true], [true, false], [false, true], [false, false]];
		var strings = arguments.collect { |pair|
			function.def.argumentString(withDefaultValues: pair[0], withEllipsis: pair[1])
		};
		var results = [
			"a, b ... c",
			"a, b, c",
			"a, b ... c",
			"a, b, c"
		];
		strings.do { |x, i|
			this.assertEquals(x, results[i], "argument string should match")
		}
	}

	test_argumentString_with_single_ellipsisArguments {
		var function = { | ... a| };
		var arguments = [[true, true], [true, false], [false, true], [false, false]];
		var strings = arguments.collect { |pair|
			function.def.argumentString(withDefaultValues: pair[0], withEllipsis: pair[1])
		};
		var results = [
			" ... a",
			"a",
			" ... a",
			"a"
		];
		strings.do { |x, i|
			x.postcs;
			this.assertEquals(x, results[i], "argument string should match")
		}
	}


	test_argumentString_with_ellipsis_andDefaultArguments {
		var function = { |a = 1, b ... c| };
		var arguments = [[true, true], [true, false], [false, true], [false, false]];
		var strings = arguments.collect { |pair|
			function.def.argumentString(withDefaultValues: pair[0], withEllipsis: pair[1])
		};
		var results = [
			"a = 1, b ... c",
			"a = 1, b, c",
			"a, b ... c",
			"a, b, c"
		];
		strings.do { |x, i|
			this.assertEquals(x, results[i], "argument string should match")
		}
	}

	test_envirFlop {
		var envir = Environment.new;
		var function, flopFunction, result, directResult;
		envir.use {
			~x = [1, 2];
			~y = [10, 100, 1000];
		};
		function = { |x, y ... z|
			if(x > 1) { x * y } { 0 }
		};
		flopFunction = function.envirFlop;
		envir.use {
			result = flopFunction.valueEnvir;
		};
		directResult = [ 0, 200, 0 ];
		this.assertEquals(result, directResult, "envirFlop should work in environment")

	}

	test_makeFlopFunc_severalArgs {
		var function = { |a, b| [a, b] }.makeFlopFunc;
		var result = function.([1, 2, 3], [100, 200]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "makeFlopFunc should work with default arguments")
	}


	test_makeFlopFunc_defaultArg {
		var function = { |a, b = #[100, 200]| [a, b] }.makeFlopFunc;
		var result = function.([1, 2, 3]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "makeFlopFunc should work with default arguments")
	}

	test_makeFlopFunc_ellipsis {
		var function = { |a, b ... c| [a, b, c] }.makeFlopFunc;
		var result = function.(1, [2, 3], [4, 5], [6, 7]);
		var directResult = [ [ 1, 2, [ 4, 6 ] ], [ 1, 3, [ 5, 7 ] ] ];
		this.assertEquals(result, directResult, "makeFlopFunc should work with ellipsis arguments")
	}


}

CommonTestClass {

	var <>a, <b = 3;
	classvar <>x, <y = 7;

	*returnLexicalScopeGetterFunc {
		^{ [x, y] }
	}

	*returnLexicalScopeSetterFunc {
		^{ |val|
			x = val;
			y = val;
		}
	}

	returnLexicalScopeGetterFunc {
		^{ [a, b] }
	}

	returnLexicalScopeSetterFunc {
		^{ |val|
			a = val;
			b = val;
		}
	}

}


