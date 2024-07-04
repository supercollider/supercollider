TestTempoClock : UnitTest {
	var clock;

	setUp {
		clock = TempoClock.new(1000);
	}

	// called after each test
	tearDown {
		clock.stop;
	}

	test_newClock_isRunningTrue {
		this.assert(clock.isRunning, "An active TempoClock should return true for isRunning");
	}

	test_stop_isRunningFalse {
		clock.stop;
		this.assert(clock.isRunning.not, "A stopped TempoClock should return false for isRunning");
	}

	test_stop_removesFromAll {
		clock.stop;
		this.assert(TempoClock.all.includes(clock).not);
	}

	test_threadWait_advancesCorrectTime {
		var timeBefore = SystemClock.seconds, timeAfter, cond = Condition.new;
		{
			1.wait;
			timeAfter = SystemClock.seconds;
			cond.unhang;
		}.fork(clock);
		cond.hang;
		// note: floating point math, == is not safe
		this.assertFloatEquals(
			timeAfter - timeBefore, 0.001,
			"TempoClock 1000 beats/sec, 1 beat should be 0.001 seconds",
			within: 1e-10
		)
	}

	test_beats2secs_validConversion {
		this.assertFloatEquals(
			clock.beats2secs(clock.beats + 1) - SystemClock.seconds, 0.001,
			"TempoClock 1000 beats/sec, 'beats2secs' for now + 1 beat should be 0.001 seconds later",
			within: 1e-10
		)
	}

	test_secs2beats_validConversion {
		this.assertFloatEquals(
			clock.secs2beats(SystemClock.seconds + 1) - clock.beats, 1000.0,
			"TempoClock 1000 beats/sec, 'secs2beats' for now + 1 second should be 1000 beats later",
			within: 1e-10
		)
	}

	// note: *not* assertFloatEquals
	// because inf - inf < within is always false,
	// while inf == inf is always true
	test_beats2secs_handlesInf {
		this.assertEquals(
			clock.beats2secs(inf), inf,
			"TempoClock:beats2secs should return inf for 'inf' beats"
		)
	}

	test_secs2beats_handlesInf {
		this.assertEquals(
			clock.secs2beats(inf), inf,
			"TempoClock:secs2beats should return inf for 'inf' beats"
		)
	}

	test_beatsPerBar_defaultIs4 {
		this.assertEquals(
			clock.beatsPerBar, 4,
			"A new TempoClock should have 4 beats per bar by default"
		)
	}

	test_nextTimeOnGrid_okAfterMeterChange {
		// formally you should change meter only on a barline
		// at 1000 bps that is 0.004 sec later, so... ok.
		var cond = Condition.new;
		{
			clock.beatsPerBar.wait;
			clock.beatsPerBar = 3;
			0.1.wait;  // must tick past the barline
			cond.unhang;
		}.fork(clock);
		cond.hang;
		this.assertEquals(
			clock.nextTimeOnGrid(-1), 7.0,
			"Set meter to 3/4, at beat 4, next barline should be 7.0"
		);
	}

	test_nextTimeOnGrid_addsPhaseCorrectly {
		this.assertEquals(
			clock.nextTimeOnGrid(1, 0.5), 0.5,
			"nextTimeOnGrid(1, 0.5) at beat 0 should be 0.5"
		)
	}

	test_nextTimeOnGrid_negativePhaseWraps {
		// when running the whole suite, 'beats' might be later
		// so, ensure the right test value
		clock.beats = 0.5;
		this.assertEquals(
			clock.nextTimeOnGrid(1, -0.9), 1.1,
			"nextTimeOnGrid(1, -0.9) at beat 0.5 should wrap up to 1.1"
		)
	}
}
