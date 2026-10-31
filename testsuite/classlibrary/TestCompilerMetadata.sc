//# VERSION 3.16
// What the compiler stores next to the code: argument and variable names, the prototype frame (default values),
// argument counts, and the source text. Code in the class library reads these at run time (valueEnvir, keyword
// arguments, SynthDef controls), so a wrong entry is a wrong program even when the bytecode is right.
//
// Variables of an inlined body are hoisted into the frame of the function around it: they appear in its varNames
// after its own variables, with nil in the prototype frame, as the body sets them on each pass. A variable a closure
// captures is not hoisted. Reusing the slots of sibling bodies (compiler-tests finding 2) would change these.
//
// Non-ASCII text before methods checks that source positions count bytes: é, ü, →, 音.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerMetadata : UnitTest {

	////////////////////////////////////////////////////////////////////////////////
	// Methods under test
	////////////////////////////////////////////////////////////////////////////////

	plain { |a, b = 2, c = #[1, 2]| var x = 3, y; ^a }
	rest { |a ... more| ^more }
	keywords { |a, b = 4 ... rest, kwargs| ^[a, b, rest, kwargs] }
	noArguments { ^1 }
	// é → 音: a comment with multi-byte characters between two methods.
	literalDefaults { |a = 1.5, b = \sym, c = "str", d = $c, e = true, f = nil, g = -3, h = inf, i = 16rFF, j = #[1, [2]]| ^a }
	expressionDefault { |a = (1 + 2), b = 4| ^[a, b] }
	hoisted { |x| var a = 1; if (x) { var b = 2, c; c = b }; while { false } { var d = 4 }; ^a }
	hoistedCaptured { |x| var a = 1; if (x) { var b = 2; ^{ b } }; ^a }
	nested { ^{ |p = 5, q| var r = 6; { |s = 7| var t; [p, q, r, s, t] } } }
	multiLine { |a|
		var s = "a string with a brace } and a quote \" in it";
		^s
	}

	////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////

	method { |name| ^this.class.findMethod(name) }

	test_method_argument_and_variable_names {
		this.assertEquals(this.method(\plain).argNames, SymbolArray[\this, \a, \b, \c]);
		this.assertEquals(this.method(\plain).varNames, SymbolArray[\x, \y]);
		this.assertEquals(this.method(\rest).argNames, SymbolArray[\this, \a, \more]);
		this.assertEquals(this.method(\rest).varNames, nil);
		this.assertEquals(this.method(\keywords).argNames, SymbolArray[\this, \a, \b, \rest, \kwargs]);
		this.assertEquals(this.method(\noArguments).argNames, SymbolArray[\this]);
	}

	test_method_counts {
		this.assertEquals([this.method(\plain).numArgs, this.method(\plain).numVars], [4, 2]);
		this.assertEquals([this.method(\rest).numArgs, this.method(\rest).varArgs], [2, true]);
		this.assertEquals(this.method(\keywords).numArgs, 3);
		this.assertEquals([this.method(\keywords).hasVarArgs, this.method(\keywords).hasKwArgs], [true, true]);
		this.assertEquals([this.method(\rest).hasVarArgs, this.method(\rest).hasKwArgs], [true, false]);
		this.assertEquals([this.method(\plain).hasVarArgs, this.method(\plain).hasKwArgs], [false, false]);
	}

	test_method_prototype_frames {
		this.assertEquals(this.method(\plain).prototypeFrame, [nil, nil, 2, [1, 2], 3, nil]);
		this.assertEquals(this.method(\rest).prototypeFrame, [nil, nil, []]);
		this.assertEquals(this.method(\keywords).prototypeFrame, [nil, nil, 4, [], []]);
		this.assertEquals(this.method(\literalDefaults).prototypeFrame,
			[nil, 1.5, \sym, "str", $c, true, nil, -3, inf, 255, [1, [2]]]);
	}

	test_literal_defaults_arrive_as_arguments {
		this.assertEquals(this.literalDefaults, 1.5);
		this.assertEquals(this.plain(1), 1);
	}

	test_expression_default_is_computed_at_the_call {
		// The prototype holds nil, the method computes the value when the argument is nil.
		this.assertEquals(this.method(\expressionDefault).prototypeFrame, [nil, nil, 4]);
		this.assertEquals(this.expressionDefault, [3, 4]);
		this.assertEquals(this.expressionDefault(7), [7, 4]);
		this.assertEquals(this.expressionDefault(b: 5), [3, 5]);
	}

	test_hoisted_variables_in_var_names {
		this.assertEquals(this.method(\hoisted).varNames, SymbolArray[\a, \b, \c, \d]);
		this.assertEquals(this.method(\hoisted).prototypeFrame, [nil, nil, 1, nil, nil, nil]);
		this.assertEquals(this.method(\hoisted).numVars, 4);
		this.assertEquals(this.hoisted(true), 1);
	}

	test_captured_variable_is_not_hoisted {
		var inner;
		this.assertEquals(this.method(\hoistedCaptured).varNames, SymbolArray[\a]);
		// The if body keeps a frame of its own, as a function, and the variable lives there.
		inner = this.hoistedCaptured(true);
		this.assertEquals(inner.def.varNames, nil);
		this.assertEquals(inner.def.context.varNames, SymbolArray[\b]);
		this.assertEquals(inner.def.context.prototypeFrame, [2]);
		this.assertEquals(inner.def.context.sourceCode, "{ var b = 2; ^{ b } }");
		this.assert(inner.def.context.context === this.method(\hoistedCaptured), "the body's context is the method");
		this.assertEquals(inner.value, 2);
	}

	test_nested_function_metadata {
		var outer = this.nested, inner = outer.value(50, 60);
		this.assertEquals([outer.def.argNames, outer.def.varNames], [SymbolArray[\p, \q], SymbolArray[\r]]);
		this.assertEquals(outer.def.prototypeFrame, [5, nil, 6]);
		this.assertEquals([inner.def.argNames, inner.def.varNames], [SymbolArray[\s], SymbolArray[\t]]);
		this.assertEquals(inner.def.prototypeFrame, [7, nil]);
		this.assertEquals(inner.value, [50, 60, 6, 7, nil]);
		this.assertEquals(outer.def.isClosed, true);
		this.assertEquals(inner.def.isClosed, false);
		this.assert(inner.def.context === outer.def, "the inner function's context is the outer function");
	}

	test_method_source_code {
		this.assertEquals(this.method(\plain).sourceCode, "plain { |a, b = 2, c = #[1, 2]| var x = 3, y; ^a }");
		this.assertEquals(this.method(\literalDefaults).sourceCode, "literalDefaults { |a = 1.5, b = \\sym, c = \"str\", "
			"d = $c, e = true, f = nil, g = -3, h = inf, i = 16rFF, j = #[1, [2]]| ^a }");
		this.assertEquals(this.method(\multiLine).sourceCode,
			"multiLine { |a|\n\t\tvar s = \"a string with a brace } and a quote \\\" in it\";\n\t\t^s\n\t}");
		this.assertEquals(this.method(\nested).sourceCode, "nested { ^{ |p = 5, q| var r = 6; { |s = 7| var t; [p, q, r, s, t] } } }");
	}

	test_nested_function_source_code {
		var outer = this.nested;
		this.assertEquals(outer.def.sourceCode, "{ |p = 5, q| var r = 6; { |s = 7| var t; [p, q, r, s, t] } }");
		this.assertEquals(outer.value.def.sourceCode, "{ |s = 7| var t; [p, q, r, s, t] }");
	}

	test_method_char_pos_is_a_byte_offset_into_the_file {
		var method = this.method(\literalDefaults), text = File.readAllString(method.filenameSymbol.asString);
		this.assertEquals(text[method.charPos .. method.charPos + 14], "literalDefaults");
		this.assertEquals(method.ownerClass, this.class);
	}

	test_extension_method_metadata {
		var method = Object.findMethod(\testCompilerMetadataExtension);
		this.assertEquals(method.argNames, SymbolArray[\this, \k]);
		this.assertEquals(method.prototypeFrame, [nil, 9]);
		this.assertEquals(method.sourceCode, "testCompilerMetadataExtension { |k = 9| ^k * 2 }");
		this.assertEquals(method.ownerClass, Object);
		this.assertEquals(1.testCompilerMetadataExtension, 18);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Functions and interpreted code
	////////////////////////////////////////////////////////////////////////////////

	test_function_metadata {
		var f = { |a = 1, b| var c = 3; c };
		this.assertEquals([f.def.argNames, f.def.varNames], [SymbolArray[\a, \b], SymbolArray[\c]]);
		this.assertEquals(f.def.prototypeFrame, [1, nil, 3]);
		this.assertEquals([f.numArgs, f.numVars, f.varArgs], [2, 1, false]);
		this.assertEquals(f.def.sourceCode, "{ |a = 1, b| var c = 3; c }");
	}

	test_function_with_hoisted_variables {
		var f = { |a| var c = 3; if (a) { var d = 4; c = c + d }; c };
		this.assertEquals(f.def.varNames, SymbolArray[\c, \d]);
		this.assertEquals(f.def.prototypeFrame, [nil, 3, nil]);
		this.assertEquals([f.value(true), f.value(false)], [7, 3]);
	}

	test_interpreted_function_metadata {
		// d is hoisted into the function, as in 3.16.
		var f = "{ |a = 1, b| var c = 3; if (a.notNil) { var d = 4; d }; c }".interpret(version: '3.16');
		this.assertEquals([f.def.argNames, f.def.varNames], [SymbolArray[\a, \b], SymbolArray[\c, \d]]);
		this.assertEquals(f.def.prototypeFrame, [1, nil, 3, nil]);
		this.assertEquals(f.def.sourceCode, "{ |a = 1, b| var c = 3; if (a.notNil) { var d = 4; d }; c }");
	}

	test_interpreted_code_metadata {
		var f = thisProcess.interpreter.compile("var q = 1; q + 1");
		this.assertEquals([f.def.argNames, f.def.varNames, f.def.prototypeFrame], [nil, SymbolArray[\q], [1]]);
		this.assertEquals(f.def.sourceCode, "var q = 1; q + 1");
		this.assertEquals(f.value, 2);
	}

	test_interpreted_code_with_arguments {
		var f = thisProcess.interpreter.compile("|x = 5, y| var z = 1; x + z");
		this.assertEquals([f.def.argNames, f.def.prototypeFrame], [SymbolArray[\x, \y], [5, nil, 1]]);
		this.assertEquals([f.value, f.value(10)], [6, 11]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// What reads the metadata
	////////////////////////////////////////////////////////////////////////////////

	test_value_envir {
		var f = { |a = 1, b = 2| a + b };
		this.assertEquals(f.valueEnvir, 3);
		this.assertEquals((b: 10).use { f.valueEnvir }, 11);
		this.assertEquals(f.valueWithEnvir((b: 20)), 21);
	}

	test_value_envir_with_hoisted_variables {
		// Hoisted variables are not arguments, so the environment cannot set them.
		var f = { |a = 1| var v = 2; if (a > 0) { var h = 3; v = v + h }; a + v };
		this.assertEquals((a: 10, v: 100, h: 1000).use { f.valueEnvir }, 15);
	}

	test_make_envir_from_args {
		var f = { |a = 1, b = 2| var x = 3; [a, b] };
		this.assertEquals(f.def.keyValuePairsFromArgs, [\a, 1, \b, 2]);
		this.assertEquals(f.def.makeEnvirFromArgs, (a: 1, b: 2));
		// Method's version leaves out `this`.
		this.assertEquals(this.method(\plain).keyValuePairsFromArgs, [\a, nil, \b, 2, \c, [1, 2]]);
	}

	test_argument_string {
		var f = { |a = 1, b, c = \x ... d| a };
		this.assertEquals(f.def.argumentString, "a = 1, b, c = 'x', d");
		this.assertEquals(f.def.argumentString(withEllipsis: true), "a = 1, b, c = 'x' ... d");
	}

	test_keyword_arguments {
		var f = { |a = 1, b = 2, c = 3| [a, b, c] };
		this.assertEquals(f.value(c: 30, a: 10), [10, 2, 30]);
		this.assertEquals(f.performKeyValuePairs(\value, [\c, 30, \a, 10]), [10, 2, 30]);
		this.assertEquals(f.functionPerformList(\value, [5]), [5, 2, 3]);
		this.assertEquals(this.plain(b: 9, a: 1), 1);
		this.assertEquals(this.keywords(1, x: 4), [1, 4, [], [\x, 4]]);
		this.assertEquals(this.keywords(1, 2, 3, b: 7), [1, 7, [3], []]);
	}

	test_keyword_arguments_with_hoisted_variables {
		// A keyword naming a hoisted variable is not an argument; it goes to kwargs.
		var f = { |a = 1 ... rest, kwargs| var v = 2; if (a > 0) { var h = 3; v = v + h }; [a, v, kwargs] };
		this.assertEquals(f.value(a: 5, h: 100), [5, 5, [\h, 100]]);
	}

	test_synth_def_controls {
		var def = SynthDef(\testCompilerMetadata, { |freq = 440, amp = 0.1, bufs = #[1, 2, 3], t_trig = 0|
			Out.ar(0, SinOsc.ar(freq) * amp)
		});
		this.assertEquals(def.allControlNames.collect { |c| [c.name, c.defaultValue, c.rate] },
			[[\freq, 440, \control], [\amp, 0.1, \control], [\bufs, [1, 2, 3], \control], [\t_trig, 0, \trigger]]);
	}

	test_synth_def_controls_ignore_hoisted_variables {
		var def = SynthDef(\testCompilerMetadata2, { |freq = 440|
			var x = freq * 2;
			if (true) { var y = 3; x = x + y };
			Out.ar(0, SinOsc.ar(x))
		});
		this.assertEquals(def.allControlNames.collect { |c| [c.name, c.defaultValue] }, [[\freq, 440]]);
	}
}

+ Object {
	testCompilerMetadataExtension { |k = 9| ^k * 2 }
}
