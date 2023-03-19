TestClockScheduling : UnitTest {

	test_immediate_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;

		0.schedBundleArrayOnClock(clock, bundles, lag: 0, server: server, latency: 0);
		this.assertEquals(server.data, [[0, bundles]], "bundles scheduled without delay should schedule immediately");
	}

	test_delta_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;

		0.01.schedBundleArrayOnClock(clock, bundles, lag: 0, server: server, latency: 0);
		0.015.wait;
		this.assertEquals(server.data, [[0.01, bundles]], "bundles scheduled with delta should match delta")

	}

	test_delta_array_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;

		[0, 0.01].schedBundleArrayOnClock(clock, bundles, lag: 0, server: server, latency: 0);
		0.015.wait;

		this.assertEquals(server.data, [[0, bundles], [0.01, bundles]],
			"bundles scheduled with delta array should match delta array"
		)

	}

	test_lag_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;

		0.schedBundleArrayOnClock(clock, bundles, lag: 0.01, server: server, latency: 0);
		0.015.wait;
		this.assertEquals(server.data, [[0.01, bundles]], "bundles scheduled with lag should match lag")

	}

	test_lag_array_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;
		// when lags are an array, deltas are required to be an array, too.
		[0, 0].schedBundleArrayOnClock(clock, bundles, lag: [0, 0.01], server: server, latency: 0);
		0.02.wait;
		this.assertEquals(server.data,  [[0, bundles], [0.01, bundles]],
			"bundles scheduled with lag array should match lag array"
		)

	}

	test_delta_lag_array_sched {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new;
		var server = TestClockSchedulingHelper.new.start;

		[0, 0.01].schedBundleArrayOnClock(clock, bundles, lag: [0.01, 0.02], server: server, latency: 0);
		0.045.wait;
		this.assertEquals(server.data, [[0.01, bundles], [0.03, bundles]],
			"bundles scheduled with lag and delta array should match resulting deltas")

	}

	test_delta_lag_array_sched_flop {
		var bundles = [["/test", 1, 2, 3]];
		var clock = TempoClock.new.tempo_(2);
		var server = TestClockSchedulingHelper.new.start;

		[0, 0.01, 0.02].schedBundleArrayOnClock(clock, bundles, lag: [0.01, 0.02], server: server, latency: 0);
		0.03.wait;
		this.assertEquals(server.data, [[0.01, bundles], [0.02, bundles], [0.025, bundles]],
			"bundles scheduled with lag and delta array should match resulting deltas")
	}

}


TestClockSchedulingHelper {

	var referenceTime, <data;
	var <>latency = 0.2, <>precision = 0.001;

	start {
		referenceTime = thisThread.seconds;
	}

	sendBundle { |latency ... bundle|
		data = data.add(
			[
				(thisThread.seconds - referenceTime).round(precision),
				bundle
			]
		)
	}
}