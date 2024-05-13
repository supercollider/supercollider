
TestThrow : UnitTest {

	test_throw_nested_threads {
		var condition = Condition.new;
		var call1 = false, call2 = false;
		defer { 0.1.wait; condition.unhang }; // unhang on failure

		fork { // isolate from surrounding thread
			var r = Routine {
				var r = Routine {
					var r = Routine {
						Error("test").throw;
					};
					var next = thisThread;
					r.exceptionHandler = {
						call1 = true;
						condition.unhang;
						next.handleError
					};
					r.next;
					condition.hang;
				};
				var next = thisThread;
				r.next;
			};
			var next = thisThread;
			r.exceptionHandler = {
				call2 = true;
				condition.unhang;
				next.handleError
			};
			r.next;
		};

		condition.hang;
		this.assert(call1, "When throwing an error in a thread, handleError should call parent thread in inner thread");
		this.assert(call2, "When throwing an error in a thread, handleError should call parent thread");
	}

}
