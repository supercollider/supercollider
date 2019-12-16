TestPprotect : UnitTest {

	test_resetExceptionHandler_onError {
		var routine, stream, success, condition = Condition.new;
		var timeout = 0.1;
		// Note that this must be a Stream, not a Pattern (x.asStream --> x).
		// If it's a pattern, then we don't have access to the routine
		// to check its exceptionHandler below.
		routine = Routine { 0.0.wait; 1 + nil };  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				condition.unhang;
			}
		).asStream;
		// Note that it is necessary to do this asynchronously!
		// Otherwise "routine"'s error will halt the entire test suite.
		stream.play;
		condition.hang(timeout);
		this.assert(success, "Pprotect should clear the stream's exceptionHandler");
	}

	test_stream_can_be_restarted_after_error {
		var pat, stream, hasRun = false;
		var condition = Condition.new;
		var timeout = 0.1;

		pat = Pprotect(
			Prout {
				0.01.yield;
				hasRun = true;
				condition.unhang;
				Error("dummy error").throw
			},
			{ stream.streamError }
		);
		stream = pat.play;
		condition.hang(timeout);
		hasRun = false;
		stream.reset;
		stream.play;
		condition.hang(timeout);
		this.assert(hasRun, "stream should be resettable after an error");
	}

	test_task_proxy_play_after_error {
		var proxy, redefine, hasRun;
		var condition = Condition.new;
		var timeout = 0.1;

		proxy = TaskProxy.new;
		proxy.quant = 0;
		proxy.play;

		redefine = {
			proxy.source = {
				0.01.wait;
				hasRun = true;
				condition.unhang;
				Error("dummy error").throw
			}
		};
		hasRun = false;
		redefine.value;
		condition.hang(timeout);
		hasRun = false;
		redefine.value;
		condition.hang(timeout);
		this.assert(hasRun, "task proxy should play again after an error");
	}

	test_nested_instances {
		var innerHasBeenCalled = false, outerHasBeenCalled = false;
		var condition = Condition.new;
		var timeout = 0.1;

		fork {
			var stream;
			stream = Pprotect(
				Pprotect(
					Prout {
						Error("dummy error").throw
					}, {
						condition.unhang;
						innerHasBeenCalled = true
					}
				),
				{
					condition.unhang;
					outerHasBeenCalled = true
				}
			).asStream;

			stream.next;

		};
		condition.hang(timeout);

		this.assert(innerHasBeenCalled, "When nesting Pprotect, inner functions should be called");
		this.assert(outerHasBeenCalled, "When nesting Pprotect, outer functions should be called");
	}

}

