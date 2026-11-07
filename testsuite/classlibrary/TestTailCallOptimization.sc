TailCallRecursor {
	*depth { |n|
		if(n == 0) {
			^this.prFrameCount
		};
		^this.depth(n - 1)
	}

	// Backtrace taken at the bottom of the recursion.
	*backtraceAtBottom { |n|
		if(n == 0) {
			^this.prBacktrace
		};
		^this.backtraceAtBottom(n - 1)
	}

	*prBacktrace { ^thisThread.getBackTrace }

	*prFrameCount {
		var count = 0, frame = thisThread.getBackTrace;
		while { frame.notNil } { count = count + 1; frame = frame.caller };
		^count
	}
}

TestTailCallOptimization : UnitTest {
	prStackDepthAtBottom { |depth|
		^TailCallRecursor.depth(depth)
	}

	test_compiler_generates_tail_calls {
		this.assert(Process.tailCallOptimize, "the compiler should be generating tail calls");
	}

	test_tail_calls_do_not_grow_the_stack {
		var shallow = this.prStackDepthAtBottom(0);
		var deep = this.prStackDepthAtBottom(50);
		this.assertEquals(deep, shallow, "tail calls should reuse the caller's frame");
	}

	test_can_be_disabled_at_runtime {
		var shallow = this.prStackDepthAtBottom(0);
		var deep;
		Process.tailCallOptimize = false;
		protect {
			this.assertEquals(Process.tailCallOptimize, false, "flag should read back as false");
			deep = this.prStackDepthAtBottom(50);
		} {
			Process.tailCallOptimize = true;
		};
		this.assert(deep > shallow, "without tail calls the stack should grow");
		this.assertEquals(Process.tailCallOptimize, true, "flag should be restored");
	}

	prFrames { |backtrace|
		var frames = List.new;
		while { backtrace.notNil } { frames.add(backtrace); backtrace = backtrace.caller };
		^frames
	}

	test_tail_called_frame_records_the_elided_function {
		var top = TailCallRecursor.backtraceAtBottom(5);
		var recursive = TailCallRecursor.class.findMethod(\backtraceAtBottom);
		this.assertEquals(top.functionDef, TailCallRecursor.class.findMethod(\prBacktrace), "top frame");
		this.assertEquals(top.tailCalledFrom, recursive, "the frame should name the function it replaced");
		this.assertEquals(
			this.prFrames(top).count { |frame| frame.functionDef == recursive }, 0,
			"every recursive frame should have been replaced"
		);
	}

	test_frames_that_are_not_tail_called_have_no_tail_called_from {
		var frame = thisThread.getBackTrace;
		this.assertEquals(frame.functionDef, this.class.findMethod(\test_frames_that_are_not_tail_called_have_no_tail_called_from));
		this.assert(frame.tailCalledFrom.isNil, "ordinary calls should not be marked");
	}

	test_disabled_tail_calls_keep_every_frame_and_no_annotation {
		var top;
		var recursive = TailCallRecursor.class.findMethod(\backtraceAtBottom);
		Process.tailCallOptimize = false;
		protect {
			top = TailCallRecursor.backtraceAtBottom(5);
		} {
			Process.tailCallOptimize = true;
		};
		this.assert(
			this.prFrames(top).select { |frame| frame.functionDef.isKindOf(Method) and: { frame.functionDef.ownerClass == TailCallRecursor.class } }
				.every { |frame| frame.tailCalledFrom.isNil },
			"no frame should be marked"
		);
		this.assertEquals(
			this.prFrames(top).count { |frame| frame.functionDef == recursive }, 6,
			"every recursive frame should remain"
		);
	}
}
