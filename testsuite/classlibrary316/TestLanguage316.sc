//# VERSION 3.16
// Runtime checks of what 3.16 adds to the language. They check values, as a bytecode dump can be as expected and still
// be wrong when it runs.

TestLanguage316Holder {
	var <>inst, <>other;
	classvar <>cv, <>cv2;

	// The shapes of an assign and a class variable assign shortcut.
	setInst { |a| inst = a }
	setSecond { |a, b| other = b }
	setLiteral { inst = 5 }
	setDefault { |a, b = 7| other = b }
	setReturned { |a| ^inst = a }
	hiddenAssign { |a| var inst; inst = a }
	*setCv { |a| cv = a }
	*setCvSecond { |a, b| cv2 = b }
	*setCvDefault { |a, b = 9| cv2 = b }
	*getCv { ^cv }
}

TestLanguage316 : UnitTest {

	// Hoisted variables

	test_hoisted_reinitialised_each_pass {
		var results = List.new;
		var c = \meow;
		3.do { |i| var c = i * 10; results.add(c) };
		this.assertEquals(results, List[0, 10, 20]);
		this.assertEquals(c, \meow);
	}

	test_hoisted_without_value_is_nil_each_pass {
		var results = List.new;
		3.do { |i| var c; results.add(c); c = i };
		this.assertEquals(results, List[nil, nil, nil]);
	}

	test_hoisted_in_while {
		var results = List.new, n = 0;
		while { n < 3 } { var c; c = c ? 0 + n; results.add(c); n = n + 1 };
		this.assertEquals(results, List[0, 1, 2]);
	}

	test_hoisted_in_if_branches {
		var f = { |a| if (a) { var c = a.asString; c ++ "!" } { var d = 2; d + 1 } };
		this.assertEquals(f.(true), "true!");
		this.assertEquals(f.(false), 3);
	}

	test_hoisted_captured_is_fresh_each_pass {
		var functions = List.new;
		3.do { |i| var c = i; functions.add({ c }) };
		this.assertEquals(functions.collect(_.value), [0, 1, 2]);
	}

	test_hoisted_captured_and_changed {
		var functions = List.new;
		3.do { |i| var c = i; functions.add({ c = c + 1 }) };
		this.assertEquals(functions.collect(_.value), [1, 2, 3]);
		this.assertEquals(functions.collect(_.value), [2, 3, 4]);
	}

	test_hoisted_nested_inlined {
		var results = List.new;
		2.do { |i| var a = i; 2.do { |j| var b = a * 10 + j; results.add(b) } };
		this.assertEquals(results, List[0, 1, 10, 11]);
	}

	// Mid-body var

	test_mid_body_var {
		var r;
		1;
		var a = 2;
		r = a;
		var b = a + 1;
		this.assertEquals([r, b], [2, 3]);
	}

	test_mid_body_var_in_function {
		var f = { |x| x + 0; var y = x + 1; y * 2 };
		this.assertEquals(f.(1), 4);
	}

	test_mid_body_var_hides_outer_after_declaration {
		var x = 1;
		var f = { var before = x; var x = 2; [before, x] };
		this.assertEquals(f.value, [1, 2]);
	}

	test_mid_body_var_in_inlined_function {
		var results = List.new;
		3.do { |i| results.add(i); var j = i * 2; results.add(j) };
		this.assertEquals(results, List[0, 0, 1, 2, 2, 4]);
	}

	// var #a, b = x

	test_var_pack {
		var #a, b = [1, 2];
		this.assertEquals([a, b], [1, 2]);
	}

	test_var_pack_short_source {
		var #a, b, c = [1, 2];
		this.assertEquals([a, b, c], [1, 2, nil]);
	}

	test_var_pack_mid_body {
		var r;
		r = 0;
		var #x, y = [3, 4];
		this.assertEquals(r + x + y, 7);
	}

	test_var_pack_in_loop {
		var results = List.new;
		2.do { |i| var #a, b = [i, i + 10]; results.add(a + b) };
		this.assertEquals(results, List[10, 12]);
	}

	test_var_pack_captured {
		var #a, b = [1, 2];
		var f = { a + b };
		a = 10;
		this.assertEquals(f.value, 12);
	}

	// Assign and class variable shortcuts

	test_assign_shortcut {
		var h = TestLanguage316Holder.new;
		h.setInst(3);
		this.assertEquals(h.inst, 3);
		this.assertEquals(h.setInst(4), h, "a shortcut without ^ returns this");
		this.assertEquals(h.inst, 4);
	}

	test_assign_second_argument {
		var h = TestLanguage316Holder.new;
		h.setSecond(1, 2);
		this.assertEquals(h.other, 2);
		this.assertEquals(h.inst, nil);
	}

	test_assign_literal {
		var h = TestLanguage316Holder.new;
		h.setLiteral;
		this.assertEquals(h.inst, 5);
	}

	test_assign_default_and_keyword {
		var h = TestLanguage316Holder.new;
		h.setDefault(1);
		this.assertEquals(h.other, 7);
		h.setDefault(1, 8);
		this.assertEquals(h.other, 8);
		h.setDefault(b: 6);
		this.assertEquals(h.other, 6);
	}

	test_assign_returned {
		var h = TestLanguage316Holder.new;
		this.assertEquals(h.setReturned(9), 9);
		this.assertEquals(h.inst, 9);
	}

	test_assign_hidden_by_local {
		var h = TestLanguage316Holder.new;
		h.inst = 1;
		h.hiddenAssign(2);
		this.assertEquals(h.inst, 1);
	}

	test_class_variable_shortcuts {
		TestLanguage316Holder.setCv(3);
		this.assertEquals(TestLanguage316Holder.getCv, 3);
		this.assertEquals(TestLanguage316Holder.cv, 3);
		TestLanguage316Holder.setCvSecond(1, 4);
		this.assertEquals(TestLanguage316Holder.cv2, 4);
		TestLanguage316Holder.setCvDefault(1);
		this.assertEquals(TestLanguage316Holder.cv2, 9);
		TestLanguage316Holder.setCvDefault(b: 2);
		this.assertEquals(TestLanguage316Holder.cv2, 2);
	}

	// Generators

	test_generator_routine {
		var r = {: x * 2, x <- [1, 2, 3] };
		this.assertEquals([r.next, r.next, r.next, r.next], [2, 4, 6, nil]);
	}

	test_generator_all {
		this.assertEquals(all {: x + y, x <- [1, 2], y <- [10, 20] }, [11, 21, 12, 22]);
	}

	test_generator_condition_and_declare {
		this.assertEquals(all {: y, x <- (1..6), x.odd, var y = x * 3 }, [3, 9, 15]);
	}

	test_generator_while_guard {
		this.assertEquals(all {: x, x <- (1..), :while x < 5 }, [1, 2, 3, 4]);
	}

	test_generator_side_effect {
		var seen = List.new;
		var r = all {: x, x <- [1, 2], :: seen.add(x * 10) };
		this.assertEquals(r, [1, 2]);
		this.assertEquals(seen, List[10, 20]);
	}

	test_generator_immediate {
		var seen = List.new;
		var r = {; seen.add(x), x <- [1, 2, 3] };
		this.assertEquals(seen, List[1, 2, 3]);
		this.assertEquals(r, [1, 2, 3]);
	}

	test_generator_var_uses_earlier_var {
		this.assertEquals(all {: z, x <- [1, 2], var y = x * 2, var z = y + 1 }, [3, 5]);
	}

	test_generator_var_redeclared_shadows {
		this.assertEquals(
			all {: [x, y], x <- [1, 2], var y = x, var y = y * 10 },
			[[1, 10], [2, 20]]
		);
	}

	test_generator_var_between_sources {
		this.assertEquals(
			all {: [x, y], x <- [1, 2], var k = x * 10, y <- [k, k + 1] },
			[[1, 10], [1, 11], [2, 20], [2, 21]]
		);
	}

	test_generator_var_in_guard_and_side_effect {
		var seen = List.new;
		var r = all {: y, x <- (1..5), var y = x * x, y.odd, :: seen.add(y), :while y < 20 };
		this.assertEquals(r, [1, 9]);
		this.assertEquals(seen, List[1, 9, 25], "the side effect runs before :while stops");
	}

	test_generator_var_captured_fresh_each_element {
		var fs = all {: { y }, x <- [1, 2, 3], var y = x * 10 };
		this.assertEquals(fs.collect(_.value), [10, 20, 30]);
	}

	test_generator_var_reads_outer_local {
		var k = 100;
		this.assertEquals(all {: y, x <- [1, 2], var y = x + k }, [101, 102]);
	}

	test_generator_var_routine_next {
		var r = {: y, x <- [1, 2], var y = x + 1 };
		this.assertEquals([r.next, r.next, r.next], [2, 3, nil]);
	}

	test_generator_var_immediate {
		var seen = List.new;
		{; seen.add(y), x <- [1, 2], var y = x * 5 };
		this.assertEquals(seen, List[5, 10]);
	}

	test_generator_inside_loop_with_var {
		var results = List.new;
		2.do { |i| var base = i * 10; results.add(all {: base + y, y <- [1, 2], var z = y }) };
		this.assertEquals(results, List[[1, 2], [11, 12]]);
	}

	test_generator_captures_each_element {
		var fs = all {: { x }, x <- [1, 2, 3] };
		this.assertEquals(fs.collect(_.value), [1, 2, 3]);
	}

	// Closed functions

	test_closed_function {
		var f = #{ |a| a + 1 };
		this.assertEquals(f.(1), 2);
		this.assert(f.isClosed, "#{ } is closed");
	}

	test_open_function_is_not_closed {
		var x = 1;
		this.assert({ x }.isClosed.not, "a function using a local isn't closed");
	}

	test_closed_function_inside_method_with_return {
		var f = { |x| if (x) { ^1 }; #{ 3 } };
		var g = #{ 2 };
		this.assertEquals(g.value, 2);
		this.assertEquals(f.(false).value, 3);
	}

	// Folding

	test_folded_mutable_is_fresh {
		var f = { [1, "a"] };
		var a = f.value, b = f.value;
		this.assert(a !== b, "each call makes a new array");
		this.assert(a[1] !== b[1], "and a new string");
		a[1] = "b";
		a.add(3);
		this.assertEquals(f.value, [1, "a"]);
	}

	test_folded_mutable_in_loop {
		var results = List.new;
		2.do { var a = [1, 2]; a.add(3); results.add(a.size) };
		this.assertEquals(results, List[3, 3]);
	}

	test_folded_variable_known_value {
		var b = 1;
		this.assertEquals(b + 1, 2);
	}

	test_folded_arithmetic {
		this.assertEquals((1 + 2) * 3, 9);
		this.assertEquals(2 ** 3, 8.0);
	}

	test_negative_zero_is_kept {
		var zero = 0.0, negative = -0.0;
		this.assertEquals(1 / negative, -inf, "-0.0 is not folded into 0.0");
		this.assertEquals(1 / zero, inf);
		this.assertEquals(1 / [0.0, -0.0][1], -inf, "nor in an array");
		this.assertEquals(1 / [-0.0, 0.0][1], inf);
	}

	test_literal_is_shared {
		var f = { #[1, 2] };
		this.assert(f.value === f.value, "a literal array is the same object");
	}

	test_known_nil_variable {
		var b;
		this.assertEquals(b, nil);
		b = 2;
		this.assertEquals(b, 2);
	}

	// Non-local returns

	test_nonlocal_return_in_if {
		this.assertEquals(this.nlrIf(true), 1);
		this.assertEquals(this.nlrIf(false), 2);
	}

	nlrIf { |x| if (x) { ^1 }; ^2 }

	nlrNested { |x| ^[1, 2].collect { |i| if (i == x) { ^i * 10 }; i } }

	test_nonlocal_return_nested {
		this.assertEquals(this.nlrNested(2), 20);
		this.assertEquals(this.nlrNested(5), [1, 2]);
	}

	nlrInlinedVar { |x| if (x) { var c = x.asString; if (c == "true") { ^1 }; ^c }; ^2 }

	test_nonlocal_return_inlined_with_variable {
		this.assertEquals(this.nlrInlinedVar(true), 1);
		this.assertEquals(this.nlrInlinedVar(false), 2);
	}

	nlrLoop { var n = 0; while { true } { var m = n; n = n + 1; if (m == 3) { ^m } } }

	test_nonlocal_return_in_while {
		this.assertEquals(this.nlrLoop, 3);
	}

	nlrInDo { 5.do { |i| var j = i; if (j == 2) { ^j } }; ^nil }

	test_nonlocal_return_in_do {
		this.assertEquals(this.nlrInDo, 2);
	}

	nlrThroughFunction { |x| var f = { |y| if (y) { ^1 }; 2 }; ^f.(x) + 100 }

	test_nonlocal_return_skips_rest {
		this.assertEquals(this.nlrThroughFunction(true), 1);
		this.assertEquals(this.nlrThroughFunction(false), 102);
	}

	nlrAfterMidVar { |x| if (x) { ^1 }; var a = 5; ^a }

	test_nonlocal_return_before_mid_body_var {
		this.assertEquals(this.nlrAfterMidVar(true), 1);
		this.assertEquals(this.nlrAfterMidVar(false), 5);
	}

	// Interpreted code is 3.16 too

	test_interpreted_mid_body_var {
		this.assertEquals("1; var a = 2; a + 1".interpret, 3);
	}

	test_interpreted_var_pack {
		this.assertEquals("var #a, b = [1, 2]; a + b".interpret, 3);
	}

	test_interpreted_hoisted {
		this.assertEquals("var r = List.new; 3.do { |i| var c = i; r.add(c) }; r".interpret, List[0, 1, 2]);
	}

	test_interpreted_generator {
		this.assertEquals("all {: x * 2, x <- [1, 2] }".interpret, [2, 4]);
	}
}
