TestPprotect : UnitTest {

	test_resetExceptionHandler_onError {
		var routine, stream;
		var condvar = CondVar();
		var success = false;

		// Note that this must be a Stream, not a Pattern (x.asStream --> x).
		// If it's a pattern, then we don't have access to the routine
		// to check its exceptionHandler below.
		routine = Routine { 0.0.wait; 1 + nil };  // throw an error
		stream = Pprotect(
			routine,
			{
				success = routine.exceptionHandler.isNil;
				condvar.signalOne;
			}
		).asStream;
		// Note that it is necessary to do this asynchronously!
		// Otherwise "routine"'s error will halt the entire test suite.
		stream.play;
		condvar.waitFor(0.1);

		this.assertEquals(success, true, "Pprotect should clear the stream's exceptionHandler");
	}

	test_stream_can_be_restarted_after_error {
		var pat, stream;
		var condvar = CondVar();
		var wasReset = false;

		pat = Pprotect(
			Prout {
				0.01.yield;
				wasReset = true;
				condvar.signalOne;
				Error("dummy error").throw
			},
			{ stream.streamError }
		);

		stream = pat.play;
		condvar.waitFor(0.1);

		wasReset = false;
		stream.reset;
		stream.play;
		condvar.waitFor(0.1);

		this.assertEquals(wasReset, true, "stream should be resettable after an error");
	}

	test_task_proxy_play_after_error {
		var proxy, redefine, hasRun;
		var condvar = CondVar();
		var didPlay = false;

		proxy = TaskProxy.new;
		proxy.quant = 0;
		proxy.play;

		redefine = {
			proxy.source = {
				0.01.wait;
				didPlay = true;
				condvar.signalOne;
				Error("dummy error").throw
			}
		};

		redefine.value;
		condvar.waitFor(0.1);

		didPlay = false;
		redefine.value;
		condvar.waitFor(0.1);

		this.assertEquals(didPlay, true, "task proxy should play again after an error");
	}

	test_nested_instances {
		var condvar = CondVar();
		var innerHasBeenCalled = false, outerHasBeenCalled = false;

		fork {
			var stream;
			stream = Pprotect(
				Pprotect(
					Prout {
						Error("dummy error").throw
					}, {
						innerHasBeenCalled = true;
						condvar.signalOne;
					}
				),
				{
					outerHasBeenCalled = true;
					condvar.signalOne;
				}
			).asStream;

			stream.next;

		};

		condvar.waitFor(0.1, { innerHasBeenCalled && outerHasBeenCalled });

		this.assert(innerHasBeenCalled, "When nesting Pprotect, inner functions should be called");
		this.assert(outerHasBeenCalled, "When nesting Pprotect, outer functions should be called");
	}

}
