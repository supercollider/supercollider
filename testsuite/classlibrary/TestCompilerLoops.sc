//# VERSION 3.16
// The methods whose code the compiler replaces with special loop opcodes: Integer do, reverseDo, for and forBy,
// Float do and reverseDo, ArrayedCollection do and reverseDo, Number forSeries, and Dictionary keysValuesArrayDo
// (which keysValuesDo, do and keysDo go through). For each: how many passes, the arguments each pass gets, the value
// the loop answers, and the edges (zero, negative, fractional, empty, changed during the loop, yield, recursion).
// TestOpcodes covers the basics of Integer do and reverseDo. Variables of nested functions are declared first in
// their body so the same text also compiles as Legacy.

TestCompilerLoops : UnitTest {

	// Every pass's arguments, as [first, second].
	passesOf { |loop|
		var out = List.new;
		loop.value({ |a, b| out.add([a, b]) });
		^out.asArray
	}

	recursiveArrayDo { |depth, log|
		if (depth > 0) {
			[\a, \b].do { |item, i|
				var mine;
				mine = [depth, item];
				this.recursiveArrayDo(depth - 1, log);
				log.add(mine);
			}
		}
	}

	recursiveFor { |depth, log|
		if (depth > 0) {
			1.for(2) { |i|
				var mine;
				mine = depth * 10 + i;
				this.recursiveFor(depth - 1, log);
				log.add(mine);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// Integer do
	////////////////////////////////////////////////////////////////////////////////

	test_integer_do_arguments {
		this.assertEquals(this.passesOf { |f| 3.do(f) }, [[0, 0], [1, 1], [2, 2]], "the index is passed twice");
	}

	test_integer_do_answers_the_receiver {
		this.assertEquals(3.do { 1 }, 3);
		this.assertEquals(0.do { 1 }, 0);
	}

	test_integer_do_counts {
		var n = 0;
		1.do { n = n + 1 };
		this.assertEquals(n, 1);
		n = 0;
		10000.do { n = n + 1 };
		this.assertEquals(n, 10000);
		n = 0;
		10000.do { |i| n = n + i };
		this.assertEquals(n, 49995000);
	}

	test_integer_do_zero_and_negative {
		this.assertEquals(this.passesOf { |f| 0.do(f) }, []);
		this.assertEquals(this.passesOf { |f| -5.do(f) }, []);
	}

	test_assigning_the_loop_argument_does_not_change_the_loop {
		var seen = List.new;
		5.do { |i| i = i + 10; seen.add(i) };
		this.assertEquals(seen.asArray, [10, 11, 12, 13, 14]);
	}

	test_integer_do_function_arguments_beyond_two_are_nil {
		var seen = List.new;
		2.do { |a, b, c| seen.add(c) };
		this.assertEquals(seen.asArray, [nil, nil]);
	}

	test_integer_do_function_with_variable_arguments {
		var seen = List.new;
		2.do { |...args| seen.add(args) };
		this.assertEquals(seen.asArray, [[0, 0], [1, 1]]);
	}

	test_integer_do_function_with_a_default {
		var seen = List.new;
		2.do { |a, b = 99| seen.add(b) };
		this.assertEquals(seen.asArray, [0, 1], "the default is replaced by the index");
	}

	test_integer_do_function_without_arguments {
		var n = 0;
		4.do { n = n + 1 };
		this.assertEquals(n, 4);
	}

	test_integer_do_with_a_function_in_a_variable {
		var n = 0, f = { |i| n = n + i };
		4.do(f);
		this.assertEquals(n, 6);
		this.assertEquals(4.perform(\do, f), 4, "and when performed");
		this.assertEquals(n, 12);
	}

	test_integer_do_with_a_value_that_is_not_a_function {
		this.assertEquals(3.do(nil), 3);
		this.assertEquals(3.do(\sym), 3, "anything that answers value");
	}

	test_integer_do_receiver_is_an_expression {
		var n = 0, k = 2;
		(k + 1).do { n = n + 1 };
		this.assertEquals(n, 3);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Integer reverseDo
	////////////////////////////////////////////////////////////////////////////////

	test_integer_reverseDo_arguments {
		this.assertEquals(this.passesOf { |f| 3.reverseDo(f) }, [[2, 0], [1, 1], [0, 2]], "the value counts down, the second counts up");
	}

	test_integer_reverseDo_answers_the_receiver {
		this.assertEquals(3.reverseDo { 1 }, 3);
	}

	test_integer_reverseDo_zero_negative_and_one {
		this.assertEquals(this.passesOf { |f| 0.reverseDo(f) }, []);
		this.assertEquals(this.passesOf { |f| -2.reverseDo(f) }, []);
		this.assertEquals(this.passesOf { |f| 1.reverseDo(f) }, [[0, 0]]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Integer for and forBy
	////////////////////////////////////////////////////////////////////////////////

	test_integer_for_arguments {
		this.assertEquals(this.passesOf { |f| 1.for(3, f) }, [[1, 0], [2, 1], [3, 2]], "inclusive, with a counter");
		this.assertEquals(this.passesOf { |f| -2.for(0, f) }, [[-2, 0], [-1, 1], [0, 2]]);
	}

	test_integer_for_single_and_counting_down {
		// The loop opcode steps by -1 when the end is below the start. The Integer:for source in the class library only
		// counts up, but it is not what runs.
		this.assertEquals(this.passesOf { |f| 4.for(4, f) }, [[4, 0]]);
		this.assertEquals(this.passesOf { |f| 5.for(3, f) }, [[5, 0], [4, 1], [3, 2]]);
	}

	test_integer_for_answers_the_receiver {
		this.assertEquals(1.for(3) { 0 }, 1);
	}

	test_integer_for_with_a_float_end {
		this.assertEquals(this.passesOf { |f| 1.for(3.5, f) }, [[1, 0], [2, 1], [3, 2]]);
	}

	test_integer_forBy_arguments {
		this.assertEquals(this.passesOf { |f| 1.forBy(7, 3, f) }, [[1, 0], [4, 1], [7, 2]]);
		this.assertEquals(this.passesOf { |f| 1.forBy(8, 3, f) }, [[1, 0], [4, 1], [7, 2]], "the end need not be reached exactly");
	}

	test_integer_forBy_single_and_empty {
		this.assertEquals(this.passesOf { |f| 3.forBy(3, 5, f) }, [[3, 0]]);
		this.assertEquals(this.passesOf { |f| 4.forBy(3, 1, f) }, []);
	}

	test_integer_forBy_negative_step {
		this.assertEquals(this.passesOf { |f| 10.forBy(4, -3, f) }, [[10, 0], [7, 1], [4, 2]]);
	}

	test_integer_forBy_float_step {
		this.assertEquals(this.passesOf { |f| 0.forBy(1, 0.25, f) }, [[0, 0], [0.25, 1], [0.5, 2], [0.75, 3], [1.0, 4]]);
	}

	test_integer_forBy_answers_the_receiver {
		this.assertEquals(1.forBy(5, 2) { 0 }, 1);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Number for, forBy and forSeries
	////////////////////////////////////////////////////////////////////////////////

	test_float_for {
		this.assertEquals(this.passesOf { |f| 1.0.for(3, f) }, [[1.0, 0], [2.0, 1], [3.0, 2]]);
		this.assertEquals(this.passesOf { |f| 0.5.for(2, f) }, [[0.5, 0], [1.5, 1]]);
	}

	test_float_forBy {
		this.assertEquals(this.passesOf { |f| 0.5.forBy(2, 0.5, f) }, [[0.5, 0], [1.0, 1], [1.5, 2], [2.0, 3]]);
		this.assertEquals(this.passesOf { |f| 2.0.forBy(1, -0.5, f) }, [[2.0, 0], [1.5, 1], [1.0, 2]]);
	}

	test_forSeries_with_a_step {
		this.assertEquals(this.passesOf { |f| 1.forSeries(3, 7, f) }, [[1, 0], [3, 1], [5, 2], [7, 3]]);
		this.assertEquals(this.passesOf { |f| 10.forSeries(7, 1, f) }, [[10, 0], [7, 1], [4, 2], [1, 3]], "counting down");
	}

	test_forSeries_without_a_step {
		this.assertEquals(this.passesOf { |f| 1.forSeries(nil, 4, f) }, [[1, 0], [2, 1], [3, 2], [4, 3]]);
		this.assertEquals(this.passesOf { |f| 4.forSeries(nil, 1, f) }, [[4, 0], [3, 1], [2, 2], [1, 3]], "steps down when the end is lower");
	}

	test_forSeries_float {
		this.assertEquals(this.passesOf { |f| 0.0.forSeries(0.5, 1.5, f) }, [[0.0, 0], [0.5, 1], [1.0, 2], [1.5, 3]]);
	}

	test_forSeries_single {
		this.assertEquals(this.passesOf { |f| 3.forSeries(4, 3, f) }, [[3, 0]]);
	}

	test_series_syntax_uses_forSeries {
		var seen = List.new;
		(1, 3 .. 9).do { |x| seen.add(x) };
		this.assertEquals(seen.asArray, [1, 3, 5, 7, 9]);
		seen.clear;
		(5 .. 1).do { |x| seen.add(x) };
		this.assertEquals(seen.asArray, [5, 4, 3, 2, 1]);
	}

	test_series_in_a_list_comprehension {
		this.assertEquals(all {: x * 2, x <- (1 .. 4) }, [2, 4, 6, 8]);
		this.assertEquals(all {: x, x <- (10, 8 .. 4) }, [10, 8, 6, 4]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Float do and reverseDo
	////////////////////////////////////////////////////////////////////////////////

	test_float_do_arguments {
		this.assertEquals(this.passesOf { |f| 3.0.do(f) }, [[0.0, 0.0], [1.0, 1.0], [2.0, 2.0]], "the values are Floats");
	}

	test_float_do_rounds_the_count {
		this.assertEquals(this.passesOf { |f| 2.5.do(f) }.size, 2, "a pass runs while i + 0.5 < this");
		this.assertEquals(this.passesOf { |f| 2.6.do(f) }.size, 3);
		this.assertEquals(this.passesOf { |f| 2.4.do(f) }.size, 2);
		this.assertEquals(this.passesOf { |f| 0.4.do(f) }.size, 0);
		this.assertEquals(this.passesOf { |f| 0.6.do(f) }.size, 1);
	}

	test_float_do_zero_and_negative {
		this.assertEquals(this.passesOf { |f| 0.0.do(f) }, []);
		this.assertEquals(this.passesOf { |f| -3.0.do(f) }, []);
	}

	test_float_do_answers_the_receiver {
		this.assertEquals(3.0.do { 1 }, 3.0);
	}

	test_float_reverseDo_arguments {
		// The value counts down and the second argument counts up, as in Integer:reverseDo.
		this.assertEquals(this.passesOf { |f| 3.0.reverseDo(f) }, [[2.0, 0.0], [1.0, 1.0], [0.0, 2.0]]);
	}

	test_float_reverseDo_fractional {
		// A pass runs while i + 0.5 >= 0, so -0.5 is the last.
		this.assertEquals(this.passesOf { |f| 2.5.reverseDo(f) }.collect(_.first), [1.5, 0.5, -0.5]);
		this.assertEquals(this.passesOf { |f| 0.4.reverseDo(f) }, []);
	}

	////////////////////////////////////////////////////////////////////////////////
	// ArrayedCollection do and reverseDo
	////////////////////////////////////////////////////////////////////////////////

	test_array_do_arguments {
		this.assertEquals(this.passesOf { |f| [\a, \b, \c].do(f) }, [[\a, 0], [\b, 1], [\c, 2]]);
	}

	test_array_reverseDo_arguments {
		this.assertEquals(this.passesOf { |f| [\a, \b, \c].reverseDo(f) }, [[\c, 0], [\b, 1], [\a, 2]]);
	}

	test_array_do_answers_the_receiver {
		var a = [1, 2];
		this.assert(a.do { 0 } === a);
		this.assert(a.reverseDo { 0 } === a);
	}

	test_array_do_empty_and_single {
		this.assertEquals(this.passesOf { |f| [].do(f) }, []);
		this.assertEquals(this.passesOf { |f| [].reverseDo(f) }, []);
		this.assertEquals(this.passesOf { |f| [7].do(f) }, [[7, 0]]);
		this.assertEquals(this.passesOf { |f| [7].reverseDo(f) }, [[7, 0]]);
	}

	test_array_do_with_nil_elements {
		this.assertEquals(this.passesOf { |f| [nil, 1, nil].do(f) }, [[nil, 0], [1, 1], [nil, 2]]);
	}

	test_literal_array_do {
		this.assertEquals(this.passesOf { |f| #[1, 2, 3].do(f) }, [[1, 0], [2, 1], [3, 2]]);
	}

	test_other_arrayed_collections {
		this.assertEquals(this.passesOf { |f| "abc".do(f) }, [[$a, 0], [$b, 1], [$c, 2]], "String");
		this.assertEquals(this.passesOf { |f| Int8Array[1, -2].do(f) }, [[1, 0], [-2, 1]], "Int8Array");
		this.assertEquals(this.passesOf { |f| FloatArray[0.5, 1.5].do(f) }, [[0.5, 0], [1.5, 1]], "FloatArray");
		this.assertEquals(this.passesOf { |f| Signal[0.25].do(f) }, [[0.25, 0]], "Signal");
		this.assertEquals(this.passesOf { |f| "ab".reverseDo(f) }, [[$b, 0], [$a, 1]]);
	}

	test_array_do_reads_the_size_on_every_pass {
		// The array shrinks during the loop. The loop stops at the new end.
		var arr = [1, 2, 3, 4, 5].copy, seen = List.new;
		arr.do { |x, i| seen.add(x); if (i == 0) { arr.removeAt(arr.size - 1); arr.removeAt(arr.size - 1) } };
		this.assertEquals(seen.asArray, [1, 2, 3]);
	}

	test_array_do_sees_changed_elements {
		var arr = [1, 2, 3].copy, seen = List.new;
		arr.do { |x, i| seen.add(x); if (i == 0) { arr[2] = 30 } };
		this.assertEquals(seen.asArray, [1, 2, 30]);
	}

	test_array_do_with_a_function_in_a_variable {
		var sum = 0, f = { |x| sum = sum + x };
		[1, 2, 3].do(f);
		this.assertEquals(sum, 6);
		[1, 2, 3].perform(\do, f);
		this.assertEquals(sum, 12);
	}

	test_list_do_is_not_an_arrayed_collection {
		// List is not an ArrayedCollection, its do is a normal method.
		this.assertEquals(this.passesOf { |f| List[\a, \b].do(f) }, [[\a, 0], [\b, 1]]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Dictionary
	////////////////////////////////////////////////////////////////////////////////

	test_dictionary_keysValuesDo {
		var d = Dictionary[\a -> 1, \b -> 2, \c -> 3], seen = List.new;
		d.keysValuesDo { |k, v, i| seen.add([k, v]) };
		this.assertEquals(seen.asArray.sort { |x, y| x[0] < y[0] }, [[\a, 1], [\b, 2], [\c, 3]]);
	}

	test_dictionary_keysValuesDo_counter {
		var d = Dictionary[\a -> 1, \b -> 2, \c -> 3], counters = List.new;
		d.keysValuesDo { |k, v, i| counters.add(i) };
		this.assertEquals(counters.asArray, [0, 1, 2], "the counter counts entries, not slots of the table");
	}

	test_dictionary_do_and_keysDo {
		var d = Dictionary[\a -> 1, \b -> 2], values = List.new, keys = List.new;
		d.do { |v| values.add(v) };
		d.keysDo { |k| keys.add(k) };
		this.assertEquals(values.asArray.sort, [1, 2]);
		this.assertEquals(keys.asArray.sort, [\a, \b]);
	}

	test_dictionary_empty {
		var n = 0;
		Dictionary.new.keysValuesDo { n = n + 1 };
		this.assertEquals(n, 0);
	}

	test_dictionary_after_removing_entries {
		// Removed entries leave holes in the table, which are skipped.
		var d = Dictionary.new, seen = List.new;
		10.do { |i| d[i] = i * 10 };
		[1, 3, 5, 7, 9].do { |i| d.removeAt(i) };
		d.keysValuesDo { |k, v| seen.add(k) };
		this.assertEquals(seen.asArray.sort, [0, 2, 4, 6, 8]);
	}

	test_dictionary_nil_values {
		// A nil value is not stored, so it is not visited.
		var d = Dictionary[\a -> 1], n = 0;
		d[\b] = nil;
		d.keysValuesDo { n = n + 1 };
		this.assertEquals(n, 1);
	}

	test_identity_dictionary_and_event {
		var seen = List.new;
		IdentityDictionary[\x -> 1, \y -> 2].keysValuesDo { |k, v| seen.add(v) };
		this.assertEquals(seen.asArray.sort, [1, 2]);
		seen.clear;
		(a: 5, b: 6).keysValuesDo { |k, v| seen.add(v) };
		this.assertEquals(seen.asArray.sort, [5, 6]);
	}

	test_keysValuesArrayDo_directly {
		var seen = List.new;
		Dictionary.new.keysValuesArrayDo([\a, 1, nil, nil, \b, 2], { |k, v, i| seen.add([k, v, i]) });
		this.assertEquals(seen.asArray, [[\a, 1, 0], [\b, 2, 1]], "nil keys are empty slots");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Loops together with errors, yields and recursion
	////////////////////////////////////////////////////////////////////////////////

	test_error_stops_the_loop {
		var n = 0;
		try { 10.do { |i| n = n + 1; if (i == 3) { Error("stop").throw } } };
		this.assertEquals(n, 4);
		n = 0;
		try { [1, 2, 3, 4].do { |x| n = n + 1; if (x == 2) { Error("stop").throw } } };
		this.assertEquals(n, 2);
	}

	test_loop_runs_again_after_an_error {
		var n = 0;
		try { 3.do { Error("stop").throw } };
		3.do { n = n + 1 };
		this.assertEquals(n, 3);
	}

	test_yield_inside_every_loop_kind {
		var r = Routine {
			2.do { |i| [\do, i].yield };
			2.reverseDo { |i| [\reverseDo, i].yield };
			1.for(2) { |i| [\for, i].yield };
			0.forBy(4, 2) { |i| [\forBy, i].yield };
			2.0.do { |i| [\floatDo, i].yield };
			[\x, \y].do { |x| [\arrayDo, x].yield };
			[\x, \y].reverseDo { |x| [\arrayReverseDo, x].yield };
			Dictionary[\k -> \v].keysValuesDo { |k, v| [\dict, k, v].yield };
			1.forSeries(3, 5) { |i| [\forSeries, i].yield };
		};
		var out = List.new, v;
		while { (v = r.next).notNil } { out.add(v) };
		this.assertEquals(out.asArray, [
			[\do, 0], [\do, 1],
			[\reverseDo, 1], [\reverseDo, 0],
			[\for, 1], [\for, 2],
			[\forBy, 0], [\forBy, 2], [\forBy, 4],
			[\floatDo, 0.0], [\floatDo, 1.0],
			[\arrayDo, \x], [\arrayDo, \y],
			[\arrayReverseDo, \y], [\arrayReverseDo, \x],
			[\dict, \k, \v],
			[\forSeries, 1], [\forSeries, 3], [\forSeries, 5]
		]);
	}

	test_two_routines_in_the_same_loop_do_not_share_the_counter {
		var make = { Routine { 3.do { |i| i.yield } } };
		var a = make.value, b = make.value;
		this.assertEquals([a.next, a.next, b.next, a.next, b.next], [0, 1, 0, 2, 1]);
	}

	test_recursion_inside_array_do {
		var log = List.new;
		this.recursiveArrayDo(2, log);
		this.assertEquals(log.asArray, [[1, \a], [1, \b], [2, \a], [1, \a], [1, \b], [2, \b]]);
	}

	test_recursion_inside_for {
		var log = List.new;
		this.recursiveFor(2, log);
		this.assertEquals(log.asArray, [11, 12, 21, 11, 12, 22]);
	}

	test_nested_loops_of_every_kind {
		var out = List.new;
		2.do { |i| [\a, \b].do { |x| 1.for(2) { |j| out.add([i, x, j]) } } };
		this.assertEquals(out.size, 8);
		this.assertEquals(out.first, [0, \a, 1]);
		this.assertEquals(out.last, [1, \b, 2]);
	}

	test_the_same_function_in_two_loops_at_once {
		var log = List.new, f;
		f = { |x| log.add(x); if (x == \outer) { [\inner].do(f) } };
		[\outer, \second].do(f);
		this.assertEquals(log.asArray, [\outer, \inner, \second]);
	}
}
