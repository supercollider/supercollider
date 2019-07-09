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
		var clock1, clock2,
		tempo = 12, beatDurThresh = 0.4 / tempo,
		secList1 = List.new, secList2 = List.new,
		cond = Condition({ secList1.size == 20 and: { secList2.size == 20 } });

		clock1 = LinkClock(tempo);
		{
			2.5.wait;  // offset clock starts
			clock2 = LinkClock(tempo);
			1.5.wait;
			{
				20.do {
					secList2.add(clock2.seconds);
					1.0.wait;
				};
				cond.signal;
			}.fork(clock2);
			20.do {
				secList1.add(clock1.seconds);
				clock1.setMeterAtBeat(rrand(2, 4), clock1.beats);
				1.0.wait;
			};
			cond.signal;
		}.fork(clock1);
		cond.wait;
		this.ifAsserts(
			clock1.numPeers == 1,
			"no extra clocks in session",
			{
				this.assert(
					(secList1 absdif: secList2).every(_ <= beatDurThresh),
					"LinkClock:setMeterAtBeat should not disrupt beat duration\nclock1: %\nclock2: %\n"
					.format(secList1, secList2)
				)
			},
			{
				this.assert(false, "Make sure no other LinkClocks are running while testing");
			}
		);
		clock1.stop;
		clock2.stop;
	}

	test_LinkClock_sync_meter_aligns_barlines {
		var numTrials = 5,
		refClock, testClock, list = List.new, resp,
		cond = Condition.new, outerCond = Condition.new;
		refClock = LinkClock(10);
		1.0.wait;  // refClock's time is not stable yet
		refClock.schedAbs(4, Routine {
			if(refClock.numPeers > 0) {
				this.assert(false, "Make sure no other LinkClocks are running while testing");
			} {
				refClock.beatsPerBar = 3;
				// hacking: syncMeter_ will cause the refClock to sync 1 sec later
				// we do not want that for this test!
				refClock.slotPut(\syncMeter, true);
				numTrials.do { |i|
					if(i > 0) {  // don't wait first time
						rrand(0.2, 3.0).wait;
					};
					testClock = LinkClock.new;
					// wait to pick up the peer
					resp = SimpleController(testClock).put(\numPeers, {
						resp.remove;
						cond.unhang;
					});
					cond.hang;
					testClock.syncMeter = true;
					resp = SimpleController(testClock).put(\meter, {
						resp.remove;
						cond.unhang;
					});
					cond.hang;
					testClock.queryMeter { |replies|
						list.add(replies.every { |reply|
							(reply[\queriedAtBeat] - testClock.baseBarBeat) % testClock.beatsPerBar
							absdif: reply[\beatInBar] < 0.05
						});
						cond.unhang;
					};
					cond.hang;
					testClock.stop;
				};
				this.assertEquals(
					list.count { |bool| bool },
					list.size,
					"Count of successful trials should = number of trials"
				);
			};
			outerCond.unhang;
		});
		outerCond.hang;
		1.0.wait;
		refClock.stop;
	}

	test_LinkClock_sync_meter_propagates_meter_changes {
		var clock1 = LinkClock.new.syncMeter_(true),
		clock2, resp, cond = Condition.new, result;
		1.0.wait;  // allow time for clock1 to find others
		if(clock1.numPeers == 0) {
			clock2 = LinkClock.new.syncMeter_(true);
			// wait for clock2 to sync meter
			resp = SimpleController(clock2).put(\meter, {
				resp.remove;
				cond.unhang;
			});
			cond.hang;
			clock1.setMeterAtBeat(3, clock1.nextBar);
			// wait for clock2 to sync meter again
			resp = SimpleController(clock2).put(\meter, {
				resp.remove;
				cond.unhang;
			});
			cond.hang;
			result = clock2.beatsPerBar == clock1.beatsPerBar;
			this.assertEquals(clock2.beatsPerBar, clock1.beatsPerBar,
				"clock1 and clock2 should have the same beatsPerBar after one changes"
			);
		} {
			result = false;
			this.assert(false, "Make sure no other LinkClocks are running while testing");
		};
		1.0.wait;
		clock1.stop; clock2.stop;
	}
}
