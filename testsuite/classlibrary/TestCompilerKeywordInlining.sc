//# VERSION 3.16
// The compiler inlines if, while, and, or, ??, !?, ?, switch, case and the loops only when they are called with
// positional arguments. With a keyword argument, or a splat (`*`), it sends the message to the class library method.
// So every construct here is written both ways, and both must answer the same value and do the same side effects in
// the same order. TestKwargs has `if(trueFunc:)` and `and(that:)`.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerKeywordInlining : UnitTest {
	var log;

	setUp { log = List.new }
	mark { |x| log.add(x); ^x }
	takeLog { var l = log.asArray; log = List.new; ^l }

	// Runs both forms, each with a clean log, and checks the values and the logs match.
	same { |inlined, sent, message|
		var a, b, logA, logB;
		a = inlined.value; logA = this.takeLog;
		b = sent.value; logB = this.takeLog;
		this.assertEquals(b, a, message ++ ": value");
		this.assertEquals(logB, logA, message ++ ": side effects");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Methods under test
	////////////////////////////////////////////////////////////////////////////////

	ifReturnInlined { |x| if (x) { ^\fromIf }; ^\after }
	ifReturnSent { |x| x.if(trueFunc: { ^\fromIf }); ^\after }
	whileReturnInlined { var i = 0; while { i < 10 } { i = i + 1; if (i == 3) { ^i } }; ^\after }
	whileReturnSent { var i = 0; { i < 10 }.while(body: { i = i + 1; if (i == 3) { ^i } }); ^\after }
	doReturnSent { 10.do(function: { |i| if (i == 4) { ^i } }); ^\after }
	switchReturnSent { |x| x.switch(*[1, { ^\one }, 2, { ^\two }]); ^\after }

	////////////////////////////////////////////////////////////////////////////////
	// Conditionals
	////////////////////////////////////////////////////////////////////////////////

	test_if {
		[true, false].do { |c|
			this.same({ if (c) { this.mark(\t) } { this.mark(\f) } },
				{ c.if(trueFunc: { this.mark(\t) }, falseFunc: { this.mark(\f) }) }, "if %, both keywords".format(c));
			this.same({ if (c) { this.mark(\t) } { this.mark(\f) } },
				{ c.if({ this.mark(\t) }, falseFunc: { this.mark(\f) }) }, "if %, one keyword".format(c));
			this.same({ if (c) { this.mark(\t) } { this.mark(\f) } },
				{ c.if(falseFunc: { this.mark(\f) }, trueFunc: { this.mark(\t) }) }, "if %, keywords reversed".format(c));
			this.same({ if (c) { this.mark(\t) } }, { c.if(trueFunc: { this.mark(\t) }) }, "if % without else".format(c));
			this.same({ if (c) { this.mark(\t) } { this.mark(\f) } },
				{ c.if(*[{ this.mark(\t) }, { this.mark(\f) }]) }, "if %, splat".format(c));
		}
	}

	test_if_with_values_not_functions {
		this.same({ if (true, 1, 2) }, { true.if(trueFunc: 1, falseFunc: 2) }, "if with values");
		this.same({ if (false, 1, 2) }, { false.if(trueFunc: 1, falseFunc: 2) }, "if false with values");
	}

	test_and_or {
		[true, false].do { |c|
			this.same({ c and: { this.mark(\rhs) } }, { c.and(that: { this.mark(\rhs) }) }, "and %".format(c));
			this.same({ c or: { this.mark(\rhs) } }, { c.or(that: { this.mark(\rhs) }) }, "or %".format(c));
			this.same({ c and: { this.mark(\rhs) } }, { c.and(*[{ this.mark(\rhs) }]) }, "and %, splat".format(c));
		}
	}

	test_nil_tests {
		[nil, 1].do { |x|
			this.same({ x ?? { this.mark(\rhs) } }, { (??)(x, obj: { this.mark(\rhs) }) }, "?? %".format(x));
			this.same({ x !? { |v| this.mark([\rhs, v]) } }, { (!?)(x, obj: { |v| this.mark([\rhs, v]) }) }, "!? %".format(x));
			this.same({ x ? 2 }, { (?)(x, obj: 2) }, "? %".format(x));
			this.same({ x.isNil }, { x.isNil(*[]) }, "isNil %".format(x));
			this.same({ x.notNil }, { x.notNil(*[]) }, "notNil %".format(x));
		}
	}

	test_switch {
		[1, 2, 3, \x].do { |x|
			this.same({ switch (x) { 1 } { this.mark(\one) } { 2 } { this.mark(\two) } { this.mark(\default) } },
				{ x.switch(*[1, { this.mark(\one) }, 2, { this.mark(\two) }, { this.mark(\default) }]) },
				"switch %".format(x));
			this.same({ switch (x, 1, { this.mark(\one) }, 2, { this.mark(\two) }) },
				{ x.switch(*[1, { this.mark(\one) }, 2, { this.mark(\two) }]) }, "switch % without default".format(x));
		}
	}

	test_case_evaluates_tests_in_order {
		// The inlined form must run the same tests as the sent form.
		[1, 2, 3].do { |x|
			this.same({ case { this.mark(\test1); x == 1 } { this.mark(\one) } { this.mark(\test2); x == 2 } { this.mark(\two) } },
				{ { this.mark(\test1); x == 1 }.case(*[{ this.mark(\one) }, { this.mark(\test2); x == 2 }, { this.mark(\two) }]) },
				"case % order".format(x));
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// Loops
	////////////////////////////////////////////////////////////////////////////////

	test_while {
		// The side effects match. The values do not: inlined, while answers nil (as the old compiler's did), sent,
		// Function:while falls off its end and answers the receiver.
		var i, w = { |inlined, sent, message| this.same({ inlined.value; nil }, { sent.value; nil }, message) };
		w.({ i = 0; while { i < 3 } { this.mark(i); i = i + 1 } },
			{ i = 0; { i < 3 }.while(body: { this.mark(i); i = i + 1 }) }, "while");
		w.({ i = 0; while { this.mark(\test); i < 2 } { i = i + 1 } },
			{ i = 0; { this.mark(\test); i < 2 }.while(*[{ i = i + 1 }]) }, "while, splat");
		w.({ i = 0; while { i = i + 1; this.mark(i); i < 3 } },
			{ i = 0; { i = i + 1; this.mark(i); i < 3 }.while(*[]) }, "while without a body");
		i = 0;
		this.assertEquals(while { i < 1 } { i = i + 1 }, nil, "inlined while answers nil");
		i = 0;
		this.assertEquals({ i < 1 }.while(body: { i = i + 1 }).class, Function, "Function:while answers the receiver");
	}

	test_integer_loops {
		this.same({ 3.do { |i| this.mark(i) } }, { 3.do(function: { |i| this.mark(i) }) }, "Integer do");
		this.same({ 3.do { |i, j| this.mark([i, j]) } }, { 3.do(function: { |i, j| this.mark([i, j]) }) }, "Integer do, two arguments");
		this.same({ 3.reverseDo { |i| this.mark(i) } }, { 3.reverseDo(function: { |i| this.mark(i) }) }, "Integer reverseDo");
		this.same({ 1.for(3) { |i| this.mark(i) } }, { 1.for(endval: 3, function: { |i| this.mark(i) }) }, "Integer for");
		this.same({ 3.for(1) { |i| this.mark(i) } }, { 3.for(endval: 1, function: { |i| this.mark(i) }) }, "Integer for down");
		this.same({ 1.forBy(7, 2) { |i| this.mark(i) } },
			{ 1.forBy(endval: 7, stepval: 2, function: { |i| this.mark(i) }) }, "Integer forBy");
		this.same({ 1.forBy(7, 2) { |i| this.mark(i) } },
			{ 1.forBy(function: { |i| this.mark(i) }, stepval: 2, endval: 7) }, "Integer forBy, keywords reversed");
		this.same({ 3.do { |i| this.mark(i) } }, { 3.do(*[{ |i| this.mark(i) }]) }, "Integer do, splat");
	}

	test_float_and_series_loops {
		this.same({ 2.5.do { |x| this.mark(x) } }, { 2.5.do(function: { |x| this.mark(x) }) }, "Float do");
		this.same({ 1.0.for(3.0) { |x| this.mark(x) } }, { 1.0.for(endValue: 3.0, function: { |x| this.mark(x) }) }, "Float for");
		this.same({ (1, 3 .. 9).do { |x| this.mark(x) } }, { (1, 3 .. 9).do(function: { |x| this.mark(x) }) }, "series");
		this.same({ 1.forSeries(3, 9) { |x| this.mark(x) } }, { 1.forSeries(second: 3, last: 9, function: { |x| this.mark(x) }) }, "forSeries");
	}

	test_collection_loops {
		var a = [\a, \b, \c], d = (x: 1);
		this.same({ a.do { |x, i| this.mark([x, i]) } }, { a.do(function: { |x, i| this.mark([x, i]) }) }, "Array do");
		this.same({ a.reverseDo { |x, i| this.mark([x, i]) } }, { a.reverseDo(function: { |x, i| this.mark([x, i]) }) }, "Array reverseDo");
		this.same({ d.keysValuesDo { |k, v| this.mark([k, v]) } }, { d.keysValuesDo(function: { |k, v| this.mark([k, v]) }) }, "Dictionary keysValuesDo");
	}

	test_loop_values {
		// What the loop itself answers.
		this.same({ 3.do { } }, { 3.do(function: { }) }, "Integer do answers");
		this.same({ [1].do { } }, { [1].do(function: { }) }, "Array do answers");
		this.same({ 1.for(2) { } }, { 1.for(endval: 2, function: { }) }, "for answers");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Bodies as closures
	////////////////////////////////////////////////////////////////////////////////

	test_body_variables_are_fresh_each_pass {
		// Inlined, the variable is hoisted and set each pass; sent, it is a new frame each pass.
		var inlined = List.new, sent = List.new;
		3.do { |i| var v; inlined.add(v); v = i };
		3.do(function: { |i| var v; sent.add(v); v = i });
		this.assertEquals(inlined.asArray, [nil, nil, nil]);
		this.assertEquals(sent.asArray, [nil, nil, nil]);
	}

	test_closures_made_in_bodies {
		var inlined = List.new, sent = List.new;
		3.do { |i| var v = i * 2; inlined.add({ v }) };
		3.do(function: { |i| var v = i * 2; sent.add({ v }) });
		this.assertEquals(sent.collect(_.value), inlined.collect(_.value));
		this.assertEquals(inlined.collect(_.value).asArray, [0, 2, 4]);
	}

	test_if_body_variables {
		var f = { |c| if (c) { var v = 1; v + 1 } { var w = 2; w * 10 } };
		var g = { |c| c.if(trueFunc: { var v = 1; v + 1 }, falseFunc: { var w = 2; w * 10 }) };
		this.assertEquals([f.(true), f.(false)], [g.(true), g.(false)]);
	}

	test_return_from_sent_bodies {
		this.assertEquals([this.ifReturnSent(true), this.ifReturnSent(false)], [this.ifReturnInlined(true), this.ifReturnInlined(false)]);
		this.assertEquals(this.whileReturnSent, this.whileReturnInlined);
		this.assertEquals(this.doReturnSent, 4);
		this.assertEquals([this.switchReturnSent(1), this.switchReturnSent(2), this.switchReturnSent(3)], [\one, \two, \after]);
	}

	test_return_from_sent_bodies_inside_values {
		var result = [1, this.ifReturnSent(true), this.doReturnSent, 2];
		this.assertEquals(result, [1, \fromIf, 4, 2]);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Interpreted code
	////////////////////////////////////////////////////////////////////////////////

	test_interpreted {
		this.assertEquals("if (true) { 1 } { 2 }".interpret, "true.if(trueFunc: { 1 }, falseFunc: { 2 })".interpret);
		this.assertEquals("var l = []; 3.do { |i| l = l.add(i) }; l".interpret,
			"var l = []; 3.do(function: { |i| l = l.add(i) }); l".interpret);
		this.assertEquals("var i = 0; while { i < 4 } { i = i + 1 }; i".interpret,
			"var i = 0; { i < 4 }.while(body: { i = i + 1 }); i".interpret);
	}
}
