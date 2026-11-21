//# VERSION 3.16
// Control flow the compiler turns into jumps: if, ifNil, ifNotNil, and, or, ??, !?, while, loop, case and switch.
// Each construct is checked three ways, because those are the ways an inliner goes wrong:
//   1. the value of the whole expression,
//   2. which branches ran, and how many times (laziness and evaluating the condition once),
//   3. the same construct when its arguments are not literal functions, so it is a normal send.

TestCompilerInlining : UnitTest {
	var log;

	setUp { log = List.new }

	// Records that a branch ran, and returns what it was given.
	mark { |tag, value| log.add(tag); ^value }

	////////////////////////////////////////////////////////////////////////////////
	// if
	////////////////////////////////////////////////////////////////////////////////

	test_if_values {
		this.assertEquals(if (true) { 1 } { 2 }, 1);
		this.assertEquals(if (false) { 1 } { 2 }, 2);
		this.assertEquals(if (true) { 1 }, 1);
		this.assertEquals(if (false) { 1 }, nil, "if without else is nil when false");
		this.assertEquals(if (true, { 1 }, { 2 }), 1, "parenthesised call");
		this.assertEquals(true.if({ 1 }, { 2 }), 1, "method call syntax");
		this.assertEquals(false.if({ 1 }, { 2 }), 2);
		this.assertEquals(if (false, { 1 }), nil);
	}

	test_if_empty_branches {
		this.assertEquals(if (true) { } { 2 }, nil, "empty true branch is nil");
		this.assertEquals(if (false) { 1 } { }, nil, "empty false branch is nil");
		this.assertEquals(if (true) { }, nil);
	}

	test_if_runs_only_the_taken_branch {
		if (true) { this.mark(\yes, 1) } { this.mark(\no, 2) };
		this.assertEquals(log.asArray, [\yes]);
		log.clear;
		if (false) { this.mark(\yes, 1) } { this.mark(\no, 2) };
		this.assertEquals(log.asArray, [\no]);
		log.clear;
		if (false) { this.mark(\yes, 1) };
		this.assertEquals(log.asArray, []);
	}

	test_if_condition_evaluated_once {
		var n = 0;
		if ({ n = n + 1; true }.value) { 1 } { 2 };
		this.assertEquals(n, 1);
		if (this.mark(\cond, true)) { 1 } { 2 };
		this.assertEquals(log.asArray, [\cond]);
	}

	test_if_condition_can_be_any_expression {
		var a = 3, b = 4;
		this.assertEquals(if (a < b) { \less } { \more }, \less);
		this.assertEquals(if (a > b) { \less } { \more }, \more);
		this.assertEquals(if (a < b and: { b < 10 }) { 1 } { 2 }, 1);
		this.assertEquals(if ((a == 3).not) { 1 } { 2 }, 2);
		this.assertEquals(if ([a, b].includes(4)) { 1 } { 2 }, 1);
	}

	test_if_non_function_branches {
		var a = 7;
		this.assertEquals(if (true, 1, 2), 1, "literal branches");
		this.assertEquals(if (false, 1, 2), 2);
		this.assertEquals(if (true, a, 2), 7, "variable branch");
		this.assertEquals(if (false, 1, a), 7);
		this.assertEquals(if (true, \sym, \other), \sym);
		this.assertEquals(if (false, 1), nil);
		this.assertEquals(if (true, [1, 2], 0), [1, 2]);
	}

	test_if_function_held_in_variable {
		var yes = { this.mark(\yes, 1) }, no = { this.mark(\no, 2) };
		this.assertEquals(if (true, yes, no), 1, "a function in a variable is called");
		this.assertEquals(log.asArray, [\yes]);
		this.assertEquals(if (false, yes, no), 2);
		this.assertEquals(log.asArray, [\yes, \no]);
	}

	test_if_mixed_branches {
		var f = { 10 };
		this.assertEquals(if (true, { 1 }, 2), 1);
		this.assertEquals(if (false, { 1 }, 2), 2);
		this.assertEquals(if (true, 1, { 2 }), 1);
		this.assertEquals(if (false, 1, { 2 }), 2);
		this.assertEquals(if (false, { 1 }, f), 10);
	}

	test_if_branch_with_arguments_is_not_inlined {
		// A function that takes arguments is called with none of them filled in.
		this.assertEquals(if (true) { |x| x }, nil);
		this.assertEquals(if (true) { |x = 5| x }, 5);
		this.assertEquals(if (false) { 1 } { |x = 6| x + 1 }, 7);
	}

	test_if_branch_with_variables {
		this.assertEquals(if (true) { var x = 4; x * 2 } { 0 }, 8);
		this.assertEquals(if (false) { 0 } { var x = 5, y = 6; x + y }, 11);
		// The variable of a branch that did not run is not visible afterwards, the next run starts fresh.
		2.do {
			var seen = if (true) { var v; v = v ? 0; v = v + 1; v } { 0 };
			this.assertEquals(seen, 1, "a branch variable starts as nil every time");
		};
	}

	test_if_nested {
		var f = { |a, b|
			if (a) { if (b) { \both } { \first } } { if (b) { \second } { \neither } }
		};
		this.assertEquals(f.(true, true), \both);
		this.assertEquals(f.(true, false), \first);
		this.assertEquals(f.(false, true), \second);
		this.assertEquals(f.(false, false), \neither);
	}

	test_if_as_argument_and_operand {
		var a = 5;
		this.assertEquals(1 + if (a > 3) { 10 } { 20 }, 11);
		this.assertEquals([if (true) { 1 } { 2 }, if (false) { 1 } { 2 }], [1, 2]);
		this.assertEquals((if (a > 3) { 10 } { 20 }) + 1, 11);
		this.assertEquals(max(if (true) { 3 } { 4 }, if (false) { 7 } { 6 }), 6);
	}

	test_if_non_boolean_condition {
		this.assertException({ if (nil) { 1 } { 2 } }, Error, "nil is not a Boolean");
		this.assertException({ if (1) { 1 } { 2 } }, Error, "an Integer is not a Boolean");
		this.assertException({ if ("x") { 1 } }, Error, "a String is not a Boolean");
		this.assertException({ var x; if (x) { 1 } { 2 } }, Error, "an unset variable is not a Boolean");
	}

	test_if_user_defined_boolean {
		// With literal functions the send is a jump, whatever the receiver is, so `if` is never looked up and a
		// receiver that is not a Boolean is an error. This matches the old compiler.
		var o = TestCompilerInliningBoolLike();
		var f = { 1 };
		this.assertException({ o.if({ 1 }, { 2 }) }, Error, "literal functions are inlined, the receiver is tested");
		this.assertEquals(o.if(f, f), \custom, "functions in variables are a normal send, the receiver is asked");
		this.assertEquals(o.if(1, 2), \custom, "plain values are a normal send");
	}

	////////////////////////////////////////////////////////////////////////////////
	// ifNil, ifNotNil, ??, !?
	////////////////////////////////////////////////////////////////////////////////

	test_isNil_test {
		var x;
		this.assertEquals(if (x.isNil) { 1 } { 2 }, 1);
		x = 5;
		this.assertEquals(if (x.isNil) { 1 } { 2 }, 2);
		this.assertEquals(if (x.notNil) { 1 } { 2 }, 1);
		x = nil;
		this.assertEquals(if (x.notNil) { 1 } { 2 }, 2);
		this.assertEquals(if (x.isNil) { 1 }, 1);
		this.assertEquals(if (x.notNil) { 1 }, nil);
	}

	test_isNil_test_evaluates_receiver_once {
		if (this.mark(\once, nil).isNil) { 1 } { 2 };
		this.assertEquals(log.asArray, [\once]);
		log.clear;
		if (this.mark(\once, 3).notNil) { 1 } { 2 };
		this.assertEquals(log.asArray, [\once]);
	}

	test_isNil_test_on_false_and_zero {
		// Only nil is nil.
		this.assertEquals(if (false.isNil) { 1 } { 2 }, 2);
		this.assertEquals(if (0.isNil) { 1 } { 2 }, 2);
		this.assertEquals(if ("".isNil) { 1 } { 2 }, 2);
		this.assertEquals(if ([].notNil) { 1 } { 2 }, 1);
	}

	test_isNil_overridden {
		// `if (x.isNil)` tests for nil itself and never sends isNil, so an override is not asked.
		// This matches the old compiler. A plain send of isNil does ask.
		var o = TestCompilerInliningNilLike();
		this.assertEquals(if (o.isNil) { 1 } { 2 }, 2, "the test is not a send, the override is ignored");
		this.assertEquals(if (o.notNil) { 1 } { 2 }, 1);
		this.assertEquals(o.isNil, true, "sent as a message the override answers");
		this.assertEquals(o.notNil, false);
	}

	test_nil_coalescing {
		var x;
		this.assertEquals(x ?? { 5 }, 5);
		x = 1;
		this.assertEquals(x ?? { 5 }, 1);
		x = false;
		this.assertEquals(x ?? { 5 }, false, "false is not nil");
		x = 0;
		this.assertEquals(x ?? { 5 }, 0, "zero is not nil");
		this.assertEquals(nil ?? { nil }, nil);
		this.assertEquals(nil ?? { nil } ?? { 3 }, 3, "chained");
	}

	test_nil_coalescing_is_lazy {
		var r = 4 ?? { this.mark(\rhs, 0) };
		this.assertEquals(log.asArray, []);
		this.assertEquals(r, 4);
		nil ?? { this.mark(\rhs, 0) };
		this.assertEquals(log.asArray, [\rhs]);
		this.assertEquals((this.mark(\lhs, 2) ?? { this.mark(\rhs, 3) }), 2);
		this.assertEquals(log.asArray, [\rhs, \lhs], "the left side runs once, the right not at all");
	}

	test_nil_coalescing_non_function_right {
		var d = 6;
		this.assertEquals(nil ?? d, 6, "not a function: a plain send");
		this.assertEquals(2 ?? d, 2);
	}

	test_nil_conditional_call {
		var x;
		this.assertEquals(x !? { |v| v + 1 }, nil);
		x = 2;
		this.assertEquals(x !? { |v| v + 1 }, 3);
		x = false;
		this.assertEquals(x !? { |v| v }, false, "false is not nil");
		this.assertEquals(x !? { 9 }, 9);
	}

	test_nil_conditional_call_is_lazy {
		nil !? { this.mark(\rhs, 0) };
		this.assertEquals(log.asArray, []);
		2 !? { this.mark(\rhs, 0) };
		this.assertEquals(log.asArray, [\rhs]);
	}

	test_question_mark_operator {
		// `?` is a value, not a function: it evaluates both sides.
		var x;
		this.assertEquals(x ? 5, 5);
		x = 3;
		this.assertEquals(x ? 5, 3);
		this.assertEquals(nil ? nil ? 7, 7);
		this.assertEquals(this.mark(\l, nil) ? this.mark(\r, 8), 8);
		this.assertEquals(log.asArray, [\l, \r]);
		log.clear;
		this.assertEquals(this.mark(\l, 1) ? this.mark(\r, 8), 1);
		this.assertEquals(log.asArray, [\l, \r], "the right side is evaluated even when it is not needed");
	}

	////////////////////////////////////////////////////////////////////////////////
	// and, or
	////////////////////////////////////////////////////////////////////////////////

	test_and_or_values {
		this.assertEquals(true and: { true }, true);
		this.assertEquals(true and: { false }, false);
		this.assertEquals(false and: { true }, false);
		this.assertEquals(false or: { true }, true);
		this.assertEquals(true or: { false }, true);
		this.assertEquals(false or: { false }, false);
		this.assertEquals(true.and({ 3 }), 3, "and answers the right side, not a Boolean");
		this.assertEquals(false.or({ 4 }), 4, "or answers the right side, not a Boolean");
		this.assertEquals(true and: { nil }, nil);
	}

	test_and_or_short_circuit {
		false and: { this.mark(\and, 0) };
		true or: { this.mark(\or, 0) };
		this.assertEquals(log.asArray, [], "the right side is skipped");
		true and: { this.mark(\and, 0) };
		false or: { this.mark(\or, 0) };
		this.assertEquals(log.asArray, [\and, \or]);
	}

	test_and_or_left_side_once {
		(this.mark(\l, true) and: { true });
		(this.mark(\l, false) or: { true });
		this.assertEquals(log.asArray, [\l, \l]);
	}

	test_and_or_chains {
		this.assertEquals(true and: { true } and: { false }, false);
		this.assertEquals(true and: { true } and: { 5 }, 5);
		this.assertEquals(false or: { false } or: { 6 }, 6);
		this.assertEquals((true and: { false }) or: { true }, true);
		this.assertEquals(false and: { this.mark(\a, true) } and: { this.mark(\b, true) }, false);
		this.assertEquals(log.asArray, []);
	}

	test_and_or_extra_arguments_are_ignored {
		// and and or take two arguments. A third is accepted and never looked at, as in the old compiler.
		this.assertEquals(and(true, { true }, { 3 }), true);
		this.assertEquals(and(true, { false }, { this.mark(\skipped, 0) }), false);
		this.assertEquals(or(false, { false }, { 4 }), false);
		this.assertEquals(or(false, { 5 }, { this.mark(\skipped, 0) }), 5);
		this.assertEquals(log.asArray, [], "the extra argument never runs");
	}

	test_and_or_non_function_right {
		var d = 3;
		this.assertEquals(true and: d, 3, "a value, not a function");
		this.assertEquals(false or: d, 3);
		this.assertEquals(false and: d, false);
		this.assertEquals(true or: d, true);
		this.assertEquals(true and: this.mark(\eager, 1), 1);
		this.assertEquals(log.asArray, [\eager], "an argument that is not a function is evaluated before the send");
	}

	test_and_or_non_boolean_receiver {
		this.assertException({ nil and: { true } }, Error);
		this.assertException({ 1 or: { true } }, Error);
	}

	test_and_or_in_conditions {
		var a = 1, b = 2;
		this.assertEquals(if (a < b and: { b < 3 }) { \in } { \out }, \in);
		this.assertEquals(if (a > b or: { b == 2 }) { \in } { \out }, \in);
		this.assertEquals(if (a > b and: { this.mark(\skip, true) }) { \in } { \out }, \out);
		this.assertEquals(log.asArray, []);
	}

	test_and_or_with_nil_check {
		var x;
		this.assertEquals(x.notNil and: { x > 3 }, false, "the guard stops the nil comparison");
		x = 5;
		this.assertEquals(x.notNil and: { x > 3 }, true);
		x = nil;
		this.assertEquals(x.isNil or: { x > 3 }, true);
	}

	////////////////////////////////////////////////////////////////////////////////
	// while
	////////////////////////////////////////////////////////////////////////////////

	test_while_values {
		var i = 0;
		var r = while { i < 3 } { i = i + 1 };
		this.assertEquals(i, 3);
		this.assertEquals(r, nil, "while is always nil");
	}

	test_while_zero_times {
		var i = 10, ran = false;
		while { i < 3 } { ran = true };
		this.assertEquals(ran, false);
		this.assertEquals(while { false } { 1 }, nil);
	}

	test_while_condition_runs_one_more_time_than_body {
		var conds = 0, bodies = 0;
		while { conds = conds + 1; conds <= 4 } { bodies = bodies + 1 };
		this.assertEquals(conds, 5);
		this.assertEquals(bodies, 4);
	}

	test_while_without_body {
		var i = 0;
		while { i = i + 1; i < 5 };
		this.assertEquals(i, 5, "the condition alone may do the work");
		i = 0;
		{ i = i + 1; i < 7 }.while;
		this.assertEquals(i, 7, "method call syntax without a body");
	}

	test_while_method_call_syntax {
		var i = 0;
		{ i < 3 }.while { i = i + 1 };
		this.assertEquals(i, 3);
		i = 0;
		{ i < 3 }.while({ i = i + 1 });
		this.assertEquals(i, 3);
		i = 0;
		while({ i < 3 }, { i = i + 1 });
		this.assertEquals(i, 3);
	}

	test_while_condition_with_several_statements {
		var i = 0, log2 = List.new;
		while { var next = i + 1; log2.add(next); next < 4 } { i = i + 1 };
		this.assertEquals(log2.asArray, [1, 2, 3, 4]);
	}

	test_while_true_leaves_by_return {
		this.assertEquals(this.whileTrueUntil(5), 5);
		this.assertEquals(this.whileTrueUntil(1), 1);
	}

	whileTrueUntil { |limit|
		var i = 0;
		while { true } { i = i + 1; if (i >= limit) { ^i } };
		^\unreachable
	}

	test_while_nested {
		var out = List.new;
		var i = 0;
		while { i < 3 } {
			var j = 0;
			while { j < 2 } { out.add([i, j]); j = j + 1 };
			i = i + 1;
		};
		this.assertEquals(out.asArray, [[0, 0], [0, 1], [1, 0], [1, 1], [2, 0], [2, 1]]);
	}

	test_while_body_variables_start_fresh {
		var i = 0, seen = List.new;
		while { i < 3 } {
			var v;
			seen.add(v);
			v = i;
			i = i + 1;
		};
		this.assertEquals(seen.asArray, [nil, nil, nil], "a variable declared in the body is nil on every pass");
	}

	test_while_non_function_arguments {
		// When the condition is not a literal function the send is not inlined, and answers its receiver.
		var f = { false };
		this.assertEquals(f.while({ 1 }), f, "a function in a variable as the condition");
		this.assertEquals(while(f, { 1 }), f);
	}

	test_while_body_in_variable {
		var i = 0, step = { i = i + 1 };
		while({ i < 4 }, step);
		this.assertEquals(i, 4);
	}

	test_while_non_boolean_condition {
		this.assertException({ while { nil } { 1 } }, Error);
		this.assertException({ while { 3 } { 1 } }, Error);
	}

	////////////////////////////////////////////////////////////////////////////////
	// loop
	////////////////////////////////////////////////////////////////////////////////

	test_loop_stops_by_return {
		var n = 0;
		this.loopUntilReturn(5, { n = n + 1 });
		this.assertEquals(n, 5);
		this.assertEquals(this.loopUntilReturn(3, { }), \done);
	}

	loopUntilReturn { |limit, body|
		var count = 0;
		loop {
			body.value;
			count = count + 1;
			if (count >= limit) { ^\done };
		};
		^\unreachable
	}

	test_loop_stops_by_exception {
		var n = 0;
		try { loop { n = n + 1; if (n == 4) { Error("stop").throw } } };
		this.assertEquals(n, 4);
	}

	test_loop_method_syntax {
		var n = 0;
		try { { n = n + 1; if (n == 3) { Error("stop").throw } }.loop };
		this.assertEquals(n, 3);
	}

	test_loop_function_in_variable {
		var n = 0, f = { n = n + 1; if (n == 3) { Error("stop").throw } };
		try { f.loop };
		this.assertEquals(n, 3);
	}

	test_loop_body_variables_start_fresh {
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

	////////////////////////////////////////////////////////////////////////////////
	// case
	////////////////////////////////////////////////////////////////////////////////

	test_case_values {
		var x = 2;
		this.assertEquals(case { x == 1 } { \one } { x == 2 } { \two } { x == 3 } { \three }, \two);
		this.assertEquals(case { x == 1 } { \one } { true } { \default }, \default);
		this.assertEquals(case { x == 1 } { \one } { x == 5 } { \five }, nil, "no match and no default is nil");
	}

	test_case_first_match_wins {
		var x = 2;
		this.assertEquals(case { x > 0 } { \first } { x > 1 } { \second } { true } { \third }, \first);
	}

	test_case_stops_evaluating_conditions {
		case { this.mark(\c1, false) } { 1 } { this.mark(\c2, true) } { 2 } { this.mark(\c3, true) } { 3 };
		this.assertEquals(log.asArray, [\c1, \c2], "conditions after the match are not evaluated");
	}

	test_case_runs_only_the_matching_branch {
		case { false } { this.mark(\a, 0) } { true } { this.mark(\b, 0) } { true } { this.mark(\c, 0) };
		this.assertEquals(log.asArray, [\b]);
	}

	test_case_single_clause {
		this.assertEquals(case { true } { 5 }, 5);
		this.assertEquals(case { false } { 5 }, nil);
	}

	test_case_trailing_default_is_a_function_after_pairs {
		// An odd final argument is a default.
		var x = 9;
		this.assertEquals(case { x == 1 } { \one } { \otherwise }, \otherwise);
		this.assertEquals(case { x == 9 } { \nine } { \otherwise }, \nine);
	}

	test_case_function_call_syntax {
		var x = 3;
		this.assertEquals(case({ x == 3 }, { \yes }, { \no }), \yes);
		this.assertEquals(case({ x == 4 }, { \yes }, { \no }), \no);
	}

	test_case_nested {
		var f = { |a, b|
			case
			{ a } { case { b } { \ab } { true } { \a } }
			{ true } { case { b } { \b } { true } { \none } }
		};
		this.assertEquals(f.(true, true), \ab);
		this.assertEquals(f.(true, false), \a);
		this.assertEquals(f.(false, true), \b);
		this.assertEquals(f.(false, false), \none);
	}

	test_case_nil_test_conditions {
		var x;
		this.assertEquals(case { x.isNil } { \nil } { x.notNil } { \notNil }, \nil);
		x = 1;
		this.assertEquals(case { x.isNil } { \nil } { x.notNil } { \notNil }, \notNil);
	}

	test_case_non_function_arguments {
		var cond = { true }, branch = { 5 };
		this.assertEquals(case(cond, branch), 5, "functions in variables are a normal send");
		this.assertEquals(case(cond, 6), 6, "a value as the result");
	}

	test_case_condition_with_variables {
		this.assertEquals(case { var y = 2; y == 2 } { \two } { true } { \other }, \two);
	}

	test_case_non_boolean_condition {
		this.assertException({ case { nil } { 1 } }, Error);
	}

	////////////////////////////////////////////////////////////////////////////////
	// switch
	////////////////////////////////////////////////////////////////////////////////

	test_switch_values {
		this.assertEquals(switch (2, 1, { \one }, 2, { \two }, 3, { \three }), \two);
		this.assertEquals(switch (1, 1, { \one }, 2, { \two }), \one);
		this.assertEquals(switch (3, 1, { \one }, 2, { \two }, { \default }), \default);
		this.assertEquals(switch (3, 1, { \one }, 2, { \two }), nil, "no match and no default is nil");
	}

	test_switch_key_types {
		this.assertEquals(switch (\b, \a, { 1 }, \b, { 2 }), 2, "symbols");
		this.assertEquals(switch ($b, $a, { 1 }, $b, { 2 }), 2, "characters");
		this.assertEquals(switch (2.5, 1.5, { 1 }, 2.5, { 2 }), 2, "floats");
		this.assertEquals(switch (true, false, { 1 }, true, { 2 }), 2, "booleans");
		this.assertEquals(switch (-1, -1, { \neg }, 0, { \zero }), \neg, "negative integers");
		this.assertEquals(switch ("b", "a", { 1 }, "b", { 2 }, { 3 }), 2, "strings match by ==, not identity");
	}

	test_switch_float_and_integer_keys_do_not_match {
		// A quirk of how the keys are looked up, kept from the old compiler: 2.0 == 2, but they are not the same key.
		this.assertEquals(switch (2.0, 2, { \int }, 3, { \three }), nil);
		this.assertEquals(switch (2, 2.0, { \float }, 3, { \three }), nil);
		this.assertEquals(switch (2.0, 2, { \int }, { \default }), \default);
		this.assertEquals(switch (2.0, 2.0, { \float }, 3, { \three }), \float);
	}

	test_switch_first_match_wins {
		this.assertEquals(switch (1, 1, { \first }, 1, { \second }), \first);
	}

	test_switch_runs_only_the_matching_branch {
		switch (2, 1, { this.mark(\one, 0) }, 2, { this.mark(\two, 0) }, 3, { this.mark(\three, 0) });
		this.assertEquals(log.asArray, [\two]);
	}

	test_switch_value_evaluated_once {
		switch (this.mark(\value, 2), 1, { 1 }, 2, { 2 }, 3, { 3 }, { 4 });
		this.assertEquals(log.asArray, [\value]);
	}

	test_switch_default_runs_only_when_nothing_matches {
		switch (1, 1, { this.mark(\one, 0) }, { this.mark(\default, 0) });
		this.assertEquals(log.asArray, [\one]);
		log.clear;
		switch (5, 1, { this.mark(\one, 0) }, { this.mark(\default, 0) });
		this.assertEquals(log.asArray, [\default]);
	}

	test_switch_method_syntax {
		this.assertEquals(2.switch(1, { \one }, 2, { \two }), \two);
	}

	test_switch_key_may_be_an_expression {
		var k = 3;
		this.assertEquals(switch (3, 1, { \one }, k, { \k }, 4, { \four }), \k, "a variable as a key");
		this.assertEquals(switch (4, 1, { \one }, k + 1, { \computed }), \computed, "a computed key");
	}

	test_switch_key_is_a_function {
		// A key written as a function is called, `{ 1 } == 1` would be false if it were compared as it is.
		this.assertEquals(switch (1, { 1 }, { \matched }, { 2 }, { \other }), \matched);
		this.assertEquals(switch (2, { 1 }, { \matched }, { 2 }, { \other }), \other);
	}

	test_switch_key_function_is_lazy {
		// The first matching key stops the search, later key functions are not run.
		switch (1, { this.mark(\k1, 1) }, { 1 }, { this.mark(\k2, 2) }, { 2 });
		this.assertEquals(log.asArray, [\k1]);
	}

	test_switch_nil_key {
		// nil marks an empty slot of the table, it must still behave as an ordinary key.
		this.assertEquals(switch (nil, nil, { \nil }, 1, { \one }, { \default }), \nil);
		this.assertEquals(switch (1, nil, { \nil }, 1, { \one }, { \default }), \one);
		this.assertEquals(switch (2, nil, { \nil }, 1, { \one }, { \default }), \default);
		this.assertEquals(switch (nil, 1, { \one }, { \default }), \default);
	}

	test_switch_sparse_and_large_keys {
		this.assertEquals(switch (1000, 1, { \a }, 1000, { \b }, 1000000, { \c }), \b);
		this.assertEquals(switch (1000000, 1, { \a }, 1000, { \b }, 1000000, { \c }), \c);
		this.assertEquals(switch (999, 1, { \a }, 1000, { \b }, { \d }), \d);
	}

	test_switch_many_cases {
		var r = (0..40).collect { |i|
			switch (i,
				0, { 0 }, 1, { 1 }, 2, { 2 }, 3, { 3 }, 4, { 4 }, 5, { 5 }, 6, { 6 }, 7, { 7 }, 8, { 8 }, 9, { 9 },
				10, { 10 }, 11, { 11 }, 12, { 12 }, 13, { 13 }, 14, { 14 }, 15, { 15 }, 16, { 16 }, 17, { 17 },
				18, { 18 }, 19, { 19 }, 20, { 20 }, 21, { 21 }, 22, { 22 }, 23, { 23 }, 24, { 24 }, 25, { 25 },
				26, { 26 }, 27, { 27 }, 28, { 28 }, 29, { 29 }, 30, { 30 },
				{ \none }
			)
		};
		this.assertEquals(r, (0..30) ++ Array.fill(10, \none));
	}

	test_switch_no_cases {
		this.assertEquals(switch(1), nil);
		this.assertEquals(switch(1, { \default }), \default);
		this.assertEquals(switch(1, \default), \default);
	}

	test_switch_with_non_function_branches {
		var f = { \from_variable };
		this.assertEquals(switch (1, 1, f, 2, { \two }), \from_variable, "a function in a variable is a normal send");
		this.assertEquals(switch (1, 1, \value, 2, { \two }), \value, "a plain value as a branch");
	}

	test_switch_branch_with_arguments {
		this.assertEquals(switch (1, 1, { |x = 4| x + 1 }), 5);
	}

	test_switch_nested {
		var f = { |a, b|
			switch (a,
				1, { switch (b, 1, { \ab }, 2, { \ac }) },
				2, { switch (b, 1, { \ba }, { \bx }) },
				{ \xx }
			)
		};
		this.assertEquals(f.(1, 1), \ab);
		this.assertEquals(f.(1, 2), \ac);
		this.assertEquals(f.(1, 3), nil);
		this.assertEquals(f.(2, 1), \ba);
		this.assertEquals(f.(2, 9), \bx);
		this.assertEquals(f.(7, 1), \xx);
	}

	test_switch_branch_variables {
		this.assertEquals(switch (1, 1, { var a = 3, b = 4; a * b }), 12);
		2.do { switch (1, 1, { var v; this.assertEquals(v, nil); v = 5 }) };
	}

	test_switch_incomparable_value {
		// Comparing an unrelated type does not throw, it just does not match.
		this.assertEquals(switch (\sym, 1, { 1 }, { \d }), \d);
		this.assertEquals(switch ([1], 1, { 1 }, { \d }), \d);
		this.assertEquals(switch (1, \sym, { 1 }, { \d }), \d);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Several constructs together
	////////////////////////////////////////////////////////////////////////////////

	test_mix_of_constructs {
		var f = { |x|
			if (x.isNil) { \nil } {
				switch (x,
					0, { \zero },
					1, { if (true) { \one } { \unreachable } },
					{ case { x < 0 } { \negative } { x > 100 } { \huge } { \other } }
				)
			}
		};
		this.assertEquals(f.(nil), \nil);
		this.assertEquals(f.(0), \zero);
		this.assertEquals(f.(1), \one);
		this.assertEquals(f.(-4), \negative);
		this.assertEquals(f.(500), \huge);
		this.assertEquals(f.(5), \other);
	}

	test_while_with_if_and_case_inside {
		var i = 0, out = List.new;
		while { i < 6 } {
			out.add(case { i == 0 } { \start } { i.even } { if (i == 4) { \four } { \even } } { \odd });
			i = i + 1;
		};
		this.assertEquals(out.asArray, [\start, \odd, \even, \odd, \four, \odd]);
	}

	test_condition_with_side_effects_in_every_construct {
		var n = 0;
		var bump = { n = n + 1; true };
		if (bump.value) { 1 };
		bump.value and: { 1 };
		bump.value or: { 1 };
		case { bump.value } { 1 };
		switch (bump.value, true, { 1 });
		this.assertEquals(n, 5, "each condition ran exactly once");
	}
}

TestCompilerInliningBoolLike {
	// Not a Boolean, but answers `if`, so a send to it must not be treated as a jump.
	if { |trueFunc, falseFunc| ^\custom }
}

TestCompilerInliningNilLike {
	isNil { ^true }
	notNil { ^false }
}
