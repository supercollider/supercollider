TestThreadReschedule : UnitTest {
	test_Threads_rescheduleToOtherClock {
		var cond = Condition.new,
		// please make sure 'tempo' is a power of two
		// to avoid floating-point rounding errors
		tempo = 1024,
		// different tempi to be sure time conversion is working
		// 0.75's denominator is also a power of two -- important for same reason
		tempoRatio = 0.75,
		clock1 = TempoClock.new(tempo),
		clock2 = TempoClock.new(tempo * tempoRatio),
		expectedBeats = [0.0, 1.0, 1.5, 2.5],
		expectedDeltaSeconds = [tempo, tempo, tempo * tempoRatio].reciprocal,
		threadTimes = Array(6), taskTimes = Array(6),
		thread = Routine {
			4.do {
				threadTimes.add([thisThread.beats, thisThread.seconds]);
				1.0.wait;
			};
		}.play(clock1),
		task = Task {
			4.do {
				taskTimes.add([thisThread.clock.beats, thisThread.clock.seconds]);
				1.0.wait;
			};
		}.play(clock1),
		controlThread = Routine {
			// For some completely bizarre and unknown reason,
			// this first wait must be slightly longer than 1.0 beat.
			// Without it, the order of execution is:
			// thread, controlThread, task.
			// ControlThread is added into the queue LAST
			// but without this extra wait, it runs before the Task.
			// Then the thread and the task wake up at different times
			// relative to rescheduling.
			// That is probably a bug in itself but outside the scope
			// of this test.
			1.0001.wait;  // remember 1 beat < 1 ms here
			thread.reschedule(clock2);
			task.reschedule(clock2);
			// The test requires 4 data points (to measure tempo after rescheduling).
			// So, all 3 beats are necessary. 3 beats < 3 ms.
			// this was increased to accommodate timing variations for systems unders stress
			6.wait;
			cond.unhang;
		}.play(clock1);
		cond.hang;
		thread.stop;
		clock1.stop;
		clock2.stop;
		this.assertEquals(
			threadTimes.flop[0], expectedBeats,
			"Rescheduling a Routine should adjust beats for continuous seconds"
		);
		this.assert(
			threadTimes.flop[1].differentiate.drop(1)
			.fuzzyEqual(expectedDeltaSeconds, 0.00001).every(_ > 0),
			"Tempo should be maintained before rescheduling a Routine, and change after rescheduling"
		);
		this.assertEquals(
			taskTimes.flop[0], expectedBeats,
			"Rescheduling a Task should adjust beats for continuous seconds"
		);
		this.assert(
			taskTimes.flop[1].differentiate.drop(1)
			.fuzzyEqual(expectedDeltaSeconds, 0.00001).every(_ > 0),
			"Tempo should be maintained before rescheduling a Task, and change after rescheduling"
		);
	}

	test_Threads_rescheduleToOtherTime {
		var cond = Condition.new,
		// please make sure 'tempo' is a power of two
		// to avoid floating-point rounding errors
		tempo = 1024,
		clock1 = TempoClock.new(tempo),
		expectedBeats = [0.0, 1.0, 3.0, 4.0],
		expectedDeltaSeconds = [1.0, 2.0, 1.0] / tempo,
		threadTimes = Array(6), taskTimes = Array(6),
		thread = Routine {
			4.do {
				threadTimes.add([thisThread.beats, thisThread.seconds]);
				1.0.wait;
			};
		}.play(clock1),
		task = Task {
			4.do {
				taskTimes.add([thisThread.clock.beats, thisThread.clock.seconds]);
				1.0.wait;
			};
		}.play(clock1),
		controlThread = Routine {
			// same issue as above
			1.0001.wait;  // remember 1 beat < 1 ms here
			// one beat later than normal
			thread.reschedule(quant: thisThread.beats.roundUp(1) + 1);
			task.reschedule(quant: thisThread.beats.roundUp(1) + 1);
			// The test requires 4 data points (to measure tempo after rescheduling).
			// So, all 3 beats are necessary. 3 beats < 3 ms.
			3.wait;
			cond.unhang;
		}.play(clock1);
		cond.hang;
		thread.stop;
		clock1.stop;
		this.assertEquals(
			threadTimes.flop[0], expectedBeats,
			"Rescheduling a Routine at another beat should affect wake-up times"
		);
		this.assert(
			threadTimes.flop[1].differentiate.drop(1)
			.fuzzyEqual(expectedDeltaSeconds, 0.00001).every(_ > 0),
			"Rescheduling a Routine, delta in seconds should match beats"
		);
		this.assertEquals(
			taskTimes.flop[0], expectedBeats,
			"Rescheduling a Task at another beat should affect wake-up times"
		);
		this.assert(
			taskTimes.flop[1].differentiate.drop(1)
			.fuzzyEqual(expectedDeltaSeconds, 0.00001).every(_ > 0),
			"Rescheduling a Task, delta in seconds should match beats"
		);
	}
}
