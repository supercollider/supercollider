TestLinkClock : UnitTest {

	classvar precision = 1e-04;

	// use Link Test Plan
	// see external_libraries/link/TEST-PLAN.md

	// Test Plan - TEMPO-1
	test_transmission_tempo {
		var clock1, clock2;
		var semaphore = Semaphore(0);

		clock1 = LinkClock.new;
		clock2 = LinkClock.new;
		clock2.tempoChanged = { semaphore.signal};

		0.2.wait;
		clock1.tempo = rrand(20,999)/60;

		// signal semaphore after a certain time to avoid
		// blocking the function
		clock2.sched(2, { semaphore.signal});

		semaphore.wait;
		this.assertFloatEquals( clock2.tempo, clock1.tempo,
			"tempo change should affect all link apps",
			precision
		);

		clock1.stop;
		clock2.stop;
	}

	// Test Plan - TEMPO-2
	test_preserve_session_tempo {
		var clock1, clock2, sessionTempo;

		clock1 = LinkClock(130/60);
		0.5.wait;
		sessionTempo = clock1.tempo;

		clock2 = LinkClock(100/60);
		0.5.wait;

		this.assertFloatEquals( clock2.tempo, sessionTempo,
			"creating a new clock should not affect existing link session",
			precision
		);

		clock1.stop;
		clock2.stop;
	}

	// Test Plan - TEMPO-4
	test_sync {
		var clock1, clock2;
		var semaphore = Semaphore(0);
		var phase1 = Array.newClear(2),
			phase2 = Array.newClear(2);

		clock1 = LinkClock(1).beats_(100.0.rand);
		clock2 = LinkClock(1).beats_(100.0.rand);
		clock2.tempoChanged = { semaphore.signal};

		// test sync at 20 bpm
		clock1.tempo = 20/60;
		clock2.sched(2, { semaphore.signal});
		semaphore.wait;

		phase1[0] = clock1.beatInBar;
		phase2[0] = clock2.beatInBar;

		// test sync at 999 bpm
		clock1.tempo = 999/60;
		clock2.sched(2, { semaphore.signal});
		semaphore.wait;

		phase1[1] = clock1.beatInBar;
		phase2[1] = clock2.beatInBar;

		this.assertArrayFloatEquals( phase1, phase2,
			"link clock should stay in sync with its tempo within the range 20-999 bpm",
			precision
		);

		clock1.stop;
		clock2.stop;
	}

	// Test Plan - TEMPO-5
	test_no_session_tempo {
		var clock, tempo;

		tempo = rrand(20,999)/60;
		clock = LinkClock(tempo);
		0.5.wait;

		this.ifAsserts( clock.numPeers == 0,
			"no existing session",
			{
				this.assertEquals( clock.tempo, tempo,
					"tempo shouldn't change if there is no session to join"
				)
			},
			{
				this.assert(false, "a link session already exists, stop every link application and run the test again");
			}
		);

		clock.stop
	}

	// Test Plan - BEATTIME-1
	test_no_session_beattime {
		var clock, beats, secs;

		clock = LinkClock(1).beats_(100.0.rand);
		secs = thisThread.clock.seconds;
		beats = clock.secs2beats(secs);
		0.4.wait;

		this.ifAsserts( clock.numPeers == 0,
			"no existing session",
			{
				this.assertFloatEquals( clock.secs2beats(secs), beats,
					"no beat jump should occcur if there is no session to join",
					precision
				)
			},
			{
				this.assert(false, "a link session already exists, stop every link application and run the test again");
			}
		);

		clock.stop;
	}

	// Test Plan - BEATTIME-2
	test_no_change_beattime {
		var clock1, clock2;
		var beats;

		clock1 = LinkClock(1);
		0.2.wait;

		beats = clock1.beats;
		clock2 = LinkClock(1);

		this.assertEquals( clock1.beats, beats,
			"clock beats should not change when a participant joins its session"
		);

		clock1.stop;
		clock2.stop;
	}

	// test starting a LinkClock from a TempoClock and reschulding
	// all its tasks
	test_from_TempoClock {
		var tempoClock = TempoClock.new;
		var linkClock;
		var routine, streamplayer;
		var semaphore = Semaphore(0);

		tempoClock.sched(0.2, {
			this.assertEquals( thisThread.clock, linkClock,
				"starting a LinkClock with newFromTempoClock should reschedule functions"
			);
			this.assertEquals( routine.clock, linkClock,
				"starting a LinkClock with newFromTempoClock should reschedule routines"
			);
			this.assertEquals( streamplayer.clock, linkClock,
				"starting a LinkClock with newFromTempoClock should reschedule stream players"
			);

			thisThread.clock.stop;
			semaphore.signal;
		});
		routine = { loop{ 0.01.wait} }.fork(tempoClock);
		streamplayer = Pbind(\dur, 0.1).play(tempoClock);
		0.1.wait;

		linkClock = LinkClock.newFromTempoClock(tempoClock);

		semaphore.wait
	}
}
