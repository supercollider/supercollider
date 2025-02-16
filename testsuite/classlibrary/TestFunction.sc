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
		var condition = CondVar.new;
		var server = Server(thisMethod.name);
		server.bootSync;
		{ |x| DC.ar(x) }.asBuffer(duration: 0.01, target: server, action: { |b|
			b.get(3, { |val|
				this.assertEquals(val, 0, "unspecified function arguments should pass as 0 when function is written to a buffer");
				b.free;
				condition.signalOne;
			})
		});
		condition.waitFor(2);
		server.quit;
		server.remove;
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


	test_makeFuncModifierString_without_defaultArguments {
		var function = { |a, b, c| [a, b, c]};
		var string = function.def.makeFuncModifierString;
		var should = [1, 2, 3];
		var is = string.interpret.value(1, 2, 3);
		this.assertEquals(is, should, "arguments should be passed correctly");
	}

	test_makeFuncModifierString_with_defaultArguments {
		var function = { |a, b, c=3| [a, b, c]};
		var string = function.def.makeFuncModifierString;
		var should = [1, 2, 3];
		var is = string.interpret.value(1, 2);
		this.assertEquals(is, should, "arguments should be passed correctly");
	}

	test_makeFuncModifierString_with_ellipsisArguments {
		var function = { |a, b ... c| [a, b] ++ c};
		var string = function.def.makeFuncModifierString;
		var should = [1, 2, 3, 4];
		var is = string.interpret.value(1, 2, 3, 4);
		this.assertEquals(is, should, "arguments should be passed correctly");
	}

	test_makeFuncModifierString_with_ellipsisArguments_empty {
		var function = { |a, b ... c| [a, b] ++ c};
		var string = function.def.makeFuncModifierString;
		var should = [1, 2];
		var is = string.interpret.value(1, 2);
		this.assertEquals(is, should, "arguments should be passed correctly");
	}


	test_makeFuncModifierString_with_single_ellipsisArguments {
		var function = { |...a| a };
		var string = function.def.makeFuncModifierString;
		var should = [1, 2];
		var is = string.interpret.value(1, 2);
		this.assertEquals(is, should, "arguments should be passed correctly");
	}




	test_flop_inEnvir {
		var envir = Environment.new;
		var function, flopFunction, result, directResult;
		envir.use {
			~x = [1, 10];
			~y = [2, 20, 200];
		};
		function = { |x, y| x + y };
		flopFunction = function.flop;
		envir.use {
			result = flopFunction.valueEnvir;
		};
		directResult = [ 3, 30, 201 ];
		this.assertEquals(result, directResult, "envirFlop should work in environment")

	}

	test_flop_severalArgs {
		var function = { |a, b| [a, b] }.flop;
		var result = function.([1, 2, 3], [100, 200]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "flop should work with default arguments")
	}

	test_flop_noArgs {
		var function = { 1 }.flop;
		var result = function.value;
		var directResult = [ 1 ];
		this.assertEquals(result, directResult, "flop should work with functions without default arguments")
	}

	test_flop_defaultArg {
		var function = { |a, b = #[100, 200]| [a, b] }.flop;
		var result = function.([1, 2, 3]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "flop should work with default arguments")
	}

	test_flop_defaultArg2 {
		var function = { |a, b ([100, 200])| [a, b] }.flop;
		var result = function.([1, 2, 3], nil);
		var directResult = [ [ 1, [ 100, 200 ] ], [ 2, [ 100, 200 ] ], [ 3, [ 100, 200 ] ] ];
		this.assertEquals(result, directResult, "flop should work with default arguments")
	}

	test_flop_ellipsis {
		var function = { |a, b ... c| [a, b, c] }.flop;
		var result = function.(1, [2, 3], [4, 5], [6, 7]);
		var directResult = [ [ 1, 2, [ 4, 6 ] ], [ 1, 3, [ 5, 7 ] ] ];
		this.assertEquals(result, directResult, "flop should work with ellipsis arguments")
	}

	test_flop_ellipsis_expandFromEllipsis {
		var function = { |a, b ... c| [a, b, c] }.flop;
		var result = function.(1, 2, [4, 5], [6, 7, 8]);
		var directResult = [ [ 1, 2, [ 4, 6 ] ], [ 1, 2, [ 5, 7 ] ], [ 1, 2, [ 4, 8 ] ], ];
		this.assertEquals(result, directResult, "flop should work with ellipsis arguments")
	}

	test_flop_ellipsis_nothingPassed {
		var function = { |a, b ... c| [a, b, c] }.flop;
		var result = function.(1, [2, 3]);
		var directResult = [ [ 1, 2, [ ]], [ 1, 3, [ ]]];
		this.assertEquals(result, directResult, "flop should work with ellipsis when nothing has been passed to it")
	}

	test_flop_ellipsis_noExpansion {
		var function = { |a, b ... c| [a, b, c] }.flop;
		var result = function.(1, 2, 3, 4);
		var directResult = [ [ 1, 2, [3, 4] ]];
		this.assertEquals(result, directResult, "flop should work with ellipsis arguments for the non-expanding case")
	}

	test_flop_partialApplication {
		var function = [_, _].flop;
		var result = function.([1, 2, 3], [100, 200]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "flop should work with partial application")
	}

	test_flop1_partialApplication {
		var function = [_, _].flop1;
		var result = function.([1, 2, 3], [100, 200]);
		var directResult = [ [ 1, 100 ], [ 2, 200 ], [ 3, 100 ] ];
		this.assertEquals(result, directResult, "flop1 should work with partial application")
	}

	test_flop1_unbubble {
		var function = { |x| x }.flop1;
		var result = function.(1);
		var directResult = 1;
		this.assertEquals(result, directResult, "flop1 should not expand for non-array arguments")
	}


	test_inEnvir {
		var envir = Environment.make { ~a = 9 };
		var func = { ~a }.inEnvir(envir);
		var result = func.value;
		this.assertEquals(result, 9, "inEnvir should bind function to environment")
	}

	test_inEnvirWithArgs {
		var envir = Environment.make { ~a = 9 };
		var func = { |b| [~a, b] }.inEnvirWithArgs(envir);
		var result = func.value(b: 4);
		this.assertEquals(result, [9, 4], "inEnvirWithArgs should bind function to environment and take keyword args")
	}

	test_unaryOpFunc_with_kwargs {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f.neg;
		result = g.(x:1, y:2);
		this.assertEquals(result, -3, "unary op functions should pass on the correct keyword arguments");
	}

	test_binaryOpFunc_with_kwargs {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f * 4;
		result = g.(x:1, y:2);
		this.assertEquals(result,  12, "binary op functions should pass on the correct keyword arguments");
	}

	test_naryOpFunc_with_kwargs {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f.linlin(0, 10, 0, -10);
		result = g.(x:1, y:2);
		this.assertEquals(result, -3, "nary op functions should pass on the correct keyword arguments");
	}

	test_naryOpFunc_kwargs_with_kwargs {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f.linlin(0, 10, outMin: 0, outMax: -10);
		result = g.(x:1, y:2);
		this.assertEquals(result, -3, "nary op functions built with keyword args should pass on the correct keyword arguments");
	}

	test_naryOpFunc_kwargs_valueArray {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f.linlin(0, 10, outMin: 0, outMax: -10);
		result = g.valueArray([1,0,2]);
		this.assertEquals(result, -3, "nary op functions built with keyword args should work correctly with valueArray");
	}

	test_naryOpFunc_kwargs_valueArrayEnvir {
		var f, g, result;
		f = { |x, z, y| x + y };
		g = f.linlin(0, 10, outMin: 0, outMax: -10);
		result = Environment.use {
			~y = 2;
			g.valueArrayEnvir([1,0]);
		};
		this.assertEquals(result, -3, "nary op functions built with keyword args should work correctly with valueArrayEnvir");
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
