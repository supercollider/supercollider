//# VERSION 3.16
// Expressions: binary operators (all left to right, no precedence), unary minus, adverbs, operators sent to objects
// that define them, literals, indexing and series, setters, splats and multiple assignment.
// Where the compiler may fold an expression of literals into a constant, the folded value is compared with the
// same operation done at run time on values it cannot know, so a fold that differs from the real operation fails.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerOperators : UnitTest {

	// The operation done at run time, on arguments the compiler cannot know.
	runOp { |a, selector, b| ^a.perform(selector, b) }
	runUnary { |a, selector| ^a.perform(selector) }

	take { |a, b, c| ^[a, b, c] }

	////////////////////////////////////////////////////////////////////////////////
	// Order of evaluation
	////////////////////////////////////////////////////////////////////////////////

	test_binary_operators_are_left_to_right {
		this.assertEquals(1 + 2 * 3, 9);
		this.assertEquals(2 * 3 + 1, 7);
		this.assertEquals(10 - 2 - 3, 5);
		this.assertEquals(2 ** 3 ** 2, 64, "(2 ** 3) ** 2");
		this.assertEquals(12 / 2 / 3, 2);
		this.assertEquals(1 + 2 == 3, true);
		this.assertEquals(2 * (3 + 1), 8, "parentheses");
	}

	test_unary_messages_bind_tighter_than_binary {
		this.assertEquals(2 + 3.squared, 11);
		this.assertEquals(2.squared + 3, 7);
		this.assertEquals(1 + 2.neg, -1);
		this.assertEquals([1, 2, 3].size + 1, 4);
	}

	test_keyword_binary_operators {
		this.assertEquals(3 max: 5, 5);
		this.assertEquals(3 max: 5 + 1, 6, "(3 max: 5) + 1");
		this.assertEquals(7 div: 2, 3);
		this.assertEquals(2 + 3 min: 4, 4, "(2 + 3) min: 4");
	}

	test_operands_are_evaluated_left_to_right {
		var log = List.new;
		var mark = { |x| log.add(x); x };
		mark.(1) + mark.(2) * mark.(3);
		this.assertEquals(log.asArray, [1, 2, 3]);
		log.clear;
		this.take(mark.(\a), mark.(\b), mark.(\c));
		this.assertEquals(log.asArray, [\a, \b, \c]);
		log.clear;
		[mark.(1), mark.(2)] ++ [mark.(3)];
		this.assertEquals(log.asArray, [1, 2, 3]);
	}

	test_receiver_is_evaluated_before_arguments {
		var log = List.new;
		var mark = { |x| log.add(x); x };
		mark.(5).max(mark.(7));
		this.assertEquals(log.asArray, [5, 7]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Unary minus and negative literals
	////////////////////////////////////////////////////////////////////////////////

	test_negative_literals {
		this.assertEquals(-1 + 0, -1);
		this.assertEquals(-1.5 * 2, -3.0);
		this.assertEquals(3 - -1, 4);
		this.assertEquals(-2.abs, 2, "the literal is -2, abs is sent to it");
		this.assertEquals(-2.squared, 4);
	}

	test_negation_of_an_expression {
		// Unary minus is only part of a number literal, an expression is negated with neg.
		var x = 5;
		this.assertEquals(x.neg, -5);
		this.assertEquals(1 - x, -4);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Folding gives the same answer as the run time operation
	////////////////////////////////////////////////////////////////////////////////

	test_folded_integer_arithmetic {
		this.assertEquals(2 + 3, this.runOp(2, '+', 3));
		this.assertEquals(2 - 3, this.runOp(2, '-', 3));
		this.assertEquals(6 * 7, this.runOp(6, '*', 7));
		this.assertEquals(1 / 2, this.runOp(1, '/', 2), "an Integer divided by an Integer can be a Float");
		this.assertEquals(7 div: 2, this.runOp(7, 'div', 2));
		this.assertEquals(-7 div: 2, this.runOp(-7, 'div', 2));
		this.assertEquals(7 % 3, this.runOp(7, '%', 3));
		this.assertEquals(-7 % 3, this.runOp(-7, '%', 3));
		this.assertEquals(2 ** 10, this.runOp(2, '**', 10));
		this.assertEquals(2 ** -1, this.runOp(2, '**', -1));
	}

	test_folded_integer_overflow {
		this.assertEquals(2147483647 + 1, this.runOp(2147483647, '+', 1));
		this.assertEquals(-2147483648 - 1, this.runOp(-2147483648, '-', 1));
		this.assertEquals(65536 * 65536, this.runOp(65536, '*', 65536));
		this.assertEquals(2147483647 * 2, this.runOp(2147483647, '*', 2));
		this.assertEquals(-2147483648.neg, this.runUnary(-2147483648, 'neg'));
		this.assertEquals(-2147483648.abs, this.runUnary(-2147483648, 'abs'));
	}

	test_folded_large_literals {
		// A literal outside the 32 bit range is a compile error (the old compiler wrapped it), so only the edges here.
		this.assertEquals(2147483647.class, Integer);
		this.assertEquals(-2147483648.class, Integer);
		this.assertEquals(2147483648.0.class, Float);
		this.assertEquals(2147483648.0, 2147483647 + 1.0);
	}

	test_folded_bit_operations {
		this.assertEquals(1 << 31, this.runOp(1, '<<', 31));
		this.assertEquals(1 << 32, this.runOp(1, '<<', 32));
		this.assertEquals(-1 >> 1, this.runOp(-1, '>>', 1));
		this.assertEquals(-8 >> 1, this.runOp(-8, '>>', 1));
		this.assertEquals(6 bitAnd: 3, this.runOp(6, 'bitAnd', 3));
		this.assertEquals(6 bitOr: 3, this.runOp(6, 'bitOr', 3));
		this.assertEquals(6 bitXor: 3, this.runOp(6, 'bitXor', 3));
	}

	test_folded_float_arithmetic {
		this.assertEquals(0.1 + 0.2, this.runOp(0.1, '+', 0.2));
		this.assertEquals(1.0 / 3.0, this.runOp(1.0, '/', 3.0));
		this.assertEquals(7.5 % 2, this.runOp(7.5, '%', 2));
		this.assertEquals(-7.5 % 2, this.runOp(-7.5, '%', 2));
		this.assertEquals(2 ** 0.5, this.runOp(2, '**', 0.5));
		this.assertEquals(1 + 0.5, this.runOp(1, '+', 0.5), "mixed Integer and Float");
	}

	test_folded_division_by_zero {
		this.assertEquals(1 / 0, this.runOp(1, '/', 0));
		this.assertEquals(-1 / 0, this.runOp(-1, '/', 0));
		this.assertEquals(1.0 / 0, this.runOp(1.0, '/', 0));
		this.assert((0 / 0).isNaN == this.runOp(0, '/', 0).isNaN, "0 / 0");
		this.assert((0.0 / 0.0).isNaN, "nan");
		this.assert((0.0 / 0.0) != (0.0 / 0.0), "nan is not equal to itself");
	}

	test_folded_comparisons {
		this.assertEquals(1 < 2, this.runOp(1, '<', 2));
		this.assertEquals(2 <= 2, this.runOp(2, '<=', 2));
		this.assertEquals(3 > 2.5, this.runOp(3, '>', 2.5));
		this.assertEquals(5 == 5.0, this.runOp(5, '==', 5.0));
		this.assertEquals(5 != 5.0, this.runOp(5, '!=', 5.0));
		this.assertEquals(5 === 5.0, this.runOp(5, '===', 5.0));
		this.assertEquals(5 !== 5.0, this.runOp(5, '!==', 5.0));
		this.assertEquals(\a == \a, true);
		this.assertEquals(\a === \a, true);
	}

	test_folded_unary_operations {
		this.assertEquals(3.neg, this.runUnary(3, 'neg'));
		this.assertEquals(-3.abs, this.runUnary(-3, 'abs'));
		this.assertEquals(9.sqrt, this.runUnary(9, 'sqrt'));
		this.assertEquals(-9.sqrt.isNaN, this.runUnary(-9, 'sqrt').isNaN);
		this.assertEquals(3.squared, this.runUnary(3, 'squared'));
		this.assertEquals(2.5.floor, this.runUnary(2.5, 'floor'));
		this.assertEquals(-2.5.ceil, this.runUnary(-2.5, 'ceil'));
		this.assertEquals(2.5.round, this.runUnary(2.5, 'round'));
		this.assertEquals(7.reciprocal, this.runUnary(7, 'reciprocal'));
		this.assertEquals(0.reciprocal, this.runUnary(0, 'reciprocal'));
		this.assertEquals(true.not, this.runUnary(true, 'not'));
		this.assertEquals(nil.isNil, this.runUnary(nil, 'isNil'));
		this.assertEquals(3.notNil, this.runUnary(3, 'notNil'));
	}

	test_folded_char_and_symbol_operations {
		this.assertEquals($a.ascii, this.runUnary($a, 'ascii'));
		this.assertEquals($a < $b, this.runOp($a, '<', $b));
		this.assertEquals(\abc.size, this.runUnary(\abc, 'size'));
	}

	test_folding_does_not_change_the_class {
		this.assertEquals((2 + 3).class, Integer);
		this.assertEquals((2 + 3.0).class, Float);
		this.assertEquals((4 / 2).class, this.runOp(4, '/', 2).class);
		this.assertEquals((7 div: 2).class, Integer);
	}

	test_folding_inside_inlined_code {
		var x = 0;
		if (1 + 1 == 2) { x = 1 } { x = 2 };
		this.assertEquals(x, 1);
		x = 0;
		(2 * 2).do { x = x + 1 };
		this.assertEquals(x, 4);
		this.assertEquals(switch (1 + 1, 2, { \two }, { \other }), \two);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Adverbs
	////////////////////////////////////////////////////////////////////////////////

	test_adverbs_on_arrays {
		this.assertEquals([1, 2] + [10, 20], [11, 22]);
		this.assertEquals([1, 2] +.t [10, 20], [[11, 21], [12, 22]], "table");
		this.assertEquals([1, 2] +.x [10, 20], [11, 21, 12, 22], "cross");
		this.assertEquals([1, 2, 3] +.s [10, 20], [11, 22], "shortest");
		this.assertEquals([1, 2, 3, 4] + [10, 20, 30], [11, 22, 33, 14], "wrapped by default");
		this.assertEquals([1, 2, 3, 4] +.f [10, 20, 30], this.foldedSum([1, 2, 3, 4], [10, 20, 30]), "folded");
	}

	foldedSum { |a, b| ^a.collect { |x, i| x + b.foldAt(i) } }

	test_adverb_reaches_a_method {
		var o = TestCompilerOperatorsReceiver();
		this.assertEquals(o + 1, [\plus, 1, nil]);
		this.assertEquals(o +.t 1, [\plus, 1, \t]);
		this.assertEquals(o +.x 1, [\plus, 1, \x]);
		this.assertEquals(o +.3 1, [\plus, 1, 3], "an integer adverb");
	}

	test_keyword_operator_with_an_adverb {
		// The old compiler's debug build asserts on this (an OpTrinaryMath out of range).
		var o = TestCompilerOperatorsReceiver();
		this.assertEquals(o max:.t 1, [\max, 1, \t]);
	}

	test_adverb_with_numbers {
		this.assertEquals(1 +.t 2, 3);
		this.assertEquals(1 +.x [10, 20], [11, 21]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Operators with their own opcodes, sent to objects that define them
	////////////////////////////////////////////////////////////////////////////////

	test_operators_sent_to_an_object_that_defines_them {
		var o = TestCompilerOperatorsReceiver();
		this.assertEquals(o + 2, [\plus, 2, nil]);
		this.assertEquals(o - 2, [\minus, 2]);
		this.assertEquals(o * 2, [\times, 2]);
		this.assertEquals(o < 2, [\less, 2]);
		this.assertEquals(o == 2, [\equals, 2]);
		this.assertEquals(o ++ 2, [\concat, 2]);
		this.assertEquals(o.neg, \neg);
		this.assertEquals(o.squared, \squared);
		this.assertEquals(o.isNil, \isNil);
		this.assertEquals(o.value, \value);
		this.assertEquals(o.value(1, 2), [\value, 1, 2]);
		this.assertEquals(o.(1, 2), [\value, 1, 2]);
	}

	test_identity_is_not_sent {
		// === and !== are tested by the VM, not sent.
		var o = TestCompilerOperatorsReceiver();
		this.assertEquals(o === o, true);
		this.assertEquals(o !== o, false);
		this.assertEquals(o === 2, false);
	}

	test_operators_with_the_wrong_argument_count {
		var o = TestCompilerOperatorsReceiver();
		this.assertEquals(o.perform('+', 1), [\plus, 1, nil]);
		this.assertEquals(o.perform('+', 1, \t), [\plus, 1, \t]);
		this.assertEquals(o.neg(1), \neg, "an extra argument to a unary operator is dropped");
		this.assertEquals(3.neg(1), -3);
	}

	test_special_selectors_as_normal_messages {
		this.assertEquals(1.perform('+', 2), 3);
		this.assertEquals(3.perform('neg'), -3);
		this.assertEquals([1, 2].perform('++', [3]), [1, 2, 3]);
	}

	test_operator_on_nil_is_an_error {
		this.assertException({ nil + 1 }, Error);
		this.assertException({ var x; x.neg }, Error);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Literals
	////////////////////////////////////////////////////////////////////////////////

	test_number_literals {
		this.assertEquals(0xFF, 255);
		this.assertEquals(2r1010, 10);
		this.assertEquals(16rFF, 255);
		this.assertEquals(36rZZ, 1295);
		this.assertEquals(16r1.8, 1.5);
		this.assertEquals(1e3, 1000.0);
		this.assertEquals(1e3.class, Float);
		this.assertEquals(1.5e-1, 0.15);
		this.assertEquals(pi, 3.141592653589793);
		this.assertEquals(2pi, 2 * pi);
		this.assertEquals(0.5pi, 0.5 * pi);
		this.assertEquals(inf.class, Float);
		this.assertEquals(inf > 1e300, true);
		this.assertEquals(-inf < -1e300, true);
	}

	test_character_literals {
		this.assertEquals($a.ascii, 97);
		this.assertEquals($ .ascii, 32);
		this.assertEquals($\t.ascii, 9);
		this.assertEquals($\n.ascii, 10);
		this.assertEquals($\\.ascii, 92);
		this.assertEquals($$.ascii, 36);
	}

	test_string_and_symbol_literals {
		this.assertEquals("a\tb".size, 3);
		this.assertEquals("say \"hi\"".size, 8);
		this.assertEquals("a" "b", "ab", "adjacent strings are joined");
		this.assertEquals('with space'.asString, "with space");
		this.assertEquals(\sym.class, Symbol);
		this.assertEquals('sym', \sym);
		this.assertEquals("".size, 0);
	}

	test_two_equal_string_literals {
		var a = "same", b = "same";
		this.assertEquals(a, b);
		this.assertEquals(a === b, this.sameLiteralTwice, "whether the compiler shares them, here and in a method");
	}

	sameLiteralTwice { var a = "same", b = "same"; ^a === b }

	test_literal_arrays {
		this.assertEquals(#[1, 2, 3], [1, 2, 3]);
		this.assertEquals(#[a, b], [\a, \b], "names in a literal array are symbols");
		this.assertEquals(#[1, [2, 3]], [1, [2, 3]]);
		this.assertEquals(#[1.5, $c, "s", \sym, nil, true], [1.5, $c, "s", \sym, nil, true]);
		this.assertEquals(#[].size, 0);
	}

	test_literal_array_is_immutable {
		var caught = false;
		try { #[1, 2].put(0, 5) } { caught = true };
		this.assert(caught, "a literal array cannot be changed");
	}

	test_literal_array_is_one_object {
		var f = { #[1, 2] };
		this.assert(f.value === f.value, "the same literal each time the code runs");
	}

	test_array_expression_is_a_new_object_each_time {
		var f = { [1, 2] };
		this.assert(f.value !== f.value);
		this.assertEquals(f.value.put(0, 5), [5, 2], "and it can be changed");
	}

	test_collection_literals {
		this.assertEquals(Array[1, 2], [1, 2]);
		this.assertEquals(List[1, 2].class, List);
		this.assertEquals(List[1, 2].asArray, [1, 2]);
		this.assertEquals((a: 1, b: 2)[\b], 2);
		this.assertEquals((a: 1).class, Event);
		this.assertEquals(Set[1, 1, 2].size, 2);
	}

	test_closed_function {
		this.assertEquals(#{ 1 + 1 }.value, 2);
		this.assertEquals(#{ |a| a * 2 }.value(4), 8);
	}

	test_association_and_point {
		this.assertEquals((\a -> 1).key, \a);
		this.assertEquals((\a -> 1).value, 1);
		this.assertEquals((1 @ 2).class, Point);
		this.assertEquals((1 @ 2).y, 2);
		this.assertEquals(Point(1, 2), 1 @ 2, "class call syntax");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Indexing, series and setters
	////////////////////////////////////////////////////////////////////////////////

	test_indexing {
		var a = [10, 20, 30, 40, 50];
		this.assertEquals(a[0], 10);
		this.assertEquals(a[4], 50);
		this.assertEquals(a[5], nil, "out of range is nil");
		this.assertEquals([[1, 2], [3, 4]][1][0], 3);
	}

	test_series_indexing {
		var a = [10, 20, 30, 40, 50];
		this.assertEquals(a[1..3], [20, 30, 40]);
		this.assertEquals(a[..1], [10, 20]);
		this.assertEquals(a[3..], [40, 50]);
		this.assertEquals(a[0, 2..], [10, 30, 50]);
		this.assertEquals(a[0, 2..3], [10, 30]);
	}

	test_index_assignment {
		var a = [1, 2, 3].copy, r;
		a[0] = 10;
		this.assertEquals(a, [10, 2, 3]);
		r = (a[1] = 20);
		this.assertEquals(a, [10, 20, 3]);
		this.assertEquals(r, a.put(1, 20), "an index assignment has the value of put");
	}

	test_series_assignment {
		var a = [1, 2, 3, 4, 5].copy;
		a[1..3] = 0;
		this.assertEquals(a, [1, 0, 0, 0, 5]);
	}

	test_setter_syntax {
		var o = TestCompilerOperatorsReceiver(), r;
		o.field = 5;
		this.assertEquals(o.field, 5);
		o.field_(6);
		this.assertEquals(o.field, 6);
		r = (o.field = 7);
		// The receiver class overrides ==, so compare by identity.
		this.assert(r === o.field_(7), "a setter assignment has the value of the setter");
	}

	test_setter_on_an_expression {
		var holder = [TestCompilerOperatorsReceiver()];
		holder[0].field = 9;
		this.assertEquals(holder[0].field, 9);
	}

	test_series_expressions {
		this.assertEquals((1..5), [1, 2, 3, 4, 5]);
		this.assertEquals((5..1), [5, 4, 3, 2, 1]);
		this.assertEquals((1, 3..9), [1, 3, 5, 7, 9]);
		this.assertEquals((1.0, 1.5..3.0), [1.0, 1.5, 2.0, 2.5, 3.0]);
		this.assertEquals((3..3), [3]);
	}

	test_series_with_expressions {
		var a = 2, b = 6;
		this.assertEquals((a..b), [2, 3, 4, 5, 6]);
		this.assertEquals((a, a + 2..b), [2, 4, 6]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Calls, splats and multiple assignment
	////////////////////////////////////////////////////////////////////////////////

	test_call_syntaxes_are_the_same {
		var f = { |a, b| [a, b] };
		this.assertEquals(max(1, 2), 1.max(2));
		this.assertEquals(f.value(1, 2), f.(1, 2));
		this.assertEquals(Point.new(1, 2), Point(1, 2));
		this.assertEquals(this.take(1, 2, 3), take(this, 1, 2, 3));
	}

	test_trailing_function_argument {
		var r = [1, 2, 3].collect { |x| x * 2 };
		this.assertEquals(r, [2, 4, 6]);
		this.assertEquals([1, 2, 3].inject(0) { |a, b| a + b }, 6, "after a parenthesised argument");
		this.assertEquals([3, 1, 2].sort({ |a, b| a > b }), [3, 2, 1]);
	}

	test_splat {
		var f = { |a, b, c| [a, b, c] }, args = [1, 2];
		this.assertEquals(f.(*args), [1, 2, nil]);
		this.assertEquals(f.(0, *args), [0, 1, 2]);
		this.assertEquals(this.take(*[1, 2, 3]), [1, 2, 3]);
		this.assertEquals(Array.with(*[1, 2]), [1, 2]);
		this.assertEquals(f.(*[]), [nil, nil, nil]);
	}

	test_splat_of_a_non_array {
		var f = { |a, b| [a, b] };
		this.assertEquals(f.(*5), [5, nil], "a single value is spread as itself");
	}

	test_multiple_assignment {
		var a, b, c, r;
		#a, b = [1, 2];
		this.assertEquals([a, b], [1, 2]);
		#a, b = [1, 2, 3];
		this.assertEquals([a, b], [1, 2], "extra values are dropped");
		#a, b ... c = [1, 2, 3, 4];
		this.assertEquals([a, b, c], [1, 2, [3, 4]]);
		r = (#a, b = [5, 6]);
		this.assertEquals(r, [5, 6], "the value is the right hand side");
	}

	test_multiple_assignment_too_few {
		var a, b;
		#a, b = [1];
		this.assertEquals([a, b], [1, nil]);
	}

	test_multiple_assignment_swap {
		var a = 1, b = 2;
		#a, b = [b, a];
		this.assertEquals([a, b], [2, 1]);
	}

	test_this_method_and_function {
		var f = { thisFunction };
		this.assertEquals(thisMethod.name, \test_this_method_and_function);
		this.assertEquals(f.value.class, Function);
		// thisFunction is a new Function object for the running code, so compare their definitions.
		this.assert(f.value.def === f.def, "thisFunction is the function running");
		this.assertEquals(thisThread.class.isKindOf(Class), true);
	}
}

TestCompilerOperatorsReceiver {
	var <>field;

	+ { |other, adverb| ^[\plus, other, adverb] }
	- { |other| ^[\minus, other] }
	* { |other| ^[\times, other] }
	< { |other| ^[\less, other] }
	== { |other| ^[\equals, other] }
	++ { |other| ^[\concat, other] }
	max { |other, adverb| ^[\max, other, adverb] }
	neg { ^\neg }
	squared { ^\squared }
	isNil { ^\isNil }
	value { |...args| ^if (args.isEmpty) { \value } { [\value] ++ args } }
	hash { ^0 }
}
