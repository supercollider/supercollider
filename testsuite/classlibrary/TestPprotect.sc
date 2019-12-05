TestPprotect : UnitTest {

	test_reset_exceptionHandler_on_error {
		var success, condition = Condition.new, stream,
		// Note that this must be a Stream, not a Pattern (x.asStream --> x).
		// If it's a pattern, then we don't have access to the routine
		// to check its exceptionHandler below.
		routine = Routine { 0.0.wait; 1 + nil };  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				condition.test = true;
			}
		).asStream;
		// Note that it is necessary to do this asynchronously!
		// Otherwise "routine"'s error will halt the entire test suite.
		stream.play;
		this.wait(condition, maxTime: 0.1);

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
		var proxy, redefine, condition = Condition.new;
		proxy = TaskProxy.new;
		proxy.quant = 0;
		proxy.play;

		redefine = {
			proxy.source = {
				0.01.wait;
				condition.test = true;
				Error("dummy error").throw
			}
		};

		condition.test = false;
		redefine.value;
		this.wait(condition, maxTime: 0.1);
		this.assert(condition.test, "task proxy should play again after an error");
	}

	test_nested_instances {
		var innerHasBeenCalled = false, outerHasBeenCalled = false;
		var condition = Condition.new;

		fork {
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
	}

}

