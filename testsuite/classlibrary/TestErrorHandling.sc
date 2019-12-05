

TestThrow : UnitTest {

	test_throw_nested_threads {

		fork { // isolate from surrounding thread

			var call1 = false, call2 = false;

			var r = Routine {
				var r = Routine {
					var r = Routine {
						Error("test").throw;
					};
					var next = thisThread;
					r.exceptionHandler = { call1 = true; next.handleError };
					r.next;
				};
				var next = thisThread;
				r.next;
			};
			var next = thisThread;
			r.exceptionHandler = { call2 = true; next.handleError };
			r.next;

			this.assert(call1 && call2, "When throwing an error in a thread, handleError should call parent thread");
		}

	}

}
