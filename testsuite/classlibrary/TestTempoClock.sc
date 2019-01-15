TestTempoClock : UnitTest {
	var clock;

	setUp {
		clock = TempoClock.new(1000);
	}

	// called after each test
	tearDown {
		clock.stop;
	}

	test_isRunning {
		var beforeStop = clock.isRunning, afterStop;
		clock.stop;
		afterStop = clock.isRunning;
		this.assert(beforeStop and: { afterStop.not }, "TempoClock:isRunning should be true before 'stop', and false afterward");
	}

	test_afterStop_isRemovedFromAll {
		clock.stop;
		this.assert(TempoClock.all.includes(clock).not, "TempoClock.all should not contain a stopped instance");
	}

	test_threadWait_advancesCorrectTime {
		var now = SystemClock.seconds, then, cond = Condition.new;
		{
			1.wait;
			then = SystemClock.seconds;
			cond.unhang;
		}.fork(clock);
		cond.hang;
		// note: floating point math, == is not safe
		this.assert((then - now absdif: 0.001) < 1e-10, "TempoClock 1000 beats/sec, 1 beat should be 0.001 seconds");
	}

	test_beats2secs_validConversion {
		this.assert(
			clock.beats2secs(clock.beats + 1) - SystemClock.seconds absdif: 0.001 < 1e-10,
			"TempoClock 1000 beats/sec, 'beats2secs' for now + 1 beat should be 0.001 seconds later"
		)
	}

	test_secs2beats_validConversion {
		this.assert(
			clock.secs2beats(SystemClock.seconds + 1) - clock.beats absdif: 1000.0 < 1e-10,
			"TempoClock 1000 beats/sec, 'secs2beats' for now + 1 second should be 1000 beats later"
		)
	}

	test_defaultMeter_unchanged {
		this.assert(
			clock.beatsPerBar == 4,
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
		var cond = Condition.new;
		{
			0.5.wait;
			cond.unhang;
		}.fork(clock);
		cond.hang;
		this.assertEquals(
			clock.nextTimeOnGrid(1, -0.9), 1.1,
			"nextTimeOnGrid(1, -0.9) at beat 0.5 should wrap up to 1.1"
		)
	}
}
