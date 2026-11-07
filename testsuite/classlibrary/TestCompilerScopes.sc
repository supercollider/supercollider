//# VERSION 3.16
// Variables, arguments and closures. In 3.16 a variable declared in an inlined body (a loop body, an if branch, ...)
// is hoisted into the enclosing frame, unless a closure captures it. Hoisting must not change what a program means,
// so these tests look for the ways it could:
//   - a variable that must start as nil on every pass, and whose initialiser must run on every pass,
//   - a variable that must not clobber an outer variable of the same name,
//   - a variable that a closure captures, which needs a fresh copy per pass,
//   - a variable that must survive a yield, a recursive call, or a re-entrant call.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerScopes : UnitTest {

	// Helpers that return closures, so the closures outlive the method that made them.
	counterPair {
		var n = 0;
		^[{ n = n + 1 }, { n }]
	}

	makeAdder { |k| ^{ |x| x + k } }

	makeFunctionsInLoop {
		var fs = List.new;
		3.do { |i| var v; v = i * 7; fs.add { v } };
		^fs.collect { |f| f.value }
	}

	recursiveMine { |n|
		var out;
		if (n > 0) {
			var mine;
			mine = n * 10;
			this.recursiveMine(n - 1);
			out = mine;
		} {
			out = 0;
		};
		^out
	}

	recursiveDo { |depth, log|
		if (depth > 0) {
			2.do { |i|
				var local;
				local = depth * 10 + i;
				log.add(local);
				this.recursiveDo(depth - 1, log);
				log.add(local);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// A variable of a loop body starts as nil on every pass
	////////////////////////////////////////////////////////////////////////////////

	test_body_var_is_nil_each_pass_do {
		var seen = List.new;
		3.do { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_do_with_argument {
		var seen = List.new;
		3.do { |i| var v; seen.add(v); v = i };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_reverseDo {
		var seen = List.new;
		3.reverseDo { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_for {
		var seen = List.new;
		1.for(3) { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_forBy {
		var seen = List.new;
		1.forBy(5, 2) { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_float_do {
		var seen = List.new;
		3.0.do { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_array_do {
		var seen = List.new;
		[1, 2, 3].do { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
		seen.clear;
		[1, 2, 3].reverseDo { var v; seen.add(v); v = 7 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_while {
		var i = 0, seen = List.new;
		while { i < 3 } { var v; seen.add(v); v = i; i = i + 1 };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_while_condition {
		var i = 0, seen = List.new;
		while { var c; seen.add(c); c = i; i = i + 1; i < 3 } { };
		this.assertEquals(seen.asArray, [nil, nil, nil], "a variable of the condition function too");
	}

	test_body_var_is_nil_each_pass_loop {
		var n = 0, seen = List.new;
		try {
			loop {
				var v;
				seen.add(v);
				v = n;
				n = n + 1;
				if (n == 3) { Error("stop").throw };
			}
		};
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_if_branch_in_loop {
		var seen = List.new;
		3.do { if (true) { var v; seen.add(v); v = 5 } };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_nested_loops {
		var seen = List.new;
		2.do { 2.do { var v; seen.add(v); v = 1 } };
		this.assertEquals(seen.asArray, [nil, nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_in_both_branches {
		var seen = List.new;
		4.do { |i|
			if (i.even) { var a; seen.add(a); a = 1 } { var b; seen.add(b); b = 2 }
		};
		this.assertEquals(seen.asArray, [nil, nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_after_return_from_inner {
		// The variable still has a value from the previous pass when the next one starts, unless it is reset.
		var seen = List.new;
		3.do { |i| var v; seen.add(v); v = i; if (i == 0) { v = \zero } };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_body_var_is_nil_each_pass_in_case_and_switch {
		var seen = List.new;
		3.do { |i|
			case { true } { var c; seen.add(c); c = 1 };
			switch (i, 0, { var s; seen.add(s); s = 1 }, 1, { var s; seen.add(s); s = 1 }, { var s; seen.add(s); s = 1 });
		};
		this.assertEquals(seen.asArray, [nil, nil, nil, nil, nil, nil]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// The initialiser of a body variable runs on every pass
	////////////////////////////////////////////////////////////////////////////////

	test_initialiser_runs_each_pass {
		var n = 0, seen = List.new;
		3.do { var k = n; n = n + 1; seen.add(k) };
		this.assertEquals(seen.asArray, [0, 1, 2]);
	}

	test_initialiser_makes_a_fresh_object_each_pass {
		var sizes = List.new, lists = List.new;
		3.do { var acc = List.new; acc.add(1); sizes.add(acc.size); lists.add(acc) };
		this.assertEquals(sizes.asArray, [1, 1, 1], "each pass starts from an empty list");
		this.assert(lists[0] !== lists[1] and: { lists[1] !== lists[2] }, "and they are different objects");
	}

	test_initialiser_in_while {
		var i = 0, seen = List.new;
		while { i < 3 } { var k = i * 2; seen.add(k); i = i + 1 };
		this.assertEquals(seen.asArray, [0, 2, 4]);
	}

	test_initialiser_in_branch {
		var seen = List.new;
		4.do { |i| if (i.odd) { var k = i * 3; seen.add(k) } { var k = i * 5; seen.add(k) } };
		this.assertEquals(seen.asArray, [0, 3, 10, 9]);
	}

	test_several_vars_in_one_declaration {
		var seen = List.new;
		2.do { |i| var a, b = 5, c; seen.add([a, b, c]); a = i; c = i; b = i };
		this.assertEquals(seen.asArray, [[nil, 5, nil], [nil, 5, nil]]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Shadowing: an inner variable never changes the outer one
	////////////////////////////////////////////////////////////////////////////////

	test_inner_var_does_not_clobber_outer_var {
		var x = 1;
		3.do { var x; x = 10; x = x + 1 };
		this.assertEquals(x, 1);
	}

	test_inner_var_does_not_clobber_outer_var_in_branch {
		var x = 1;
		if (true) { var x; x = 10 };
		this.assertEquals(x, 1);
		if (false) { 0 } { var x; x = 20 };
		this.assertEquals(x, 1);
	}

	test_inner_var_does_not_clobber_outer_var_in_while {
		var x = 1, i = 0;
		while { i < 2 } { var x; x = 10; i = i + 1 };
		this.assertEquals(x, 1);
	}

	test_inner_var_reads_as_nil_not_as_outer {
		var x = 5, seen;
		3.do { var x; seen = x };
		this.assertEquals(seen, nil, "the inner x is a new variable, it does not start with the outer value");
		this.assertEquals(x, 5);
	}

	test_inner_var_does_not_clobber_argument {
		var f = { |x| 3.do { var x; x = 99 }; x };
		this.assertEquals(f.(9), 9);
	}

	test_inner_argument_does_not_clobber_outer_var {
		var i = \outer;
		3.do { |i| i = i + 1 };
		this.assertEquals(i, \outer);
	}

	test_inner_argument_shadows_for_the_whole_body {
		var i = \outer, seen = List.new;
		3.do { |i| seen.add(i) };
		this.assertEquals(seen.asArray, [0, 1, 2]);
		this.assertEquals(i, \outer);
	}

	test_same_name_in_sibling_bodies {
		var a = List.new;
		if (true) { var t; t = 1; a.add(t) };
		if (true) { var t; t = 2; a.add(t) };
		3.do { |i| var t; t = i + 10; a.add(t) };
		this.assertEquals(a.asArray, [1, 2, 10, 11, 12]);
	}

	test_same_name_in_nested_bodies {
		var out = List.new;
		2.do { |i|
			var t;
			t = \outer;
			2.do { |j| var t; t = [i, j]; out.add(t) };
			out.add(t);
		};
		this.assertEquals(out.asArray, [[0, 0], [0, 1], \outer, [1, 0], [1, 1], \outer]);
	}

	test_same_name_in_nested_branches {
		var f = { |a, b|
			var r;
			if (a) { var t; t = 1; if (b) { var t; t = 2; r = t } { r = t } } { var t; t = 3; r = t };
			r
		};
		this.assertEquals(f.(true, true), 2);
		this.assertEquals(f.(true, false), 1);
		this.assertEquals(f.(false, true), 3);
	}

	test_inner_var_shadowing_a_var_a_closure_captured {
		var x = 1, f = { x };
		3.do { var x; x = 50 };
		this.assertEquals(f.value, 1);
		x = 2;
		this.assertEquals(f.value, 2);
	}

	test_var_with_the_name_of_a_method_or_class {
		var size = 3, class = \c, value = 4;
		this.assertEquals(size + value, 7);
		this.assertEquals(class, \c);
		this.assertEquals([1, 2].size, 2, "a message with the same name as a variable is still a message");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Closures capture the variable of the pass that made them
	////////////////////////////////////////////////////////////////////////////////

	test_closure_over_loop_argument {
		var fs = List.new;
		3.do { |i| fs.add { i * 10 } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 10, 20]);
	}

	test_closure_over_body_var_do {
		var fs = List.new;
		3.do { |i| var v; v = i * 2; fs.add { v } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 2, 4]);
	}

	test_closure_over_body_var_reverseDo {
		var fs = List.new;
		3.reverseDo { |i| var v; v = i * 2; fs.add { v } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [4, 2, 0]);
	}

	test_closure_over_body_var_for_and_forBy {
		var fs = List.new;
		1.for(3) { |i| var v; v = i; fs.add { v } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [1, 2, 3]);
		fs.clear;
		1.forBy(5, 2) { |i| var v; v = i; fs.add { v } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [1, 3, 5]);
	}

	test_closure_over_body_var_array_do {
		var fs = List.new;
		[\a, \b, \c].do { |item| var v; v = item; fs.add { v } };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [\a, \b, \c]);
	}

	test_closure_over_body_var_while {
		var i = 0, fs = List.new;
		while { i < 3 } { var v; v = i; fs.add { v }; i = i + 1 };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 1, 2]);
	}

	test_closure_over_body_var_loop {
		var n = 0, fs = List.new;
		try {
			loop {
				var v;
				v = n;
				fs.add { v };
				n = n + 1;
				if (n == 3) { Error("stop").throw };
			}
		};
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 1, 2]);
	}

	test_closure_over_body_var_made_in_a_method {
		this.assertEquals(this.makeFunctionsInLoop, [0, 7, 14]);
	}

	test_closure_over_branch_var {
		var fs = List.new;
		4.do { |i|
			if (i.odd) { var v; v = i; fs.add { v } } { var w; w = i * 100; fs.add { w } }
		};
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 1, 200, 3]);
	}

	test_closure_over_switch_and_case_var {
		var fs = List.new;
		3.do { |i|
			switch (i, 0, { var v; v = \a; fs.add { v } }, 1, { var v; v = \b; fs.add { v } }, { var v; v = \c; fs.add { v } });
			case { true } { var v; v = i * 5; fs.add { v } };
		};
		this.assertEquals(fs.collect { |f| f.value }.asArray, [\a, 0, \b, 5, \c, 10]);
	}

	test_closure_sees_mutation_after_the_pass {
		var fs = List.new;
		3.do { |i| var v; v = i; fs.add { v }; v = v + 100 };
		this.assertEquals(fs.collect { |f| f.value }.asArray, [100, 101, 102]);
	}

	test_closure_sees_later_mutation_of_outer_var {
		var x = 1, f = { x };
		x = 2;
		this.assertEquals(f.value, 2);
	}

	test_closure_mutates_outer_var {
		var x = 1, f = { x = x + 1 };
		f.value;
		f.value;
		this.assertEquals(x, 3);
	}

	test_closures_share_a_variable {
		var x = 0, inc = { x = x + 1 }, get = { x };
		inc.value;
		inc.value;
		this.assertEquals(get.value, 2);
	}

	test_closures_of_different_passes_do_not_share {
		var incs = List.new, gets = List.new;
		2.do {
			var n;
			n = 0;
			incs.add { n = n + 1 };
			gets.add { n };
		};
		incs[0].value;
		incs[0].value;
		incs[1].value;
		this.assertEquals(gets[0].value, 2);
		this.assertEquals(gets[1].value, 1);
	}

	test_closure_outlives_the_method {
		var pair = this.counterPair;
		var other = this.counterPair;
		pair[0].value;
		pair[0].value;
		other[0].value;
		this.assertEquals(pair[1].value, 2);
		this.assertEquals(other[1].value, 1, "each call of the method has its own variable");
	}

	test_closure_over_an_argument_of_a_method {
		var add3 = this.makeAdder(3), add10 = this.makeAdder(10);
		this.assertEquals(add3.(1), 4);
		this.assertEquals(add10.(1), 11);
		this.assertEquals(add3.(5), 8);
	}

	test_closure_over_an_assigned_argument {
		var f = { |a| var g; g = { a }; a = a + 1; g };
		this.assertEquals(f.(1).value, 2, "the closure sees the argument as it is now");
	}

	test_closure_over_this {
		var f = { this.class };
		this.assertEquals(f.value, TestCompilerScopes);
		3.do { |i| var g; g = { this.class }; this.assertEquals(g.value, TestCompilerScopes) };
	}

	test_closure_in_closure_in_loop {
		var out = List.new;
		2.do { |i|
			var f;
			f = { |j| { i * 10 + j } };
			out.add(f.(1).value);
			out.add(f.(2).value);
		};
		this.assertEquals(out.asArray, [1, 2, 11, 12]);
	}

	test_closure_captures_two_levels_up {
		var a = 1;
		var f = { var b; b = 2; { |c| a + b + c } };
		this.assertEquals(f.value.value(3), 6);
		a = 10;
		this.assertEquals(f.value.value(3), 15);
	}

	test_deep_nesting_with_arguments {
		this.assertEquals({ |a| { |b| { |c| { |d| { |e| { |f| a + b + c + d + e + f } } } } } }.(1).(2).(3).(4).(5).(6), 21);
	}

	test_deep_nesting_with_variables {
		var f = { var a; a = 1; { var b; b = 2; { var c; c = 3; { var d; d = 4; { a + b + c + d } } } } };
		this.assertEquals(f.value.value.value.value.value, 10);
	}

	test_deep_nesting_assigns_the_outermost {
		var x = 0;
		var f = { { { { x = x + 1 } } } };
		f.value.value.value.value;
		f.value.value.value.value;
		this.assertEquals(x, 2);
	}

	test_closure_called_after_the_loop_ended {
		var f;
		3.do { |i| if (i == 1) { var v; v = \middle; f = { v } } };
		this.assertEquals(f.value, \middle);
	}

	test_capture_and_no_capture_mixed {
		// One variable is captured and one is not, in the same body.
		var fs = List.new, plain = List.new;
		3.do { |i|
			var captured, other;
			captured = i;
			other = i * 2;
			plain.add(other);
			fs.add { captured };
		};
		this.assertEquals(fs.collect { |f| f.value }.asArray, [0, 1, 2]);
		this.assertEquals(plain.asArray, [0, 2, 4]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// A hoisted variable survives yields, recursion and re-entrant calls
	////////////////////////////////////////////////////////////////////////////////

	test_variable_survives_a_recursive_call {
		this.assertEquals(this.recursiveMine(3), 30);
		this.assertEquals(this.recursiveMine(0), 0);
	}

	test_variable_survives_recursion_in_a_loop {
		var log = List.new;
		this.recursiveDo(2, log);
		this.assertEquals(log.asArray, [20, 10, 10, 11, 11, 20, 21, 10, 10, 11, 11, 21]);
	}

	test_variable_survives_reentrant_function {
		var f;
		f = { |n| var v; v = n; if (n > 0) { f.(n - 1) }; v };
		this.assertEquals(f.(4), 4, "the outer call's variable is not overwritten by the inner calls");
	}

	test_variable_survives_yield_in_do {
		var r = Routine { 3.do { |i| var v; v = i * 2; v.yield; v = 99 } };
		this.assertEquals([r.next, r.next, r.next, r.next], [0, 2, 4, nil]);
	}

	test_variable_survives_yield_in_loop {
		var r = Routine { var n; n = 0; loop { var v; v = n; n = n + 1; v.yield } };
		this.assertEquals([r.next, r.next, r.next], [0, 1, 2]);
	}

	test_variable_survives_yield_in_while {
		var r = Routine { var i; i = 0; while { i < 3 } { var v; v = i * 3; i = i + 1; v.yield } };
		this.assertEquals([r.next, r.next, r.next, r.next], [0, 3, 6, nil]);
	}

	test_body_var_is_still_fresh_after_a_yield {
		var seen = List.new;
		var r = Routine { 3.do { |i| var v; seen.add(v); v = i; i.yield } };
		4.do { r.next };
		this.assertEquals(seen.asArray, [nil, nil, nil]);
	}

	test_routines_from_one_function_do_not_share_variables {
		var make = { Routine { var c; c = 0; loop { c = c + 1; c.yield } } };
		var r1 = make.value, r2 = make.value;
		this.assertEquals([r1.next, r1.next, r2.next, r1.next, r2.next], [1, 2, 1, 3, 2]);
	}

	test_closure_made_before_a_yield_sees_changes_after {
		var r, f;
		r = Routine { var x; x = 1; f = { x }; 0.yield; x = 2; 0.yield };
		r.next;
		this.assertEquals(f.value, 1);
		r.next;
		this.assertEquals(f.value, 2);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Arguments
	////////////////////////////////////////////////////////////////////////////////

	test_missing_arguments_are_nil {
		this.assertEquals({ |a, b| [a, b] }.(1), [1, nil]);
		this.assertEquals({ |a, b| [a, b] }.(), [nil, nil]);
	}

	test_extra_arguments_are_ignored {
		this.assertEquals({ |a| a }.(1, 2, 3), 1);
		this.assertEquals({ 5 }.(1, 2, 3), 5);
	}

	test_default_arguments {
		var f = { |a, b = 2, c = 3| [a, b, c] };
		this.assertEquals(f.(1), [1, 2, 3]);
		this.assertEquals(f.(1, 5), [1, 5, 3]);
		this.assertEquals(f.(1, 5, 7), [1, 5, 7]);
		this.assertEquals(f.(), [nil, 2, 3]);
	}

	test_default_argument_kinds {
		var f = { |a = \sym, b = "str", c = 2.5, d = -3, e = nil, g = true| [a, b, c, d, e, g] };
		this.assertEquals(f.value, [\sym, "str", 2.5, -3, nil, true]);
	}

	test_literal_default_argument_is_one_object {
		// A literal array is built once, so every call that uses the default gets the same object.
		var f = { |a = #[1, 2]| a };
		this.assert(f.value === f.value, "a literal default is a single literal");
	}

	test_variable_arguments {
		this.assertEquals({ |a ...rest| [a, rest] }.(1, 2, 3), [1, [2, 3]]);
		this.assertEquals({ |a ...rest| [a, rest] }.(1), [1, []]);
		this.assertEquals({ |...all| all }.(), []);
		this.assertEquals({ |...all| all }.(1, 2, 3), [1, 2, 3]);
		this.assertEquals({ |a, b = 5 ...rest| [a, b, rest] }.(1, 2, 3, 4), [1, 2, [3, 4]]);
	}

	test_variable_arguments_are_captured {
		var f = { |...all| { all.size } };
		this.assertEquals(f.(1, 2, 3).value, 3);
	}

	test_argument_assignment {
		this.assertEquals({ |a| a = a + 1; a }.(1), 2);
		this.assertEquals({ |a, b| a = b; b = 0; [a, b] }.(1, 2), [2, 0]);
	}

	test_many_arguments {
		var f = { |a, b, c, d, e, f, g, h, i, j, k, l| [a, b, c, d, e, f, g, h, i, j, k, l] };
		this.assertEquals(f.(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12), (1..12));
		this.assertEquals(f.(1, 2, 3), [1, 2, 3, nil, nil, nil, nil, nil, nil, nil, nil, nil]);
	}

	test_argument_named_like_the_function_value_method {
		this.assertEquals({ |value| value }.(5), 5);
		this.assertEquals({ |this_| this_ }.(6), 6);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Assignment
	////////////////////////////////////////////////////////////////////////////////

	test_assignment_is_an_expression {
		var a, b, c;
		this.assertEquals(a = 5, 5);
		this.assertEquals((a = 5) + 1, 6);
		this.assertEquals(a = b = c = 7, 7);
		this.assertEquals([a, b, c], [7, 7, 7]);
		this.assertEquals(max(a = 3, 2), 3);
		this.assertEquals(a, 3);
	}

	test_assignment_order {
		var a = 1, b;
		b = (a = 2) + a;
		this.assertEquals(b, 4, "operands are evaluated left to right");
		a = 1;
		b = a + (a = 2);
		this.assertEquals(b, 3);
	}

	test_assignment_to_var_in_condition {
		var x;
		if ((x = 5) > 3) { x = x + 1 };
		this.assertEquals(x, 6);
		while { (x = x - 1) > 2 } { };
		this.assertEquals(x, 2);
	}

	test_assignment_in_inlined_bodies {
		var x = 0;
		if (true) { x = 1 };
		this.assertEquals(x, 1);
		3.do { x = x + 1 };
		this.assertEquals(x, 4);
		switch (1, 1, { x = \sw });
		this.assertEquals(x, \sw);
		case { true } { x = \cs };
		this.assertEquals(x, \cs);
		x = nil;
		x = x ?? { \filled };
		this.assertEquals(x, \filled);
	}

	test_swap_through_a_temp_in_a_loop {
		var a = 1, b = 2;
		3.do { var t; t = a; a = b; b = t };
		this.assertEquals([a, b], [2, 1]);
	}

	test_accumulate_in_nested_loops {
		var total = 0;
		4.do { |i| 3.do { |j| total = total + (i * j) } };
		this.assertEquals(total, 18);
	}

	test_unused_variable_and_unused_argument {
		var unused, alsoUnused = 3;
		this.assertEquals({ |a, b, c| 4 }.(1, 2, 3), 4);
		3.do { |i, j| var k; 5 };
		this.assertEquals(unused, nil);
	}

	test_shadows {
		var t = 10;
		if (true) {
			var t = t;
			this.assertEquals(t, 10, "Should shadow");
			t = 20;
			this.assertEquals(t, 20, "Should shadow");
		};
		this.assertEquals(t, 10, "Should shadow");
	}
}
