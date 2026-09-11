TestPprotect : UnitTest {

	test_pprotect_can_be_caught {
		var catches = 0;
		var cond = CondVar();

		var pat = Pprotect(Routine{Error("meow").throw}, { 
			catches = catches + 1;
			cond.signalOne;
		 });

		// Pprotect rethrows the exception, so we must catch it again to stop it escaping the test function.
		try(deep:true) { pat.play } { 
			catches = catches + 1;
			cond.signalOne;
		};
		this.assert(cond.waitFor(0.2, { catches == 2 }), "catches == 2 and didn't timeout. Pprotect should catch exceptions, and so should a try around the `p.play`." );
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
 
		try(deep: true) { stream = pat.play } { caught = caught + 1};
		this.assert(condvar.waitFor(0.2, { caught == 2 }), "TIMEOUT or caught should be 1");
 
		wasReset = false;
		stream.reset;
		try(deep: true) { stream.play } { caught = caught + 1};
		this.assert(condvar.waitFor(0.2, {caught == 4}), "TIMEOUT or caught should be 4");
 
		this.assertEquals(wasReset, true, "stream should be resettable after an error");
	}
 
	test_task_proxy_play_after_error {
		var proxy, redefine, hasRun;
		var condvar = CondVar();
		var didPlay = false;
 
		proxy = TaskProxy.new;
		proxy.quant = 0;
		try(deep: true) { proxy.play };
 
		redefine = {
			proxy.source = {
				0.01.wait;
				didPlay = true;
				condvar.signalOne;
				Error("dummy error").throw
			}
		};
 
		try(deep: true) { redefine.value };
		condvar.waitFor(0.1);
 
		didPlay = false;
		try(deep: true) { redefine.value };
		condvar.waitFor(0.1);

		this.assertEquals(didPlay, true, "task proxy should play again after an error");
	}

	test_nested_instances {
		var condvar = CondVar();
		var innerHasBeenCalled = false, outerHasBeenCalled = false;

		fork {
			var stream;
			try(deep: true) {
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
