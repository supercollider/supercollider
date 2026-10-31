//# VERSION 3.16
// Interpreted code, as `interpret` and `compile` see it. It is compiled as a function inside
// Interpreter:functionCompileContext, so `this` is the Interpreter, `a` to `z` are its instance variables, and a
// closed function `#{ }` can use those and nothing else from outside. `~x` reads the current environment when the
// code runs. A compile error answers nil and leaves the interpreter usable. The code's version is the one passed with
// it, else a `//# VERSION` on its line 1.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerInterpreted : UnitTest {
	classvar names = #[\a, \b, \f, \x, \y];
	// A declaration after an expression is an error in Legacy, and allowed in 3.16.
	classvar lateVar = "var p = 1; p; var q = 2; q";
	var saved;

	setUp { saved = names.collect { |n| thisProcess.interpreter.perform(n) } }
	tearDown { names.do { |n, i| thisProcess.interpreter.perform(n.asSetter, saved[i]) } }

	run { |source| ^source.interpret }
	inEnvironment { |source| ^Environment.new.use { source.interpret } }

	////////////////////////////////////////////////////////////////////////////////
	// What the code is compiled in
	////////////////////////////////////////////////////////////////////////////////

	test_context_is_the_interpreter {
		this.assert(this.run("this") === thisProcess.interpreter, "this");
		this.assertEquals(this.run("thisMethod.name"), \functionCompileContext);
		this.assertEquals(this.run("thisFunction.def.sourceCode"), "thisFunction.def.sourceCode");
	}

	test_interpreter_variables {
		this.assertEquals(this.run("a = 5; a + 1"), 6);
		this.assertEquals(this.run("a"), 5, "kept between calls");
		this.assertEquals(thisProcess.interpreter.a, 5, "an instance variable of the Interpreter");
		this.assertEquals(this.run("b = { |n| if (n <= 1) { 1 } { n * b.(n - 1) } }; b.(5)"), 120, "recursion through b");
	}

	test_local_hides_interpreter_variable {
		thisProcess.interpreter.a = 5;
		this.assertEquals(this.run("var a = 100; a"), 100);
		this.assertEquals(this.run("a"), 5, "the interpreter's a is unchanged");
	}

	test_interpreter_variables_in_inlined_bodies_and_closures {
		this.assertEquals(this.run("x = 0; 4.do { |i| x = x + i }; x"), 6);
		this.assertEquals(this.run("x = 0; if (true) { var t = 3; x = t }; x"), 3);
		this.assertEquals(this.run("x = 0; f = { x = x + 10 }; f.value; f.value; x"), 20);
		this.assertEquals(this.run("x = 0; y = 0; while { x < 3 } { x = x + 1; y = y + x }; y"), 6);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Environment variables
	////////////////////////////////////////////////////////////////////////////////

	test_environment_variables {
		this.assertEquals(this.inEnvironment("~zz = 3; ~zz + 1"), 4);
		this.assertEquals(this.inEnvironment("~zz"), nil, "a new environment each time");
		this.assertEquals(this.run("(zz: 1).use { ~zz }"), 1);
	}

	test_environment_variables_in_inlined_bodies {
		this.assertEquals(this.inEnvironment("~zz = 0; 3.do { ~zz = ~zz + 1 }; ~zz"), 3);
		this.assertEquals(this.inEnvironment("~zz = 0; if (true) { var t = 2; ~zz = t }; ~zz"), 2);
		this.assertEquals(this.inEnvironment("~zz = 0; while { ~zz < 5 } { ~zz = ~zz + 2 }; ~zz"), 6);
		this.assertEquals(this.inEnvironment("~zz = nil; ~zz ?? { ~zz = 7 }; ~zz"), 7);
	}

	test_environment_variables_in_closures_read_when_called {
		var f = this.inEnvironment("~zz = 1; { ~zz }");
		this.assertEquals(f.value, nil, "the closure reads the environment current when it runs");
		this.assertEquals((zz: 9).use { f.value }, 9);
		f = this.inEnvironment("~zz = 0; { ~zz = ~zz + 10; ~zz }");
		this.assertEquals((zz: 1).use { f.value }, 11);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Closed functions
	////////////////////////////////////////////////////////////////////////////////

	test_closed_function_uses_interpreter_members {
		this.assertEquals(this.run("a = 5; #{ a + 1 }.value"), 6);
		this.assertEquals(this.run("#{ this }.value").class, Interpreter);
		this.assertEquals(this.run("#{ |q| var w = 2; q * w }.value(4)"), 8);
	}

	test_closed_function_name_is_the_interpreter_variable {
		// Inside #{ }, x is the Interpreter's x, not the local that hides it outside.
		thisProcess.interpreter.x = \interpreters;
		this.assertEquals(this.run("var x = 1; #{ x }.value"), \interpreters);
		this.assertEquals(this.run("var x = 1; #{ x = 99 }.value; x"), 1, "the local is untouched");
		this.assertEquals(thisProcess.interpreter.x, 99, "the Interpreter's x was assigned");
	}

	test_closed_function_reads_environment {
		this.assertEquals(this.inEnvironment("~zz = 3; #{ ~zz }.value"), 3);
	}

	test_closed_function_cannot_use_outer_locals {
		this.assertEquals(this.run("var outerLocal = 1; #{ outerLocal }.value"), nil, "a compile error answers nil");
		this.assertEquals(this.run("1 + 1"), 2, "and the interpreter still works");
	}

	test_closed_function_is_closed {
		var f = this.run("#{ |q| q }");
		this.assert(f.def.isClosed, "isClosed");
		this.assertEquals(f.value(3), 3);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Errors
	////////////////////////////////////////////////////////////////////////////////

	test_compile_errors_answer_nil_and_leave_the_interpreter_usable {
		["1 +", "[1, 2", "foo.bar = ", "{ |a| ", "var 1 = 2;", "if (true) {", "#[1, 2"].do { |source|
			this.assertEquals(this.run(source), nil, source);
			this.assertEquals(this.run("1 + 2"), 3, "usable after " ++ source);
		};
		this.assertEquals(thisProcess.interpreter.compile("1 +"), nil, "compile answers nil");
	}

	test_return_is_a_compile_error {
		this.assertEquals(this.run("^7; 8"), nil);
		this.assertEquals(this.run("{ ^5 }.value; 6"), nil);
		this.assertEquals(this.run("if (true) { ^5 }; 6"), nil);
		this.assertEquals(this.run("3"), 3);
	}

	test_run_time_errors_leave_the_interpreter_usable {
		var result = try { this.run("nil.notAMethodAnywhere") } { |e| e.class };
		this.assertEquals(result, DoesNotUnderstandError);
		result = try { this.run("var q = 1; if (q == 1) { Error(\"raised\").throw }; 2") } { |e| e.what };
		this.assertEquals(result, "raised");
		this.assertEquals(this.run("1 + 2"), 3);
	}

	test_empty_code {
		this.assertEquals(this.run(""), nil);
		this.assertEquals(this.run("// only a comment"), nil);
		this.assertEquals(this.run("  \n\t "), nil);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Versions
	////////////////////////////////////////////////////////////////////////////////

	test_no_version_is_legacy {
		this.assertEquals(this.run(lateVar), nil, "a compile error answers nil");
		this.assertEquals(thisProcess.interpreter.compile(lateVar, "/a/file.scd"), nil, "with a file");
	}

	test_version_on_line_1 {
		this.assertEquals(this.run("//# VERSION 3.16\n" ++ lateVar), 2);
		this.assertEquals(this.run("//# VERSION legacy\n" ++ lateVar), nil, "a compile error answers nil");
		this.assertEquals(this.run("//# VERSION legacy\n1 + 2"), 3);
	}

	test_version_argument {
		this.assertEquals(lateVar.interpret(version: \legacy), nil, "a compile error answers nil");
		this.assertEquals(lateVar.interpret(version: \latest), 2);
		this.assertEquals(lateVar.interpret(version: '3.16'), 2, "a Symbol");
		this.assertEquals(lateVar.interpret(version: "3.16"), 2, "a String");
		this.assertEquals(lateVar.compile(version: \latest).value, 2, "String:compile");
		this.assertEquals(thisProcess.interpreter.compile(lateVar, version: \latest).value, 2, "Interpreter:compile");
		this.assertEquals(thisProcess.interpreter.interpret(lateVar, version: \latest), 2, "Interpreter:interpret");
	}

	test_version_argument_and_line_1 {
		this.assertEquals(("//# VERSION 3.16\n" ++ lateVar).interpret(version: '3.16'), 2, "the same version");
		this.assertEquals(("//# VERSION 3.16\n" ++ lateVar).interpret(version: \latest), 2, "latest is 3.16");
		this.assertEquals(("//# VERSION 3.16\n1").interpret(version: \legacy), nil, "a conflict is a compile error");
	}

	test_version_does_not_stick {
		// Were legacy kept, it would conflict with line 1.
		"1".interpret(version: \legacy);
		this.assertEquals(this.run("//# VERSION 3.16\n" ++ lateVar), 2);
	}

	test_bad_version_answers_nil {
		// Posts an error, as a compile error does.
		this.assertEquals("1".interpret(version: \meow), nil, "not a release");
		this.assertEquals("1".interpret(version: '9.99'), nil, "after the latest release");
		this.assertEquals("1".interpret(version: 3.16), nil, "a Float");
	}

	test_interpreted_code_as_316 {
		// Hoisting is 3.16: a variable in an if body is fresh each pass, and nil without a value.
		this.assertEquals("var l = []; 3.do { |i| var v; l = l.add(v); v = i }; l".interpret(version: \latest),
			[nil, nil, nil]);
		// A 3.16 generator qualifier scope.
		this.assertEquals("all {: { x }, x <- (1..3) }.collect(_.value)".interpret(version: \latest), [1, 2, 3]);
	}

	test_compile_answers_a_function_to_call_later {
		var f = thisProcess.interpreter.compile("var q = 2; q * 3");
		this.assertEquals([f.value, f.value], [6, 6]);
	}
}
