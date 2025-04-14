TestEnvironmentRedirect {

	test_dispatch {
		var e = EnvironmentRedirect();
		var notified = false;
		e.dispatch = { notified = true };

		e[\test] = 1;
		e.assert(notified, "should run dispatch when a new value is put")

		notified = false;
		e[\test] = 2;
		e.assert(notified, "should run dispatch when an existing value is modified")

		notified = false;
		e[\test] = nil;
		e.assert(notified, "should run dispatch when an existing value is set to nil")

		notified = false;
		e.removeAt(\test);
		e.assert(notified, "should run dispatch when an existing value is removed")

		notified = false;
		e.removeAt(\notExisting);
		e.assert(notified.not, "should not run dispatch when a non-existing value is removed")
	}
}
