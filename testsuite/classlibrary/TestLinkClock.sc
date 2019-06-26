TestLinkClock : UnitTest {

	classvar precision = 1e-04;

	// use Link Test Plan
	// see external_libraries/link/TEST-PLAN.md

	// Test Plan - TEMPO-1
	// Tempo changes should be transmitted between connected apps.
	test_settingTempo_transmitsToLinkPeers {
		var clock1, clock2;
		var semaphore = Semaphore(0),
		controller;

		clock1 = LinkClock.new;
		clock2 = LinkClock.new;
		controller = SimpleController(clock2).put(\tempo, {
			controller.remove;
			semaphore.signal
		});

		0.2.wait;
		clock1.tempo = 3;

		// signal semaphore after a certain time to avoid
		// blocking the function
		clock2.sched(2, { semaphore.signal });

		semaphore.wait;
		this.assertFloatEquals( clock2.tempo, clock1.tempo,
			"tempo change should affect all link apps",
			precision
		);

		clock1.stop;
		clock2.stop;
	}

	// Test Plan - TEMPO-2
	// Opening an app with Link enabled should not change the tempo of an existing Link session.
	test_newLinkClock_shouldNotChangeLinkTempo {
		var clock1, clock2, sessionTempo;

		clock1 = LinkClock(2.5);
		0.5.wait;
		sessionTempo = clock1.tempo;

		clock2 = LinkClock(1);
		0.5.wait;

		this.assertFloatEquals( clock2.tempo, sessionTempo,
			"creating a new clock should not affect existing link session",
			precision
		);

		clock1.stop;
		clock2.stop;
	}

	// Test Plan - TEMPO-3
	// When connected, loading a new document should not change the Link session tempo.
	// SC does not change tempo when opening a code document, so this test is irrelevant.

	// Test Plan - TEMPO-4
	// Tempo range handling.
	test_settingTempo_maintainsSyncOver20To999BPM {
		var clock1, clock2;
		var semaphore = Semaphore(0), controller;
		var phase1 = Array.newClear(2),
			phase2 = Array.newClear(2);

		// initially, phase should disagree -- Link should bring into phase
		clock1 = LinkClock(1).beats_(20);
		clock2 = LinkClock(1).beats_(21);
		controller = SimpleController(clock2).put(\tempo, {
			semaphore.signal
		});

		// test sync at 20 bpm
		clock1.tempo = 20/60;
		clock2.sched(2, { semaphore.signal });
		semaphore.wait;

		phase1[0] = clock1.beatInBar;
		phase2[0] = clock2.beatInBar;

		// test sync at 999 bpm
		clock1.tempo = 999/60;
		clock2.sched(2, { semaphore.signal });
		semaphore.wait;

		phase1[1] = clock1.beatInBar;
		phase2[1] = clock2.beatInBar;

		this.assertArrayFloatEquals( phase1, phase2,
			"link clock should stay in sync with its tempo within the range 20-999 bpm",
			precision
		);

		clock1.stop;
		clock2.stop;
		controller.remove;
	}

	// Test Plan - TEMPO-5
	// Enabling Link does not change app's tempo if there is no Link session to join.
	test_newLinkClock_usesOwnTempoIfNoSession {
		var clock, tempo;

		tempo = 5;
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
	// Enabling Link does not change app's beat time if there is no Link session to join.
	test_newLinkClock_usesOwnBeatsIfNoSession {
		var clock, beats, secs;

		clock = LinkClock(1).beats_(10);
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
	// App's beat time does not change if another participant joins its session.
	test_newLinkClock_syncsBeatsToExistingLinkClock {
		var clock1, clock2;
		var beats;

		clock1 = LinkClock(1);
		0.2.wait;

		beats = clock1.beats;
		clock2 = LinkClock(1);

		this.assertEquals( clock1.beats, beats,
			"first clock's beats should not change when a participant joins its session"
		);

		clock1.stop;
		clock2.stop;
	}

	// test starting a LinkClock from a TempoClock and rescheduling all its tasks
	// SC-specific test, not part of Ableton's test plan
	test_newFromTempoClock_reschedulesOldClockQueue {
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
		routine = { loop{ 0.01.wait } }.fork(tempoClock);
		streamplayer = Pbind(\dur, 0.02).play(tempoClock);
		0.1.wait;

		linkClock = LinkClock.newFromTempoClock(tempoClock);

		semaphore.wait
	}

	test_LinkClock_meter_change_beats_keep_continuity {
		// the issue may not reproduce at other tempi
		// I suggest not changing this
		var tempo = 7, beatDur = tempo.reciprocal,
		clock1, clock2,
		secList,
		failedLists,
		outerCond = Condition.new, innerCond = Condition.new;

		// does not consistently fail under original conditions
		// it will *probably* fail at least once under multiple trials
		// but it should never fail -- so run multiple trials
		"Enter 'test_LinkClock_meter_change_beats_keep_continuity'".postln;
		{
			5.do { |i|
				clock1 = LinkClock(tempo).quantum_(4);
				{
					// issue reproduces only if clocks are started some time apart
					2.5.wait;
					clock2 = LinkClock(tempo).quantum_(4);
					0.5.wait;

					// now save beat durations
					secList = List.new;
					(7 - clock1.beats).wait;
					clock2.schedAbs(clock2.nextBar, {
						clock2.setMeterAtBeat(3, clock2.beats);
					});
					1.0.wait;
					secList.add(clock1.seconds);  // now at barline
					clock1.setMeterAtBeat(3, clock1.beats);
					4.do { |i|
						1.0.wait;
						secList.add(clock1.seconds);  // now at barline
					};
					innerCond.unhang;
				}.fork(clock1);
				innerCond.hang;
				secList = secList.differentiate.drop(1);
				if(secList.any { |val| val absdif: beatDur > 0.00001 }) {
					"Trial % failed: %\n".postf(i+1, secList);
					failedLists = failedLists.add(secList);
				} {
					"Trial % passed\n".postf(i+1);
				};
				clock1.stop; clock2.stop;
				0.1.wait;
			};
			outerCond.unhang;
		}.fork;

		outerCond.hang;

		this.assert(
			failedLists.size == 0,
			"LinkClock's beats should be the same duration across meter changes (failed trials = %)"
			.format(failedLists)
		);
	}
}
