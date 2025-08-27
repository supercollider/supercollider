TestEnvironmentRedirect : UnitTest {
	var envir, notified, argsPassed;

	setUp {
		envir = EnvironmentRedirect();
		envir.dispatch = { |key, val|
			notified = true;
			argsPassed = [key, val];
		};
		this.reset;
	}

	reset {
		notified = false;
		argsPassed = nil;
	}

	test_dispatch_put {
		envir[\test] = 1;
		this.assert(notified, "should run dispatch when a new value is put");
		this.assertEquals(argsPassed, [\test, 1], "should pass correct args to dispatch");
	}


	test_dispatch_put_twice {
		envir[\test] = 1;
		this.reset;
		envir[\test] = 1;
		this.assert(notified, "should run dispatch when the same value is put twice");
		this.assertEquals(argsPassed, [\test, 1], "should pass correct args to dispatch");
	}

	test_dispatch_replace {
		envir[\test] = 1;
		this.reset;
		envir[\test] = 2;
		this.assert(notified, "should run dispatch when an existing value is modified");
		this.assertEquals(argsPassed, [\test, 2], "should pass correct args to dispatch when modifying an existing value");
	}


	// removing or putting nil

	test_dispatch_set_nil {
		envir[\test] = 1;
		this.reset;
		envir[\test] = nil;
		this.assert(notified, "should run dispatch when an existing value is set to nil");
		this.assertEquals(argsPassed, [\test, nil], "should pass correct args to dispatch when settinga value to nil");
	}

	test_dispatch_remove {
		envir[\test] = 2;
		this.reset;
		envir.removeAt(\test);
		this.assert(notified, "should run dispatch when an existing value is removed");
		this.assertEquals(argsPassed, [\test, nil], "should pass correct args to dispatch when removing a value");
	}


	test_dispatch_remove_twice {
		envir[\test] = 2;
		envir.removeAt(\test);
		this.reset;
		envir.removeAt(\test);
		this.assert(notified.not, "should not run dispatch we try to remove a value twice");
	}

	test_dispatch_nil_nonexisting {
		envir[\notExisting] = nil;
		this.assert(notified.not, "should not run dispatch when a non-existing value is set to nil");
	}

	test_dispatch_remove_nonexisting {
		envir.removeAt(\notExisting);
		this.assert(notified.not, "should not run dispatch when a non-existing value is removed");
	}

}

TestLazyEnvir : TestEnvironmentRedirect {
	setUp {
		envir = LazyEnvir();
		envir.dispatch = { |key, val|
			notified = true;
			argsPassed = [key, val];
		};
		this.reset;
	}

	// these need to be made explicit to become registered, subclassing is not enough
	test_dispatch_put { super.test_dispatch_put }
	test_dispatch_put_twice { super.test_dispatch_put_twice }
	test_dispatch_replace { super.test_dispatch_replace }

	test_dispatch_set_nil { super.test_dispatch_set_nil }
	test_dispatch_remove { super.test_dispatch_remove }
	test_dispatch_remove_twice { super.test_dispatch_remove_twice }

	test_dispatch_nil_nonexisting { super.test_dispatch_nil_nonexisting }
	test_dispatch_remove_nonexisting { super.test_dispatch_remove_nonexisting }
}
