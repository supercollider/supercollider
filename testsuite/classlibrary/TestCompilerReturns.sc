//# VERSION 3.16
// `^` returns from the method around it, however deeply it is nested. Inside an inlined body that is a jump, inside
// a function that is really a closure it unwinds the frames in between. These tests check, for each kind of nesting:
//   - the value that comes back, and that the code after the `^` did not run,
//   - that nothing is left on the stack, which matters when the `^` is in the middle of building another value,
//   - that cleanup code (protect) between the `^` and its method still runs.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerReturns : UnitTest {
	var log;

	setUp { log = List.new }

	mark { |tag| log.add(tag); ^tag }

	////////////////////////////////////////////////////////////////////////////////
	// Methods under test
	////////////////////////////////////////////////////////////////////////////////

	// Falls off the end: a method without ^ returns this, not its last value.
	noReturn { 1 + 1 }
	emptyMethod { }
	returnsNil { ^nil }
	returnsLast { ^1; this.mark(\unreachable) }

	ifReturn { |x| if (x) { ^1 }; ^2 }
	ifElseReturn { |x| if (x) { ^1 } { ^2 }; this.mark(\unreachable); ^3 }
	ifReturnsBranchValue { |x| ^if (x) { 1 } { 2 } }
	ifReturnInBothThenFallThrough { |x| if (x) { ^\a }; if (x.not) { ^\b }; ^\c }

	doReturn { |items, limit|
		items.do { |item| if (item > limit) { ^item } };
		^nil
	}

	nestedDoReturn { |target|
		3.do { |i| 3.do { |j| if (i * 3 + j == target) { ^[i, j] } } };
		^nil
	}

	nestedDoReturnWithVars { |target|
		3.do { |i|
			var row;
			row = i * 3;
			3.do { |j| var cell; cell = row + j; if (cell == target) { ^[i, j, cell] } }
		};
		^nil
	}

	whileReturn { |limit|
		var i = 0;
		while { i < 100 } { i = i + 1; if (i >= limit) { ^i } };
		^\none
	}

	whileConditionReturn { |limit|
		var i = 0;
		while { i = i + 1; if (i >= limit) { ^\fromCondition }; true } { };
		^\none
	}

	loopReturn { |limit|
		var i = 0;
		loop { i = i + 1; if (i >= limit) { ^i } };
	}

	forReturn {
		1.for(10) { |i| if (i == 4) { ^i } };
		^nil
	}

	forByReturn {
		1.forBy(20, 3) { |i| if (i > 8) { ^i } };
		^nil
	}

	reverseDoReturn {
		10.reverseDo { |i| if (i < 7) { ^i } };
		^nil
	}

	floatDoReturn {
		5.0.do { |i| if (i > 2) { ^i } };
		^nil
	}

	arrayDoReturn {
		[10, 20, 30].do { |x| if (x == 20) { ^x } };
		^nil
	}

	arrayReverseDoReturn {
		[10, 20, 30].reverseDo { |x| if (x == 20) { ^x } };
		^nil
	}

	dictionaryReturn {
		var d = Dictionary[\a -> 1];
		d.keysValuesDo { |k, v| ^[k, v] };
		^nil
	}

	switchReturn { |x|
		switch (x,
			1, { ^\one },
			2, { ^\two },
			{ ^\other }
		);
		this.mark(\unreachable);
	}

	switchFallThrough { |x|
		switch (x, 1, { \one }, 2, { ^\two });
		^\after
	}

	caseReturn { |x|
		case
		{ x < 0 } { ^\negative }
		{ x == 0 } { ^\zero };
		^\positive
	}

	andReturn { |a, b|
		(a and: { ^\shortcut });
		^b
	}

	orReturn { |a|
		(a or: { ^\shortcut });
		^\a_was_true
	}

	nilCoalescingReturn { |x|
		x ?? { ^\was_nil };
		^x
	}

	nilConditionalReturn { |x|
		x !? { ^\was_not_nil };
		^\was_nil
	}

	nonInlinedClosureReturn { |items|
		// collect is a real closure, the ^ unwinds through it.
		items.collect { |x| if (x == 2) { ^\found }; x };
		^\not_found
	}

	closureCalledInsideReturns {
		var f = { ^5 };
		f.value;
		^6
	}

	closureNeverCalled {
		var f = { ^5 };
		^6
	}

	passedClosureReturns {
		this.callIt({ ^7 });
		this.mark(\unreachable);
		^8
	}

	callIt { |f| f.value; this.mark(\unreachable_after_call); ^0 }

	passedClosureReturnsThroughProtect {
		this.callItProtected({ ^7 });
		^8
	}

	callItProtected { |f|
		protect { f.value } { this.mark(\cleanup) };
		^0
	}

	deepClosureReturn {
		var f = { { { { ^\deep } } } };
		f.value.value.value.value;
		^\not_deep
	}

	deepClosureReturnWithVars {
		var f = { var a; a = 1; { var b; b = 2; { var c; c = 3; { ^[a, b, c] } } } };
		f.value.value.value.value;
		^\not_deep
	}

	returnsDeadClosure { ^{ ^1 } }

	tryReturn {
		try { ^\from_try };
		^\after_try
	}

	tryWithErrorThenReturn {
		try { Error("x").throw } { ^\from_catch };
		^\after
	}

	protectReturn {
		protect { ^\from_protect } { this.mark(\cleanup) };
		^\after_protect
	}

	protectReturnNested {
		protect { protect { ^\inner } { this.mark(\inner_cleanup) } } { this.mark(\outer_cleanup) };
		^\after
	}

	protectError {
		protect { Error("e").throw } { this.mark(\cleanup) };
		^\after
	}

	protectReturnInCleanup {
		protect { 1 } { ^\from_cleanup };
		^\after
	}

	returnInArgument { |x|
		^this.take(1, 2, if (x) { ^\early } { 3 })
	}

	take { |a, b, c| ^[a, b, c] }

	returnInArrayLiteral { |x|
		^[1, 2, if (x) { ^\early } { 3 }]
	}

	returnInBinaryOperand { |x|
		^1 + if (x) { ^\early } { 2 }
	}

	returnInReceiver { |x|
		^(if (x) { ^\early } { [1, 2] }).size
	}

	returnInNestedCallArguments { |x|
		^this.take(this.take(1, 2, 3), 4, [5, 6, (if (x) { ^\early } { 7 })])
	}

	returnInSwitchKeyArguments { |x|
		^this.take(1, switch (x, 1, { ^\early }, { 2 }), 3)
	}

	returnInInlinedConditionOfNestedCall { |x|
		^this.take(1, 2, if (if (x) { ^\early } { true }) { \yes } { \no })
	}

	returnInLoopInsideArgument { |x|
		^this.take(1, 2, { 3.do { |i| if (x and: { i == 1 }) { ^\early } }; \done }.value)
	}

	returnInPrimitiveArgument { |x|
		^max(1, if (x) { ^\early } { 5 })
	}

	returnInSetterArgument { |x|
		log.add(if (x) { ^\early } { 5 });
		^log.size
	}

	returnValueIsEvaluatedFirst {
		^this.mark(\evaluated)
	}

	returnOfAssignment {
		var x;
		^x = 4
	}

	returnInVariableInitialiser { |x|
		var y = if (x) { ^\early } { 9 };
		^y
	}

	multipleReturnsInOneBody { |x|
		if (x == 1) { ^\one };
		if (x == 2) { ^\two };
		if (x == 3) { ^\three };
		^\many
	}

	returnsFromDeeplyNestedInlinedBranches { |a, b, c|
		if (a) { if (b) { if (c) { ^\abc } { ^\ab } } { ^\a } } { ^\none }
	}

	*classMethodReturn { |x| 3.do { |i| if (i == x) { ^i * 10 } }; ^nil }

	////////////////////////////////////////////////////////////////////////////////
	// Plain returns
	////////////////////////////////////////////////////////////////////////////////

	test_method_without_return_answers_this {
		this.assert(this.noReturn === this, "a method with no ^ answers this");
		this.assert(this.emptyMethod === this, "an empty method answers this");
	}

	test_return_nil {
		this.assertEquals(this.returnsNil, nil);
	}

	test_code_after_return_does_not_run {
		this.assertEquals(this.returnsLast, 1);
		this.assertEquals(log.asArray, []);
	}

	test_return_of_assignment {
		this.assertEquals(this.returnOfAssignment, 4);
	}

	test_return_evaluates_its_value_first {
		this.assertEquals(this.returnValueIsEvaluatedFirst, \evaluated);
		this.assertEquals(log.asArray, [\evaluated]);
	}

	test_class_method_return {
		this.assertEquals(TestCompilerReturns.classMethodReturn(1), 10);
		this.assertEquals(TestCompilerReturns.classMethodReturn(7), nil);
	}

	test_function_value_is_its_last_expression {
		this.assertEquals({ 1; 2; 3 }.value, 3);
		this.assertEquals({ }.value, nil);
		this.assertEquals({ var x; x = 5 }.value, 5);
	}

	////////////////////////////////////////////////////////////////////////////////
	// From inlined control flow
	////////////////////////////////////////////////////////////////////////////////

	test_return_from_if {
		this.assertEquals(this.ifReturn(true), 1);
		this.assertEquals(this.ifReturn(false), 2);
	}

	test_return_from_both_branches {
		this.assertEquals(this.ifElseReturn(true), 1);
		this.assertEquals(this.ifElseReturn(false), 2);
		this.assertEquals(log.asArray, []);
	}

	test_return_a_branch_value {
		this.assertEquals(this.ifReturnsBranchValue(true), 1);
		this.assertEquals(this.ifReturnsBranchValue(false), 2);
	}

	test_return_in_consecutive_ifs {
		this.assertEquals(this.ifReturnInBothThenFallThrough(true), \a);
		this.assertEquals(this.ifReturnInBothThenFallThrough(false), \b);
	}

	test_return_from_do {
		this.assertEquals(this.doReturn([1, 5, 9], 3), 5);
		this.assertEquals(this.doReturn([1, 2], 3), nil);
		this.assertEquals(this.doReturn([], 3), nil);
	}

	test_return_from_nested_do {
		this.assertEquals(this.nestedDoReturn(4), [1, 1]);
		this.assertEquals(this.nestedDoReturn(8), [2, 2]);
		this.assertEquals(this.nestedDoReturn(99), nil);
	}

	test_return_from_nested_do_with_variables {
		this.assertEquals(this.nestedDoReturnWithVars(5), [1, 2, 5]);
		this.assertEquals(this.nestedDoReturnWithVars(99), nil);
	}

	test_return_from_while_body {
		this.assertEquals(this.whileReturn(5), 5);
		this.assertEquals(this.whileReturn(1000), \none);
	}

	test_return_from_while_condition {
		this.assertEquals(this.whileConditionReturn(3), \fromCondition);
	}

	test_return_from_loop {
		this.assertEquals(this.loopReturn(7), 7);
	}

	test_return_from_hard_coded_loops {
		this.assertEquals(this.forReturn, 4);
		this.assertEquals(this.forByReturn, 10);
		this.assertEquals(this.reverseDoReturn, 6);
		this.assertEquals(this.floatDoReturn, 3.0);
		this.assertEquals(this.arrayDoReturn, 20);
		this.assertEquals(this.arrayReverseDoReturn, 20);
	}

	test_return_from_dictionary_loop {
		this.assertEquals(this.dictionaryReturn, [\a, 1]);
	}

	test_return_from_switch {
		this.assertEquals(this.switchReturn(1), \one);
		this.assertEquals(this.switchReturn(2), \two);
		this.assertEquals(this.switchReturn(3), \other);
		this.assertEquals(log.asArray, []);
	}

	test_switch_branch_that_does_not_return_falls_through {
		this.assertEquals(this.switchFallThrough(1), \after);
		this.assertEquals(this.switchFallThrough(2), \two);
		this.assertEquals(this.switchFallThrough(3), \after);
	}

	test_return_from_case {
		this.assertEquals(this.caseReturn(-1), \negative);
		this.assertEquals(this.caseReturn(0), \zero);
		this.assertEquals(this.caseReturn(1), \positive);
	}

	test_return_from_and_or {
		this.assertEquals(this.andReturn(true, \b), \shortcut);
		this.assertEquals(this.andReturn(false, \b), \b);
		this.assertEquals(this.orReturn(false), \shortcut);
		this.assertEquals(this.orReturn(true), \a_was_true);
	}

	test_return_from_nil_operators {
		this.assertEquals(this.nilCoalescingReturn(nil), \was_nil);
		this.assertEquals(this.nilCoalescingReturn(3), 3);
		this.assertEquals(this.nilConditionalReturn(nil), \was_nil);
		this.assertEquals(this.nilConditionalReturn(3), \was_not_nil);
	}

	test_return_from_nested_inlined_branches {
		this.assertEquals(this.returnsFromDeeplyNestedInlinedBranches(true, true, true), \abc);
		this.assertEquals(this.returnsFromDeeplyNestedInlinedBranches(true, true, false), \ab);
		this.assertEquals(this.returnsFromDeeplyNestedInlinedBranches(true, false, true), \a);
		this.assertEquals(this.returnsFromDeeplyNestedInlinedBranches(false, true, true), \none);
	}

	test_many_returns_in_one_body {
		this.assertEquals([1, 2, 3, 4].collect { |x| this.multipleReturnsInOneBody(x) }, [\one, \two, \three, \many]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// From real closures
	////////////////////////////////////////////////////////////////////////////////

	test_return_through_collect {
		this.assertEquals(this.nonInlinedClosureReturn([1, 2, 3]), \found);
		this.assertEquals(this.nonInlinedClosureReturn([1, 3]), \not_found);
	}

	test_return_from_a_closure_called_in_the_method {
		this.assertEquals(this.closureCalledInsideReturns, 5, "the ^ in the closure returns from the method");
		this.assertEquals(this.closureNeverCalled, 6);
	}

	test_return_from_a_closure_passed_to_another_method {
		this.assertEquals(this.passedClosureReturns, 7);
		this.assertEquals(log.asArray, [], "the method that called the closure is unwound without finishing");
	}

	test_return_unwinds_through_protect_without_running_cleanup {
		// protect is for errors. A ^ that passes through it unwinds the frames and skips the cleanup.
		// This is how the old compiler behaves too.
		this.assertEquals(this.passedClosureReturnsThroughProtect, 7);
		this.assertEquals(log.asArray, []);
	}

	test_return_through_deep_closures {
		this.assertEquals(this.deepClosureReturn, \deep);
		this.assertEquals(this.deepClosureReturnWithVars, [1, 2, 3]);
	}

	test_return_from_a_closure_after_its_method_returned {
		// There is no method left to return to. No error is raised, the thread that ran the ^ just ends, so it is run
		// in a Routine of its own here. (In the test thread it would end the whole test run.)
		var f = this.returnsDeadClosure, reached = false;
		var r = Routine { f.value; reached = true };
		r.next;
		this.assertEquals(reached, false, "the code after a dead ^ does not run");
	}

	test_routine_closure_yields_without_a_method_to_return_to {
		var r = Routine { 3.do { |i| i.yield } };
		this.assertEquals([r.next, r.next, r.next, r.next], [0, 1, 2, nil]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// try and protect
	////////////////////////////////////////////////////////////////////////////////

	test_return_from_try {
		this.assertEquals(this.tryReturn, \from_try);
	}

	test_return_from_catch_handler {
		this.assertEquals(this.tryWithErrorThenReturn, \from_catch);
	}

	test_return_from_protect_skips_cleanup {
		this.assertEquals(this.protectReturn, \from_protect);
		this.assertEquals(log.asArray, [], "a ^ does not run the cleanup, as in the old compiler");
	}

	test_error_in_protect_runs_cleanup {
		this.assertEquals(try { this.protectError }, nil);
		this.assertEquals(log.asArray, [\cleanup], "an error does");
	}

	test_return_from_nested_protect_skips_both_cleanups {
		this.assertEquals(this.protectReturnNested, \inner);
		this.assertEquals(log.asArray, []);
	}

	test_return_from_protect_cleanup {
		this.assertEquals(this.protectReturnInCleanup, \from_cleanup);
	}

	test_try_leaves_the_stack_as_it_was {
		var before = this.stackDepth, after;
		this.tryReturn;
		this.tryWithErrorThenReturn;
		this.protectReturn;
		after = this.stackDepth;
		this.assertEquals(after, before);
	}

	////////////////////////////////////////////////////////////////////////////////
	// A return in the middle of building another value
	////////////////////////////////////////////////////////////////////////////////
	// Each is run in a Routine of its own, with nothing else half built around the call. See
	// TestCompilerReturnsWithPendingValues below for the same calls with a value waiting on the stack.

	// Runs func in a Routine, answers its value, or \died if the thread ended.
	viaRoutine { |func|
		var result = \died;
		Routine { result = func.value }.next;
		^result
	}

	test_return_in_an_argument {
		this.assertEquals(this.viaRoutine { this.returnInArgument(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInArgument(false) }, [1, 2, 3]);
	}

	test_return_in_an_array_literal {
		this.assertEquals(this.viaRoutine { this.returnInArrayLiteral(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInArrayLiteral(false) }, [1, 2, 3]);
	}

	test_return_in_a_binary_operand {
		this.assertEquals(this.viaRoutine { this.returnInBinaryOperand(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInBinaryOperand(false) }, 3);
	}

	test_return_in_a_receiver {
		this.assertEquals(this.viaRoutine { this.returnInReceiver(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInReceiver(false) }, 2);
	}

	test_return_in_nested_call_arguments {
		this.assertEquals(this.viaRoutine { this.returnInNestedCallArguments(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInNestedCallArguments(false) }, [[1, 2, 3], 4, [5, 6, 7]]);
	}

	test_return_in_a_switch_inside_an_argument {
		this.assertEquals(this.viaRoutine { this.returnInSwitchKeyArguments(1) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInSwitchKeyArguments(5) }, [1, 2, 3]);
	}

	test_return_in_a_condition_inside_an_argument {
		this.assertEquals(this.viaRoutine { this.returnInInlinedConditionOfNestedCall(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInInlinedConditionOfNestedCall(false) }, [1, 2, \yes]);
	}

	test_return_in_a_loop_inside_an_argument {
		this.assertEquals(this.viaRoutine { this.returnInLoopInsideArgument(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInLoopInsideArgument(false) }, [1, 2, \done]);
	}

	test_return_in_a_primitive_argument {
		this.assertEquals(this.viaRoutine { this.returnInPrimitiveArgument(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInPrimitiveArgument(false) }, 5);
	}

	test_return_in_an_argument_of_a_send_on_a_field {
		this.assertEquals(this.viaRoutine { this.returnInSetterArgument(true) }, \early);
		this.assertEquals(log.asArray, []);
		this.assertEquals(this.viaRoutine { this.returnInSetterArgument(false) }, 1);
	}

	test_return_in_a_variable_initialiser {
		this.assertEquals(this.viaRoutine { this.returnInVariableInitialiser(true) }, \early);
		this.assertEquals(this.viaRoutine { this.returnInVariableInitialiser(false) }, 9);
	}

	test_stack_is_unchanged_after_many_returns_from_loops {
		// The depth is read into a variable first: read as an argument it would count the receiver waiting for it.
		var before = this.stackDepth, after;
		100.do { this.nestedDoReturn(4); this.whileReturn(3); this.loopReturn(3); this.switchReturn(1) };
		after = this.stackDepth;
		this.assertEquals(after, before);
	}

	test_repeated_returns_do_not_grow_the_stack {
		var before, after;
		1000.do { this.nonInlinedClosureReturn([1, 2, 3]) };
		before = this.stackDepth;
		1000.do { this.nonInlinedClosureReturn([1, 2, 3]) };
		after = this.stackDepth;
		this.assertEquals(after, before);
	}

	test_return_from_deep_recursion {
		var depth = this.recurseThenReturn(200);
		this.assertEquals(depth, 200);
	}

	recurseThenReturn { |n|
		if (n == 0) { ^0 };
		^this.recurseThenReturn(n - 1) + 1
	}

	test_return_from_a_closure_that_is_a_method_argument_default {
		this.assertEquals(this.defaultedReturn, \from_default);
	}

	defaultedReturn { |f = nil|
		f = f ?? { { ^\from_default } };
		f.value;
		^\after
	}
}

// The same returns as in "A return in the middle of building another value" in TestCompilerReturns, but with a value
// waiting on the stack, as in `[\pending, this.returnInArgument(true)]`.
//
// A `^` written in the method's own frame, while a call is half built, must remove the operands already pushed for
// that call. `returnFromMethod` used to trim the stack only when the return crossed more than one frame, so these
// operands were left under the result and the caller read them instead of its own values, e.g.,
// `[\pending, this.returnInArgument(true)]` was `[1, 2]`, and some shapes ended the thread with an error.

TestCompilerReturnsWithPendingValues : UnitTest {
	var returns;

	setUp { returns = TestCompilerReturns.new; returns.setUp }

	// Runs func in a Routine, answers its value, \error if it threw, or \died if the thread ended.
	viaRoutine { |func|
		var result = \died;
		Routine { result = try { func.value } { \error } }.next;
		^result
	}

	test_pending_value_with_return_in_an_argument {
		this.assertEquals(this.viaRoutine { [\pending, returns.returnInArgument(true)] }, [\pending, \early]);
	}

	test_pending_value_with_return_in_an_array_literal {
		this.assertEquals(this.viaRoutine { [\pending, returns.returnInArrayLiteral(true)] }, [\pending, \early]);
	}

	test_pending_value_with_return_in_a_binary_operand {
		this.assertEquals(this.viaRoutine { [\pending, returns.returnInBinaryOperand(true)] }, [\pending, \early]);
	}

	test_pending_value_with_return_in_nested_call_arguments {
		this.assertEquals(this.viaRoutine { [\pending, returns.returnInNestedCallArguments(true)] }, [\pending, \early]);
	}

	test_pending_value_with_return_in_a_variable_initialiser {
		this.assertEquals(this.viaRoutine { [\pending, returns.returnInVariableInitialiser(true)] }, [\pending, \early]);
	}

	test_pending_receiver_with_return_in_an_argument {
		this.assertEquals(this.viaRoutine { returns.returnInArgument(true).asString }, "early");
	}

	test_pending_value_with_return_through_a_closure {
		// This one is trimmed and works.
		this.assertEquals(this.viaRoutine { [\pending, returns.nonInlinedClosureReturn([1, 2, 3])] }, [\pending, \found]);
	}

	test_pending_value_with_plain_return_from_if {
		this.assertEquals(this.viaRoutine { [\pending, returns.ifReturn(true)] }, [\pending, 1]);
	}
}
