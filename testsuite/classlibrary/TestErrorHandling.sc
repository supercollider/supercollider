
TestThrow : UnitTest {
	test_same_thread {
		this.assert(try { Error().throw; false } { true }, "Same thread catch" );
	}

	test_resume {
		var r = Routine { Error("out of line").throw };
		this.assert(try { r.value; false } { true }, "Resume catch");
		this.assert(try { Routine{Error("in line").throw}.value; false } { true }, "Resume catch, inline");
	}

	test_throw_nested_threads {
		var errors = 0;
		var condVar = CondVar();
		try(deep: true) {
			try { 
				fork { Error("meow").throw };
				Error("woof").throw;
			} { |er|
				errors = errors + 1;
				condVar.signalAll;
			}
		} {
			errors = errors + 10;
			condVar.signalAll;
		};
		condVar.wait { errors == 11 };
		this.assertEquals(errors, 11, "Threads should inherit their exception handlers");
	}

	test_throw_nested_threads_many {
		var errors = 0;
		var condVar = CondVar();
		try(deep: true) { 
			fork { 
				fork { Error("meow").throw };
				Error("meow").throw ;
			 };
			 fork {
				fork { fork{ Error("woof").throw } };
			};
			Error("meow").throw ;
		} { 
			errors = errors + 1;
			if(errors == 4){
				condVar.signalAll;
			}
		};
		condVar.wait { errors == 4 };
		this.assertEquals(errors, 4, "Threads should inherit their exception handlers even in nested threads");
	}

	test_nested_trys {
		var one = false;
		var two = false;
		try {
			try {
				Error().throw
			} {
				|er|
				one = true;
				er.throw;
			}
		} {
			two = true;
		};
		this.assert(one, "one");
		this.assert(two, "two");
	}

	test_halts {
		try {
			Error().throw;
			this.assert(false, "Should never get here");
		} {
			this.assert(true, "Errors should throw");
		};

		try(deep: true) {
			fork { 
				Error().throw;
				this.assert(false, "Should never get here");
			};
		} {
			this.assert(true, "Errors should throw");
		};

		try(deep: true) {
			fork {
				fork { 
					Error().throw;
					this.assert(false, "Should never get here");
				};
			};
		} {
			this.assert(true, "Errors should throw");
		};
	}

	test_rethrow {
		var errors = 0;
		var condVar = CondVar();


		try(deep: true) {
			try { fork {Error().throw} } { |er| er.throw };
		} {
			errors = errors + 1;
			condVar.signalAll;
		};

		condVar.wait { errors == 1 };
		this.assertEquals(errors, 1, "Should be able to rethrow to the parent");
	}

	test_inherit {
		var r = Routine { Error("meow").throw; };
		var wasThrow = false;
		var condVar = CondVar();

		try { 
			r.value();
		} {
			wasThrow = true;
			condVar.signalAll;
		};
		this.assert(condVar.waitFor(2, { wasThrow }));
		this.assert(wasThrow, "Should throw an error and be caught 1");

		r.reset;

		wasThrow = false;
		try(deep: true) { 
			fork { r.value() }
		} {
			wasThrow = true;
			condVar.signalAll;
		};
		this.assert(condVar.waitFor(2, { wasThrow }));
		this.assert(wasThrow, "Should throw an error and be caught 2");

		r.reset;
		wasThrow = false;

		fork {
			try { 
				r.value();
				this.assert(false, "should not get here 3") 
			} {
				wasThrow = true;
				condVar.signalAll;
			};
		};
		this.assert(condVar.waitFor(2, { wasThrow }));
		this.assert(wasThrow, "Should throw an error and be caught 3");
	}

	nonLocalReturn { ^{ OutOfContextReturnError.returnIsValid} }

	test_outofcontextcheck {
		this.assert(OutOfContextReturnError.returnIsValid);
		this.assert(this.nonLocalReturn.() == false);
	}

}

TestProtect : UnitTest {
	test_basic {
		var r = false;
		try { 
			protect { Error().throw } { r = true }
		};
		this.assert(r);
	}
}
