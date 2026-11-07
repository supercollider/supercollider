//# VERSION 3.16
// Frame lifetime. A frame is reference counted, and freed when its function returns, unless the compiler marks it
// as needing a heap context (needsHeapContext), in which case the garbage collector owns it. The compiler must mark
// every frame that can outlive its call: one a closure refers to, directly or through a closure nested in it, and
// one a closure reaches through `this`, an instance variable or `^`. A frame wrongly left reference counted is freed
// while something still points at it, and its memory is reused, which shows up as wrong values or a crash.
//
// Each test makes frames that outlive their call, forces the collector to run (churn), then uses them.
// Variables of nested functions are declared first in their body so the same text also compiles as Legacy.

TestCompilerFrames : UnitTest {
	var value = 10;

	// Allocates enough to run the incremental collector through several cycles, and to reuse freed frames.
	churn {
		var keep;
		20.do { |i|
			keep = Array.fill(5000, { |j| [i, j, i + j] });
			this.frameReuser(i);
		};
		^keep.size
	}

	// Calls that make and free many reference counted frames, so a freed frame's memory is reused by another frame.
	frameReuser { |n|
		var a = n, b = n + 1, c = n + 2, d = n + 3, e = n + 4, f = n + 5, g = n + 6, h = n + 7;
		200.do { |i| this.frameFiller(i, a, b, c) };
		^[a, b, c, d, e, f, g, h].sum
	}

	frameFiller { |w, x, y, z| var p = w * 1000, q = x * 1000, r = y * 1000, s = z * 1000; ^p + q + r + s }

	////////////////////////////////////////////////////////////////////////////////
	// Methods that return closures over their frame
	////////////////////////////////////////////////////////////////////////////////

	adders { ^(1..10).collect { |i| { |x| x + i } } }

	// The closure is two functions deep, the frame it reads is the method's.
	twoDeep { |a| var b = a * 2; ^{ { a + b } } }

	// The middle function reads nothing, so only the inner closure's reach keeps the method's frame.
	threeDeep { |a| ^{ { { a } } } }

	counter { var count = 0; ^[{ count = count + 1 }, { count }] }

	readsThis { ^{ this } }
	readsInstanceVariable { ^{ value } }
	readsInstanceVariableTwoDeep { ^{ { value } } }
	writesInstanceVariable { ^{ |v| value = v } }

	// Variables in an inlined body. A captured one must not share a slot between passes.
	capturedInLoopBody {
		var functions = [], i = 0;
		while { i < 5 } {
			var j = i * 2;
			functions = functions.add({ j });
			i = i + 1;
		};
		^functions
	}

	capturedInIfBody { |x|
		if (x) {
			var y = 7, z = 8;
			^[{ y }, { z = z + 1 }]
		};
		^nil
	}

	notCapturedInIfBody { |x|
		var f = { 3 };
		if (x) { var y = 7; ^[f, y] };
		^nil
	}

	// A closure and a Routine share a variable of the method.
	sharedWithRoutine {
		var shared = 0;
		var routine = Routine { loop { shared = shared + 1; shared.yield } };
		^[routine, { shared }]
	}

	// thisFunction's closure has the frame around the function as its context. In a method's own body that is the
	// method's frame, which is reference counted unless something else makes it a heap context, so calling the
	// answer of `foo { ^thisFunction }` after foo returned hangs the VM (compiler-tests finding 9). Not tested here,
	// as the hang would stop the whole class.

	recursive { var base = 100; ^{ |n| if (n <= 0) { base } { n + thisFunction.(n - 1) } } }

	// A closure made by a function frame, which itself has finished.
	fromFunctionFrame { ^{ |a| var b = a + 1; { a + b } }.value(5) }

	// A function frame that makes a closure, called many times, every closure kept.
	manyFunctionFrames { ^(1..50).collect { |i| { |a| var b = a * 3; { a + b + i } }.value(i) } }

	// A non-local return through reference counted frames, while the collector runs in them.
	returnThrough { |f| ^this.middle(f) + 1000 }
	middle { |f| var x = Array.fill(100, { |i| i }); this.churn; f.value(x); ^x.size }
	callsReturnThrough { |n| this.returnThrough({ |x| ^x.sum + n }); ^\notReached }

	deadReturn { ^{ |x| ^x } }

	routineWithLocals {
		^Routine {
			var a = Array.fill(100, { |i| [i] }), b = "a string";
			a.yield;
			this.churn;
			(a.collect(_.first).sum.asString ++ b).yield;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// Tests
	////////////////////////////////////////////////////////////////////////////////

	test_closures_made_in_a_collect_survive_a_collection {
		var adders = this.adders;
		this.churn;
		this.assertEquals(adders.collect { |f| f.value(100) }, (101..110));
	}

	test_closures_two_and_three_deep_keep_the_method_frame {
		var two = this.twoDeep(3).value, three = this.threeDeep(4).value.value;
		this.churn;
		this.assertEquals(two.value, 9);
		this.assertEquals(three.value, 4);
	}

	test_closures_sharing_a_frame_see_each_others_writes {
		var increment, read;
		#increment, read = this.counter;
		3.do { increment.value };
		this.churn;
		increment.value;
		this.assertEquals(read.value, 4);
	}

	test_closures_over_this_and_instance_variables_survive_a_collection {
		var self = this.readsThis, iv = this.readsInstanceVariable, iv2 = this.readsInstanceVariableTwoDeep;
		var setter = this.writesInstanceVariable;
		this.churn;
		this.assert(self.value === this, "this");
		this.assertEquals(iv.value, 10, "instance variable");
		this.assertEquals(iv2.value.value, 10, "instance variable, two deep");
		setter.value(11);
		this.churn;
		this.assertEquals(value, 11, "write through the closure");
		this.assertEquals(iv.value, 11, "read through the closure");
	}

	test_closures_over_inlined_body_variables_are_fresh_each_pass {
		var functions = this.capturedInLoopBody;
		this.churn;
		this.assertEquals(functions.collect(_.value), [0, 2, 4, 6, 8]);
	}

	test_closures_over_an_if_body_survive_a_collection {
		var read, increment;
		#read, increment = this.capturedInIfBody(true);
		this.churn;
		increment.value;
		this.assertEquals([read.value, increment.value], [7, 10]);
	}

	test_variable_not_captured_in_an_if_body {
		var f, y;
		#f, y = this.notCapturedInIfBody(true);
		this.churn;
		this.assertEquals([f.value, y], [3, 7]);
	}

	test_frame_shared_by_a_closure_and_a_routine {
		var routine, read;
		#routine, read = this.sharedWithRoutine;
		this.assertEquals(routine.next, 1);
		this.churn;
		this.assertEquals(routine.next, 2);
		this.assertEquals(read.value, 2);
		this.churn;
		this.assertEquals(routine.next, 3);
		this.assertEquals(read.value, 3);
	}

	test_this_function_recursion_after_its_method_returned {
		var f = this.recursive;
		this.churn;
		this.assertEquals(f.value(4), 110);
	}

	test_closure_from_a_finished_function_frame {
		var f = this.fromFunctionFrame, many = this.manyFunctionFrames;
		this.churn;
		this.assertEquals(f.value, 11);
		this.assertEquals(many.collect(_.value), (1..50).collect { |i| i * 5 });
	}

	test_non_local_return_through_frames_during_collection {
		var results = (1..20).collect { |n| this.callsReturnThrough(n) };
		this.churn;
		this.assertEquals(results, (1..20).collect { |n| 4950 + n });
	}

	test_dead_return_ends_its_thread_after_a_collection {
		var f = this.deadReturn, result = \notSet, after = \notSet, routine;
		this.churn;
		routine = Routine { after = \started; f.value(1); after = \continued; \end };
		result = routine.next;
		this.assertEquals(after, \started, "the thread ends at the ^");
		this.assertEquals(result, 1, "the routine answers the value of the ^");
		this.assertEquals(routine.next, nil, "the routine has ended");
		// The thread that ran the test is still usable.
		this.assertEquals(this.adders.first.value(1), 2);
	}

	test_suspended_routine_keeps_its_locals {
		var r = this.routineWithLocals, first;
		first = r.next;
		this.churn;
		this.assertEquals(first.size, 100);
		this.assertEquals(r.next, "4950a string");
	}

	test_routine_locals_without_closures {
		// The routine's function has no nested closure, so its frame is reference counted while it is suspended.
		var r = Routine { var a = Array.fill(50, { |i| i }), sum = 0; a.do { |x| sum = sum + x; sum.yield } };
		var values = List.new;
		10.do { values.add(r.next); this.frameReuser(1) };
		this.churn;
		40.do { values.add(r.next) };
		this.assertEquals(values.last, 1225);
		this.assertEquals(values.size, 50);
	}

	test_closures_in_nested_loops_after_collection {
		var functions = List.new;
		4.do { |i| 4.do { |j| var k = i * 4 + j; functions.add({ k }) } };
		this.churn;
		this.assertEquals(functions.collect(_.value).asArray, (0..15));
	}

	test_closures_in_a_while_with_hoisted_and_captured_variables {
		var functions = List.new, i = 0;
		while { i < 6 } {
			var notCaptured = i * 10, captured = i;
			functions.add({ captured + 100 });
			notCaptured = notCaptured + 1;
			i = i + 1;
		};
		this.churn;
		this.assertEquals(functions.collect(_.value).asArray, (100..105));
	}

	test_interpreted_closures_survive_a_collection {
		var f = thisProcess.interpreter.interpret("(1..5).collect { |i| var j = i * i; { j + i } }");
		this.churn;
		this.assertEquals(f.collect(_.value), [2, 6, 12, 20, 30]);
	}

	test_interpreted_this_function_survives_a_collection {
		var f = thisProcess.interpreter.interpret("{ |n| var m = n * 2; { thisFunction } }.value(3)");
		var inner;
		this.churn;
		inner = f.value;
		this.churn;
		this.assert(inner.isKindOf(Function), "thisFunction is a Function");
	}

	test_many_frames_alive_at_once {
		// Deep recursion, each level keeping a closure over its own frame.
		var make = { |n| if (n == 0) { [] } { [{ n }] ++ make.(n - 1) } };
		var functions = make.(200);
		this.churn;
		this.assertEquals(functions.collect(_.value), (200..1));
	}
}
