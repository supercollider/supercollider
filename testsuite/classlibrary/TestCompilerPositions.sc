//# VERSION 3.16
// Source positions at run time. A frame's position is the source range of the instruction it is at, read through
// its DebugFrame (ipIndex) and the def's byteCodeLocations, which is what backtraces print. After inlining, hoisting
// and folding, a call must still point at its own text. Each test calls `this.where(\tag)`, which answers the
// DebugFrame of its caller, and checks the text there.
// Positions are byte offsets, so multi-byte text comes before some calls: é, ü, →, 音.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerPositions : UnitTest {

	where { |tag, extra| ^this.getBackTrace.caller }
	*where { |tag| ^this.getBackTrace.caller }

	// The source text at the frame's position.
	textAt { |frame|
		var i = frame.ipIndex - 1, locations = frame.functionDef.byteCodeLocations;
		^frame.functionDef.sourceCodeFileOrSnippet[locations[i * 2] .. locations[i * 2 + 1] - 1]
	}

	// The 1 based line of the frame's position in its file.
	lineAt { |frame|
		var i = frame.ipIndex - 1, start = frame.functionDef.byteCodeLocations[i * 2];
		^frame.functionDef.sourceCodeFileOrSnippet[0 .. start - 1].occurrencesOf($\n) + 1
	}

	// The 1 based line of the first match of text in this file.
	lineOf { |text|
		var source = File.readAllString(this.class.filenameSymbol.asString), at = source.find(text);
		^source[0 .. at - 1].occurrencesOf($\n) + 1
	}

	check { |frame, text, message|
		this.assertEquals(this.textAt(frame), text, message ? text);
		this.assertEquals(this.lineAt(frame), this.lineOf(text), (message ? text) ++ ": line");
	}

	////////////////////////////////////////////////////////////////////////////////
	// Methods under test
	////////////////////////////////////////////////////////////////////////////////

	inIf { |x| if (x) { ^this.where(\inIf) } { ^this.where(\inElse) } }
	inHoistedInitialiser { |x| if (x) { var h = this.where(\hoisted); ^h }; ^nil }
	inWhileCondition { var found; while { found = this.where(\whileCondition); false } { }; ^found }
	inWhileBody { var i = 0, found; while { i < 2 } { i = i + 1; found = this.where(\whileBody) }; ^found }
	inLoop { var found; 3.do { |i| if (i == 1) { found = this.where(\inLoop) } }; ^found }
	inArrayLoop { var found; [1, 2].do { |x| found = this.where(\inArrayLoop) }; ^found }
	inCase { |x| ^case { x == 1 } { this.where(\case1) } { x == 2 } { this.where(\case2) } }
	inSwitch { |x| ^switch (x) { 1 } { this.where(\switch1) } { this.where(\switchDefault) } }
	inNilTests { |x| ^[x !? { this.where(\notNil) }, x ?? { this.where(\isNil) }] }
	inAndOr { |x| ^[x and: { this.where(\and) }, x or: { this.where(\or) }] }
	// é → 音: multi-byte characters before these two.
	withFolding { ^[1 + 2 * 3, this.where(\folded, 4 * 5)][1] }
	afterFolding { var k = 2 ** 10; ^this.where(\afterFolding, k) }
	inClosure { ^{ this.where(\inClosure) }.value }
	inNestedClosure { ^{ { this.where(\nested) }.value }.value }
	multiLine {
		^this.where(
			\multiLine)
	}
	inGenerator { ^all {: this.where(\generator), x <- (1..1) }.first }
	inGeneratorGuard { var found; all {: x, x <- (1..1), (found = this.where(\guard)).notNil }; ^found }
	inArgumentOfSend { ^[1, 2].collect { |v| this.where(\argument, v) }.first }

	////////////////////////////////////////////////////////////////////////////////
	// Tests
	////////////////////////////////////////////////////////////////////////////////

	test_plain_call {
		this.check(this.where(\plain), "this.where(\\plain)");
	}

	test_inlined_if {
		this.check(this.inIf(true), "this.where(\\inIf)");
		this.check(this.inIf(false), "this.where(\\inElse)");
	}

	test_hoisted_variable_initialiser {
		this.check(this.inHoistedInitialiser(true), "this.where(\\hoisted)");
	}

	test_inlined_while {
		this.check(this.inWhileCondition, "this.where(\\whileCondition)");
		this.check(this.inWhileBody, "this.where(\\whileBody)");
	}

	test_loops {
		this.check(this.inLoop, "this.where(\\inLoop)");
		this.check(this.inArrayLoop, "this.where(\\inArrayLoop)");
	}

	test_case_switch_and_nil_tests {
		this.check(this.inCase(1), "this.where(\\case1)");
		this.check(this.inCase(2), "this.where(\\case2)");
		this.check(this.inSwitch(1), "this.where(\\switch1)");
		this.check(this.inSwitch(9), "this.where(\\switchDefault)");
		this.check(this.inNilTests(1)[0], "this.where(\\notNil)");
		this.check(this.inNilTests(nil)[1], "this.where(\\isNil)");
		this.check(this.inAndOr(true)[0], "this.where(\\and)");
		this.check(this.inAndOr(false)[1], "this.where(\\or)");
	}

	test_folding {
		this.check(this.withFolding, "this.where(\\folded, 4 * 5)");
		this.check(this.afterFolding, "this.where(\\afterFolding, k)");
	}

	test_closures {
		this.check(this.inClosure, "this.where(\\inClosure)");
		this.check(this.inNestedClosure, "this.where(\\nested)");
	}

	test_call_over_several_lines {
		this.check(this.multiLine, "this.where(\n\t\t\t\\multiLine)");
	}

	test_generators {
		this.check(this.inGenerator, "this.where(\\generator)");
		this.check(this.inGeneratorGuard, "this.where(\\guard)");
	}

	test_closure_passed_to_a_send {
		this.check(this.inArgumentOfSend, "this.where(\\argument, v)");
	}

	test_callers_point_at_their_calls {
		var frame = this.inClosure;
		this.assertEquals(this.textAt(frame.caller), "{ this.where(\\inClosure) }.value", "the method, at the value");
		this.assertEquals(frame.caller.functionDef.name, \inClosure);
	}

	test_exception_backtrace {
		var frame, texts = List.new;
		try { var q = 1; if (q == 1) { nil.notAMethodAnywhere(\fromTest) } } { |e| frame = e.backtrace };
		while { frame.notNil } { texts.add(try { this.textAt(frame) }); frame = frame.caller };
		this.assert(texts.includesEqual("nil.notAMethodAnywhere(\\fromTest)"), "the failing send is in the backtrace");
	}

	test_error_string_has_the_line {
		var frame = this.inIf(true), string = frame.prAsErrorString("", nil, false, true);
		this.assert(string.contains(":" ++ this.lineOf("this.where(\\inIf)") ++ ":"), "the printed location has the line");
	}

	test_interpreted_code {
		var frame = "var q = 1;\nif (q == 1) { TestCompilerPositions.where(\\interpreted) }".interpret;
		this.assertEquals(this.textAt(frame), "TestCompilerPositions.where(\\interpreted)");
		this.assertEquals(this.lineAt(frame), 2);
	}
}
