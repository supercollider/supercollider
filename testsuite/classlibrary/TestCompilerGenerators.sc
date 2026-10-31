//# VERSION 3.16
// Generator expressions: `{: body, qualifiers }` is a Routine yielding body for each solution, `{; body, qualifiers }`
// runs body for each solution straight away. Every qualifier opens a scope of its own, so a closure made in the body
// sees the values of that pass. The tests cover each kind of qualifier (generator, guard, var, side effect,
// :while), several of them together, nesting, closures over qualifier variables, `^` from inside, and the order of
// side effects. TestGeneratorExpr has the examples from the guide.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerGenerators : UnitTest {
	var log;

	setUp { log = List.new }
	mark { |x| log.add(x); ^x }

	////////////////////////////////////////////////////////////////////////////////
	// Methods under test
	////////////////////////////////////////////////////////////////////////////////

	returnFromGenerator { var r = {: if (x > 2) { ^x } { x }, x <- (1..5) }; ^[\after, r.all] }
	returnFromGeneratorNothingPending { var r = {: if (x > 2) { ^x } { x }, x <- (1..5) }; r.all; ^\after }
	returnFromLoop { {; if (x > 2) { ^x }, x <- (1..5) }; ^\after }
	returnFromGuard { ^all {: x, x <- (1..5), if (x == 4) { ^[\guard, x] } { true } } }
	generatorOverArgument { |items| ^all {: x * 2, x <- items } }
	generatorOverInstanceVariable { log = [1, 2, 3]; ^all {: x + 1, x <- log } }

	////////////////////////////////////////////////////////////////////////////////
	// Qualifiers
	////////////////////////////////////////////////////////////////////////////////

	test_generator_qualifier {
		this.assertEquals(all {: x * 2, x <- (1..4) }, [2, 4, 6, 8]);
		this.assertEquals(all {: x, x <- [\a, \b] }, [\a, \b]);
		this.assertEquals(all {: x, x <- 4 }, [0, 1, 2, 3], "anything that responds to do");
		this.assertEquals(all {: x, x <- (1, 3 .. 9) }, [1, 3, 5, 7, 9], "a series with a step");
		this.assertEquals(all {: x, x <- (3..1) }, [3, 2, 1], "a series counting down");
		this.assertEquals(all {: x, x <- [] }, nil, "no solutions");
	}

	test_generator_qualifier_with_index {
		this.assertEquals(all {: [x, i], x i <- [\a, \b] }, [[\a, 0], [\b, 1]]);
	}

	test_series_without_end {
		var r = {: x, x <- (1..) };
		this.assertEquals([r.next, r.next, r.next], [1, 2, 3]);
		r = {: x, x <- (1..inf) };
		this.assertEquals([r.next, r.next], [1.0, 2.0], "a Float end makes a Float series");
	}

	test_guard {
		this.assertEquals(all {: x, x <- (1..10), x.odd }, [1, 3, 5, 7, 9]);
		this.assertEquals(all {: x, x <- (1..10), x.odd, x > 4 }, [5, 7, 9], "two guards");
	}

	test_several_generators {
		this.assertEquals(all {: [x, y], x <- (1..3), y <- (x..3) }, [[1, 1], [1, 2], [1, 3], [2, 2], [2, 3], [3, 3]]);
		this.assertEquals(all {: [x, y, z], x <- (1..2), y <- (1..2), z <- [\a] },
			[[1, 1, \a], [1, 2, \a], [2, 1, \a], [2, 2, \a]]);
	}

	test_var_qualifier {
		this.assertEquals(all {: [x, y], x <- (1..3), var y = x * 10 }, [[1, 10], [2, 20], [3, 30]]);
		this.assertEquals(all {: x, x <- (1..3), var x = x * 100 }, [100, 200, 300], "a var shadows the generator");
		this.assertEquals(all {: z, x <- (1..3), var y = x + 1, var z = y * y }, [4, 9, 16]);
	}

	test_var_qualifier_before_any_generator {
		this.assertEquals(all {: x + y, var y = 10, x <- (1..3) }, [11, 12, 13]);
	}

	test_while_ends_the_whole_generator {
		this.assertEquals(all {: x, x <- (1..10), :while x < 4 }, [1, 2, 3]);
		this.assertEquals(all {: [x, y], x <- (1..3), :while x < 3, y <- (1..2) }, [[1, 1], [1, 2], [2, 1], [2, 2]]);
		// The inner loop's first false ends the outer loop too.
		this.assertEquals(all {: [x, y], x <- (1..3), y <- (1..3), :while y < 2 }, [[1, 1]]);
		this.assertEquals(all {: [x, y], x <- (1..3), y <- (1..3), :while y < x }, nil);
	}

	test_side_effects_run_in_order {
		var result = all {: x, x <- (1..3), :: this.mark([\a, x]), x.odd, :: this.mark([\b, x]) };
		this.assertEquals(result, [1, 3]);
		this.assertEquals(log.asArray, [[\a, 1], [\b, 1], [\a, 2], [\a, 3], [\b, 3]]);
	}

	test_generator_is_lazy {
		var r = {: this.mark(x), x <- (1..5) };
		this.assertEquals(r.class, Routine);
		this.assertEquals(log.size, 0, "nothing runs until next");
		this.assertEquals([r.next, r.next], [1, 2]);
		this.assertEquals(log.asArray, [1, 2]);
	}

	test_loop_expression_runs_at_once {
		{; this.mark(x * 2), x <- (1..3) };
		this.assertEquals(log.asArray, [2, 4, 6]);
		{; this.mark([x, y]), x <- (1..2), y <- [\a], x.even };
		this.assertEquals(log.asArray, [2, 4, 6, [2, \a]]);
	}

	test_generator_over_arguments_and_instance_variables {
		this.assertEquals(this.generatorOverArgument([1, 2]), [2, 4]);
		this.assertEquals(this.generatorOverInstanceVariable, [2, 3, 4]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Scopes
	////////////////////////////////////////////////////////////////////////////////

	test_closures_capture_each_pass {
		var functions = all {: { x }, x <- (1..3) };
		this.assertEquals(functions.collect(_.value), [1, 2, 3]);
	}

	test_closures_capture_var_qualifiers {
		var functions = all {: { [x, y] }, x <- (1..2), var y = x * 10 };
		this.assertEquals(functions.collect(_.value), [[1, 10], [2, 20]]);
	}

	test_closures_capture_both_loops {
		var functions = all {: { [x, y] }, x <- (1..2), y <- [\a, \b] };
		this.assertEquals(functions.collect(_.value), [[1, \a], [1, \b], [2, \a], [2, \b]]);
	}

	test_closure_writes_its_own_pass {
		var functions = all {: { x = x + 100; x }, x <- (1..3) };
		this.assertEquals(functions.collect(_.value), [101, 102, 103]);
		this.assertEquals(functions.collect(_.value), [201, 202, 203], "each closure keeps its own x");
	}

	test_generator_reads_and_writes_outer_variables {
		var total = 0, result;
		result = all {: total = total + x, x <- (1..4) };
		this.assertEquals(result, [1, 3, 6, 10]);
		this.assertEquals(total, 10);
	}

	test_nested_generators {
		this.assertEquals(all {: all {: [x, y], y <- (1..2) }, x <- (1..2) }, [[[1, 1], [1, 2]], [[2, 1], [2, 2]]]);
		this.assertEquals(all {: x + all {: y, y <- (1..x) }.sum, x <- (1..3) }, [2, 5, 9]);
	}

	test_generator_inside_inlined_code {
		var result = if (log.isEmpty) { all {: x, x <- (1..2) } } { nil };
		this.assertEquals(result, [1, 2]);
		result = [];
		2.do { |i| result = result ++ all {: [i, x], x <- (1..2) } };
		this.assertEquals(result, [[0, 1], [0, 2], [1, 1], [1, 2]]);
	}

	test_interpreted_generator {
		this.assertEquals("all {: x * y, x <- (1..3), var y = x + 1 }".interpret, [2, 6, 12]);
		this.assertEquals("all {: { x }, x <- (1..3) }.collect(_.value)".interpret, [1, 2, 3]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// ^ from inside
	////////////////////////////////////////////////////////////////////////////////

	test_return_from_inside_a_generator {
		// The Routine runs in a thread of its own, the ^ still reaches the method that made it. The method was
		// building [\after, r.all] when the ^ came, and those operands must not be left on the stack
		// (compiler-tests finding 10: the VM skips that clean up when the ^ yields to a parent thread). The result is
		// read inside an array, so stray operands show up as a wrong array rather than as a wrong receiver.
		var thread = thisThread, result;
		result = [1, this.returnFromGenerator, 2];
		this.assertEquals(result, [1, 3, 2]);
		this.assert(thisThread === thread, "back in the calling thread");
		this.assertEquals(all {: x, x <- (1..2) }, [1, 2], "generators still work");
	}

	test_return_from_inside_a_generator_without_pending_operands {
		var result = [1, this.returnFromGeneratorNothingPending, 2];
		this.assertEquals(result, [1, 3, 2]);
	}

	test_return_from_inside_a_loop_expression {
		this.assertEquals(this.returnFromLoop, 3);
	}

	test_return_from_a_guard {
		this.assertEquals(this.returnFromGuard, [\guard, 4]);
	}
}
