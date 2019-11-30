
TestPprotect : UnitTest {

	test_reset_exceptionHandler_on_error {
		var success, cond = Condition.new,
		// Note that this must be a Stream, not a Pattern (x.asStream --> x).
		// If it's a pattern, then we don't have access to the routine
		// to check its exceptionHandler below.
		routine = Routine { 0.0.wait; 1 + nil },  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				{ cond.unhang }.defer;
			}
		).asStream;
		// Note that it is necessary to do this asynchronously!
		// Otherwise "routine"'s error will halt the entire test suite.
		stream.play;
		cond.hang;
		this.assert(success, "Pprotect should clear the stream's exceptionHandler");
	}

	test_stream_can_be_restarted_after_error {
		var pat, stream, hasRun = false;
		pat = Pprotect(
			Prout {
				0.01.yield;
				hasRun = true;
				Error("dummy error").throw
			},
			{ stream.streamError }
		);
		stream = pat.play;
		0.02.wait;
		hasRun = false;
		stream.reset;
		stream.play;
		0.02.wait;
		this.assert(hasRun, "stream should be resettable after an error");
	}

	test_task_proxy_play_after_error {
		var proxy, redefine, hasRun;
		proxy = TaskProxy.new;
		proxy.quant = 0;
		proxy.play;

		redefine = {
			proxy.source = {
				0.01.wait;
				hasRun = true;
				Error("dummy error").throw
			}
		};
		hasRun = false;
		redefine.value;
		0.02.wait;
		hasRun = false;
		redefine.value;
		0.02.wait;
		this.assert(hasRun, "task proxy should play again after an error");
	}

	/*

	// this stops the test run, calling test_nested_instances twice, works.
	// keeping ths commented out for now

	test_nested_instances {

		var stream, innerHasBeenCalled = false, outerHasbeenCalled = false;

		stream = Pprotect(
			Pprotect(
				Prout {
					Error("dummy error").throw
				}, {
					innerHasBeenCalled = true
				}
			),
			{ outerHasbeenCalled = true }
		).asStream;

		stream.next;

		this.assert(innerHasBeenCalled, "When nesting Pprotect, inner functions should be called");
		this.assert(outerHasbeenCalled, "When nesting Pprotect, outer functions should be called");
	}
	*/
}
