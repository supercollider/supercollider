TestPprotect : UnitTest {

	test_pprotect_can_be_caught {
		var catches = 0;
		var cond = CondVar();

		var pat = Pprotect(Routine{Error("meow").throw}, { 
			catches = catches + 1;
			cond.signalOne;
		 });

		// Pprotect rethrows the exception, so we must catch it again to stop it escaping the test function.
		// This is quite odd because it only runs the handler when an error occurs, so its really like a try that always throws (why?).
		try { pat.play } { 
			catches = catches + 1;
			cond.signalOne;
		};
		this.assert(cond.waitFor(0.2, { catches == 2 }), "catches == 2 and didn't timeout. Pprotect should clear the stream's exceptionHandler" );
	}

	test_stream_can_be_restarted_after_error {
		var pat, stream;
		var condvar = CondVar();
		var wasReset = false;
		var caught = 0;
 
		pat = Pprotect(
			Prout {
				0.01.yield;
				wasReset = true;
				condvar.signalOne;
				Error("dummy error").throw;
			},
			{ 
				stream.streamError; 
				caught = caught + 1; 
			}
		);
 
		try { stream = pat.play } { caught = caught + 1};
		this.assert(condvar.waitFor(0.2, { caught.debug(\caught) == 2 }), "TIMEOUT or caught should be 1");
 
		wasReset = false;
		stream.reset;
		try { stream.play } { caught = caught + 1};
		this.assert(condvar.waitFor(0.2, {caught.debug(\caught) == 4}), "TIMEOUT or caught shoudl be 2");
 
		this.assertEquals(wasReset, true, "stream should be resettable after an error");
	}
 
	test_task_proxy_play_after_error {
		var proxy, redefine, hasRun;
		var condvar = CondVar();
		var didPlay = false;
 
		proxy = TaskProxy.new;
		proxy.quant = 0;
		try { proxy.play };
 
		redefine = {
			proxy.source = {
				0.01.wait;
				didPlay = true;
				condvar.signalOne;
				Error("dummy error").throw
			}
		};
 
		try { redefine.value };
		condvar.waitFor(0.1);
 
		didPlay = false;
		try { redefine.value };
		condvar.waitFor(0.1);

		this.assertEquals(didPlay, true, "task proxy should play again after an error");
	}

	test_nested_instances {
		var condvar = CondVar();
		var innerHasBeenCalled = false, outerHasBeenCalled = false;

		fork {
			var stream;
			try {
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
			}
 
		};
 
		condvar.waitFor(0.1, { innerHasBeenCalled && outerHasBeenCalled });
		this.assert(innerHasBeenCalled, "When nesting Pprotect, inner functions should be called");
		this.assert(outerHasBeenCalled, "When nesting Pprotect, outer functions should be called");
	}
 
}
