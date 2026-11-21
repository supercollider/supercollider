//# VERSION 3.16
// The sizes the bytecode format limits: table entries, frame slots, arguments, jump lengths and nesting, and the
// encodings that change with size (integer literals by width, literal indexes). The code is generated and compiled
// with the interpreter, so each limit is tested at the largest size that compiles: that size must run correctly,
// and one more must be rejected with a diagnostic rather than produce wrong code. The diagnostics are printed.

TestCompilerLimits : UnitTest {

	////////////////////////////////////////////////////////////////////////////////
	// Helpers
	////////////////////////////////////////////////////////////////////////////////

	compile { |source| ^thisProcess.interpreter.compile(source) }
	compiles { |source| ^this.compile(source).notNil }
	valueOf { |source| var f = this.compile(source); ^if (f.isNil) { \did_not_compile } { f.value } }
	id { |x| ^x }

	// The largest n in lo..hi for which make.(n) compiles, given lo compiles and hi does not.
	largestThatCompiles { |make, lo, hi|
		while { hi - lo > 1 } {
			var mid = (lo + hi) div: 2;
			if (this.compiles(make.(mid))) { lo = mid } { hi = mid };
		};
		^lo
	}

	// Checks a limit: lo compiles, hi does not, and the largest size that compiles gives expect.(n).
	checkLimit { |name, make, lo, hi, expect|
		var n;
		this.assert(this.compiles(make.(lo)), "%: % compiles".format(name, lo));
		this.assert(this.compiles(make.(hi)).not, "%: % is rejected".format(name, hi));
		n = this.largestThatCompiles(make, lo, hi);
		this.assertEquals(this.valueOf(make.(n)), expect.(n), "%: runs correctly at the largest size, %".format(name, n));
		this.assert(this.compiles(make.(n + 1)).not, "%: one more than the largest is rejected".format(name));
	}

	// 250 variables, so the if body's 10 don't fit and it is a function with its own frame.
	returnFromRefusedBody { |x|
		var w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15, w16, w17, w18, w19, w20, w21, w22, w23, w24, w25, w26, w27, w28, w29, w30, w31, w32, w33, w34, w35, w36, w37, w38, w39, w40, w41, w42, w43, w44, w45, w46, w47, w48, w49, w50, w51, w52, w53, w54, w55, w56, w57, w58, w59, w60, w61, w62, w63, w64, w65, w66, w67, w68, w69, w70, w71, w72, w73, w74, w75, w76, w77, w78, w79, w80, w81, w82, w83, w84, w85, w86, w87, w88, w89, w90, w91, w92, w93, w94, w95, w96, w97, w98, w99, w100, w101, w102, w103, w104, w105, w106, w107, w108, w109, w110, w111, w112, w113, w114, w115, w116, w117, w118, w119, w120, w121, w122, w123, w124, w125, w126, w127, w128, w129, w130, w131, w132, w133, w134, w135, w136, w137, w138, w139, w140, w141, w142, w143, w144, w145, w146, w147, w148, w149, w150, w151, w152, w153, w154, w155, w156, w157, w158, w159, w160, w161, w162, w163, w164, w165, w166, w167, w168, w169, w170, w171, w172, w173, w174, w175, w176, w177, w178, w179, w180, w181, w182, w183, w184, w185, w186, w187, w188, w189, w190, w191, w192, w193, w194, w195, w196, w197, w198, w199, w200, w201, w202, w203, w204, w205, w206, w207, w208, w209, w210, w211, w212, w213, w214, w215, w216, w217, w218, w219, w220, w221, w222, w223, w224, w225, w226, w227, w228, w229, w230, w231, w232, w233, w234, w235, w236, w237, w238, w239, w240, w241, w242, w243, w244, w245, w246, w247, w248, w249;
		if (x) { var b0, b1, b2, b3, b4, b5, b6, b7, b8, b9; b0 = \fromBody; ^b0 };
		^\after
	}

	statements { |n, text| ^"".catList(Array.fill(n, text)) }
	names { |n, prefix| ^n.collect { |i| prefix ++ i }.join(", ") }

	////////////////////////////////////////////////////////////////////////////////
	// Encodings that change with the value
	////////////////////////////////////////////////////////////////////////////////

	test_integer_literal_encodings {
		// -1, 0, 1 and 2 have their own opcodes, then 8, 16, 24 and 32 bit pushes.
		this.assertEquals(this.id(-1), "-1".asInteger);
		this.assertEquals(this.id(0), "0".asInteger);
		this.assertEquals(this.id(1), "1".asInteger);
		this.assertEquals(this.id(2), "2".asInteger);
		this.assertEquals(this.id(3), "3".asInteger);
		this.assertEquals(this.id(-2), "-2".asInteger);
		this.assertEquals(this.id(127), "127".asInteger);
		this.assertEquals(this.id(128), "128".asInteger);
		this.assertEquals(this.id(-128), "-128".asInteger);
		this.assertEquals(this.id(-129), "-129".asInteger);
		this.assertEquals(this.id(255), "255".asInteger);
		this.assertEquals(this.id(256), "256".asInteger);
		this.assertEquals(this.id(32767), "32767".asInteger);
		this.assertEquals(this.id(32768), "32768".asInteger);
		this.assertEquals(this.id(-32768), "-32768".asInteger);
		this.assertEquals(this.id(-32769), "-32769".asInteger);
		this.assertEquals(this.id(65535), "65535".asInteger);
		this.assertEquals(this.id(65536), "65536".asInteger);
		this.assertEquals(this.id(8388607), "8388607".asInteger);
		this.assertEquals(this.id(8388608), "8388608".asInteger);
		this.assertEquals(this.id(-8388608), "-8388608".asInteger);
		this.assertEquals(this.id(-8388609), "-8388609".asInteger);
		this.assertEquals(this.id(16777215), "16777215".asInteger);
		this.assertEquals(this.id(16777216), "16777216".asInteger);
		this.assertEquals(this.id(2147483647), "2147483647".asInteger);
		this.assertEquals(this.id(-2147483648), "-2147483648".asInteger);
	}

	test_integer_literal_encodings_in_generated_code {
		// The same, through the interpreter, each value a separate push.
		var values = [-1, 0, 1, 2, 3, 127, 128, -128, -129, 32767, 32768, -32768, -32769,
			8388607, 8388608, -8388608, -8388609, 2147483647, -2147483648];
		var source = "var l = List.new; " ++ values.collect { |v| "l.add(" ++ v ++ ");" }.join ++ " l.asArray";
		this.assertEquals(this.valueOf(source), values);
	}

	test_float_literal_encodings {
		// 0.5, -1.0, 0.0, 1.0, 2.0 and inf have their own opcodes, the rest are literals.
		this.assertEquals(this.id(0.5), "0.5".asFloat);
		this.assertEquals(this.id(-1.0), "-1.0".asFloat);
		this.assertEquals(this.id(0.0), "0.0".asFloat);
		this.assertEquals(this.id(1.0), "1.0".asFloat);
		this.assertEquals(this.id(2.0), "2.0".asFloat);
		this.assertEquals(this.id(1.5), "1.5".asFloat);
		this.assertEquals(this.id(-0.5), "-0.5".asFloat);
		this.assertEquals(this.id(3.0), "3.0".asFloat);
		this.assertEquals(this.id(inf), 1 / 0);
		this.assertEquals(this.id(-inf), -1 / 0);
		this.assertEquals(this.id(1.0).class, Float, "1.0 is not pushed as the Integer 1");
		this.assertEquals(this.id(0.0).class, Float);
		this.assertEquals(this.id(2.0).class, Float);
	}

	test_negative_zero {
		// From 3.16 -0.0 is its own literal. The old compiler compared literals as numbers and pushed 0.0.
		this.assertEquals(1 / this.id(-0.0), -inf);
		this.assertEquals(1 / this.id(0.0), inf);
	}

	test_negative_zero_from_an_operation {
		this.assertEquals(1 / this.id(0.0).neg, -inf);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Tables
	////////////////////////////////////////////////////////////////////////////////

	test_many_literals {
		// The literal index has a short form below 16.
		[15, 16, 17, 254, 255, 256, 600].do { |n|
			var source = "var l = List.new; " ++ n.collect { |i| "l.add(\\lit" ++ i ++ ");" }.join ++ " l.asArray";
			var result = this.valueOf(source);
			this.assertEquals(result, n.collect { |i| ("lit" ++ i).asSymbol }, "% symbol literals".format(n));
		};
	}

	test_many_float_literals {
		[15, 16, 17, 300].do { |n|
			var source = "var l = List.new; " ++ n.collect { |i| "l.add(" ++ (i + 0.25) ++ ");" }.join ++ " l.asArray";
			this.assertEquals(this.valueOf(source), n.collect { |i| i + 0.25 }, "% float literals".format(n));
		};
	}

	test_many_inner_functions {
		[15, 16, 17].do { |n|
			var source = "var l = List.new; " ++ n.collect { |i| "l.add({ " ++ i ++ " });" }.join ++ " l.collect(_.value).asArray";
			this.assertEquals(this.valueOf(source), (0 .. n - 1), "% functions".format(n));
		};
	}

	test_inner_function_limit {
		// Inner functions are entries of the same table as selectors and literals.
		this.checkLimit("inner functions",
			{ |n| "var l = List.new; " ++ n.collect { |i| "l.add({ " ++ i ++ " });" }.join ++ " l.collect(_.value).asArray" },
			10, 600, { |n| (0 .. n - 1) });
	}

	test_selector_limit {
		this.checkLimit("selectors",
			{ |n| "{ |c| if (c) { " ++ n.collect { |i| "nil.sel" ++ i ++ ";" }.join ++ " }; 42 }.value(false)" },
			10, 600, { 42 });
	}

	////////////////////////////////////////////////////////////////////////////////
	// Frames and arguments
	////////////////////////////////////////////////////////////////////////////////

	test_variable_limit {
		this.checkLimit("variables",
			{ |n| "{ var " ++ this.names(n, "v") ++ "; v0 = 1; v" ++ (n - 1) ++ " = 2; v0 + v" ++ (n - 1) ++ " }.value" },
			10, 600, { 3 });
	}

	test_argument_limit {
		// 255 arguments compile, but calling such a function corrupts memory in the VM, so it is not run here.
		var make = { |n| "{ |" ++ this.names(n, "a") ++ "| a0 + a" ++ (n - 1) ++ " }.valueArray((1.." ++ n ++ "))" };
		this.assertEquals(this.valueOf(make.(254)), 255, "254 arguments");
		this.assert(this.compiles(make.(256)).not, "256 arguments are rejected");
	}

	test_arguments_and_variables_share_the_frame {
		var source = "{ |" ++ this.names(200, "a") ++ "| var " ++ this.names(55, "v") ++ "; v54 = a0; v54 }.value(9)";
		this.assertEquals(this.valueOf(source), 9, "200 arguments and 55 variables");
		source = "{ |" ++ this.names(200, "a") ++ "| var " ++ this.names(56, "v") ++ "; 0 }.value";
		this.assert(this.compiles(source).not, "200 arguments and 56 variables are rejected");
	}

	test_send_argument_limit {
		this.checkLimit("arguments of a send",
			{ |n| "Array.with(" ++ (1..n).join(", ") ++ ").size" },
			10, 600, { |n| n });
	}

	test_hoisted_variables_beyond_the_frame_limit {
		// The variables of inlined bodies are hoisted into the frame around them. The program never has more than 60
		// variables in one function, so it must compile, as it did before hoisting.
		var body = { |b, per| "var " ++ this.names(per, "h" ++ b ++ "_") ++ "; h" ++ b ++ "_0 = 1; t = t + h" ++ b ++ "_0;" };
		this.assertEquals(this.valueOf("var t = 0; " ++ 10.collect { |b| "if (t >= 0) { " ++ body.(b, 30) ++ " };" }.join ++ " t"), 10,
			"ten sibling if bodies of 30 variables");
		this.assertEquals(this.valueOf("var t = 0; " ++ 10.collect { |b| "1.do { " ++ body.(b, 30) ++ " };" }.join ++ " t"), 10,
			"ten sibling do bodies of 30 variables");
		this.assertEquals(this.valueOf("var t = 0; " ++ "".catList(5.collect { |b| "if (t >= 0) { " ++ body.(b, 60) }) ++ this.statements(5, " };") ++ " t"), 5,
			"five nested if bodies of 60 variables");
		this.assertEquals(this.valueOf("var t = 0, " ++ this.names(250, "w") ++ "; if (t >= 0) { " ++ body.(0, 10) ++ " }; t"), 1,
			"250 variables and an if body of 10");
	}

	test_body_that_would_not_fit_is_a_function {
		// A send whose bodies' variables would take the frame past 255 is sent rather than inlined, so its bodies are
		// functions. The first body here fits and is hoisted, the second doesn't.
		var vars = "var t = 0, " ++ this.names(200, "w") ++ "; ";
		var fits = "if (t >= 0) { var " ++ this.names(50, "a") ++ "; a0 = 1; t = t + a0 }; ";
		var tooBig = "if (t >= 0) { var " ++ this.names(10, "b") ++ "; b9 = 10; t = t + b9 }; ";
		this.assertEquals(this.valueOf(vars ++ fits ++ tooBig ++ "t"), 11, "both bodies run");
		this.assertEquals(this.valueOf(vars ++ fits ++ "if (t < 0) { var " ++ this.names(10, "b") ++ "; t = -1 } { var c = 5; t = t + c }; t"), 6,
			"the else body of a refused if runs");
	}

	test_return_from_a_body_that_would_not_fit {
		// The refused body is a closure, so its ^ is a non-local return; inside interpreted code that is an error,
		// so this is a method.
		this.assertEquals(this.returnFromRefusedBody(true), \fromBody);
		this.assertEquals(this.returnFromRefusedBody(false), \after);
	}

	test_while_that_would_not_fit {
		var source = "var i = 0, " ++ this.names(250, "w") ++ "; while { i < 3 } { var " ++ this.names(10, "b") ++ "; b0 = 1; i = i + b0 }; i";
		this.assertEquals(this.valueOf(source), 3);
	}

	test_captured_variable_in_a_body_that_would_not_fit {
		var source = "var t = 0, f, " ++ this.names(250, "w") ++ "; if (t >= 0) { var " ++ this.names(10, "b") ++ "; b3 = 9; f = { b3 } }; f.value";
		this.assertEquals(this.valueOf(source), 9);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Jumps
	////////////////////////////////////////////////////////////////////////////////

	test_if_body_jump_limit {
		this.checkLimit("if body",
			{ |n| "{ |c| var x = 0; if (c) { " ++ this.statements(n, "x = x + 1;") ++ " }; x }.value(true)" },
			100, 40000, { |n| n });
	}

	test_if_body_jump_limit_with_an_else {
		this.checkLimit("if body before an else",
			{ |n| "{ |c| var x = 0; if (c) { " ++ this.statements(n, "x = x + 1;") ++ " } { x = -1 }; x }.value(true)" },
			100, 40000, { |n| n });
		this.checkLimit("else body",
			{ |n| "{ |c| var x = 0; if (c) { x = -1 } { " ++ this.statements(n, "x = x + 1;") ++ " }; x }.value(false)" },
			100, 40000, { |n| n });
	}

	test_while_body_jump_limit {
		this.checkLimit("while body",
			{ |n| "var i = 0, x = 0; while { i < 1 } { " ++ this.statements(n, "x = x + 1;") ++ " i = i + 1 }; x" },
			100, 40000, { |n| n });
	}

	test_switch_branch_jump_limit {
		// The big branch is second, so the jump from the end of the first branch crosses it.
		this.checkLimit("switch branch",
			{ |n| "var f = { |k| var x = 0; switch (k, 1, { x = -1 }, 2, { " ++ this.statements(n, "x = x + 1;") ++ " }, { x = -2 }); x }; [f.(1), f.(2), f.(3)]" },
			100, 40000, { |n| [-1, n, -2] });
	}

	test_and_jump_limit {
		this.checkLimit("and body",
			{ |n| "{ |c| var x = 0; c and: { " ++ this.statements(n, "x = x + 1;") ++ " true }; x }.value(true)" },
			100, 40000, { |n| n });
	}

	test_long_straight_line_code {
		// No jump crosses it, so its length is not limited.
		this.assertEquals(this.valueOf("var x = 0; " ++ this.statements(30000, "x = x + 1;") ++ " x"), 30000);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Nesting and size
	////////////////////////////////////////////////////////////////////////////////

	test_deep_parentheses {
		this.assertEquals(this.valueOf(this.statements(3000, "(") ++ "1" ++ this.statements(3000, ")")), 1);
	}

	test_nested_if_limit {
		this.checkLimit("nested ifs",
			{ |n| "var x = 1; " ++ this.statements(n, "if (x > 0) { ") ++ "x = 7" ++ this.statements(n, " }") ++ "; x" },
			10, 6000, { 7 });
	}

	test_deep_closures {
		// The innermost function reads a variable n functions out. The bytecode counts those functions in a byte, so
		// 255 is the most, and one more is a diagnostic rather than a read of the wrong variable.
		this.checkLimit("reading a variable functions out",
			{ |n| "{ var a = 5; " ++ this.statements(n, "{ ") ++ "a" ++ this.statements(n, " }") ++ " }" ++ this.statements(n + 1, ".value") },
			10, 300, { 5 });
	}

	test_deep_closures_assigning {
		this.checkLimit("assigning a variable functions out",
			{ |n| "var a = 0; " ++ this.statements(n, "{ ") ++ "a = a + 1" ++ this.statements(n, " }") ++ this.statements(n, ".value") ++ "; a" },
			10, 300, { 1 });
	}

	test_deep_closures_using_only_their_own_variables {
		// Only a use that far out is limited, not the nesting.
		var n = 300;
		var source = this.statements(n, "{ ") ++ "var b = 3; b" ++ this.statements(n, " }") ++ this.statements(n, ".value");
		this.assertEquals(this.valueOf(source), 3);
	}

	test_deep_closures_through_inlined_bodies {
		// An inlined body has no frame, so it doesn't count.
		var source = "var a = 5, t = true; " ++ this.statements(300, "if (t) { ") ++ "{ a }.value" ++ this.statements(300, " }");
		this.assertEquals(this.valueOf(source), 5);
	}

	test_long_string_literal {
		this.assertEquals(this.valueOf("\"" ++ String.fill(100000, $a) ++ "\".size"), 100000);
	}

	test_large_literal_array {
		this.assertEquals(this.valueOf("#[" ++ (0 .. 9999).join(", ") ++ "].sum"), 49995000);
	}

	test_large_array_expression {
		// Each element is a variable, so each is pushed.
		[1000, 5000].do { |n|
			this.assertEquals(this.valueOf("var x = 1; [" ++ Array.fill(n, "x").join(", ") ++ "].sum"), n, "% elements".format(n));
		};
	}
}
