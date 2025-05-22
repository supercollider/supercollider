TestEnvironmentRedirect : UnitTest {

	test_dispatch {
		var e = EnvironmentRedirect();
		var notified = false;
		e.dispatch = { notified = true };

		e[\test] = 1;
		this.assert(notified, "should run dispatch when a new value is put");

		notified = false;
		e[\test] = 2;
		this.assert(notified, "should run dispatch when an existing value is modified");

		notified = false;
		e[\test] = nil;
		this.assert(notified, "should run dispatch when an existing value is set to nil");
		e[\test] = 2;

		notified = false;
		e.removeAt(\test);
		this.assert(notified, "should run dispatch when an existing value is removed");

		notified = false;
		e.removeAt(\test);
		this.assert(notified.not, "should not run dispatch we try to remove a value twice");

		notified = false;
		e.removeAt(\notExisting);
		this.assert(notified.not, "should not run dispatch when a non-existing value is removed");
	}
}
