
TestThrow : UnitTest {

	test_throw_nested_threads {
		var condition = Condition.new;
		var call1 = false, call2 = false;

		fork { // isolate from surrounding thread
			var r = Routine {
				var r = Routine {
					var r = Routine {
						Error("test").throw;
					};
					var next = thisThread;
					r.exceptionHandler = {
						condition.unhang;
						call1 = true;
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
				condition.unhang;
				call2 = true;
				next.handleError
			};
			r.next;
		};

		condition.hang;
		this.assert(call1 && call2, "When throwing an error in a thread, handleError should call parent thread");
	}

}
