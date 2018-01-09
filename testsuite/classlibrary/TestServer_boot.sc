TestServer_boot : UnitTest {

	var s; // mock server for testing

	setUp {
		s = Server(this.class.name, nil, ServerOptions());
	}

	tearDown {
		s.remove;
	}

	cycleNotify { |server|
		// XXX: no efficient way to wait
		server.notify_(false);
		while { server.notified } { 0.1.wait };
		server.notify_(true);
		while { server.notified.not } { 0.1.wait };
	}

	test_waitForBoot {
		var vals = List[];
		var of = OSCFunc({ |msg| vals.add(msg[3]) }, \tr, s.addr);
		var cond = Condition();

		// waitForBoot tests doWhenBooted implicitly
		s.waitForBoot({

			var synth = { SendTrig.ar(Impulse.ar(10), 4711, 1) }.play(s);
			1.wait;
			synth.free;
			cond.unhang;
		});

		cond.hang;
		this.assert((vals.size > 8 and: vals.every(_ == 1)),
			"waitForBoot should allow starting a synth."
		);

		of.free;
		s.quit;
	}

	test_bootSync {
		var vals = List[];
		var of = OSCFunc({ |msg| vals.add(msg[3]) }, \tr, s.addr);
		var synth;

		s.bootSync;

		synth = { SendTrig.ar(Impulse.ar(10), 4711, 1) }.play(s);
		1.wait;
		synth.free;

		this.assert((vals.size > 8 and: vals.every(_ == 1)),
			"bootSync should allow starting a synth right after continuing."
		);

		of.free;
		s.quit;
	}

	// test that setting notify=false, then notify=true doesn't cause ServerBoot to be run
	test_notifyAndServerBootActions {
		var count = 0;
		var func = { count = count + 1 };

		ServerBoot.add(func, s);
		this.bootServer(s);
		this.cycleNotify(s);

		// No efficient way to ensure that ServerTree has run at this point, if it was going to.
		1.wait;

		this.assertEquals(count, 1, "Toggling Server.notify should not cause ServerBoot actions to run.");

		s.quit;
		ServerBoot.remove(func, s);
	}

	// test that setting notify=false, then notify=true doesn't cause ServerTree to be run
	test_notifyAndServerTreeActions {
		var count = 0;
		var func = { count = count + 1 };

		ServerTree.add(func, s);
		this.bootServer(s);
		this.cycleNotify(s);

		// No efficient way to ensure that ServerTree has run at this point, if it was going to.
		1.wait;

		this.assertEquals(count, 1, "Toggling Server.notify should not cause ServerTree actions to run.");

		s.quit;
		ServerTree.remove(func, s);
	}

	// Check that ServerBoot runs when the server is able to accept commands.
	// Will not fail deterministically, but should not fail if implementation is correct.
	test_ServerBootActionTiming {
		var defName = thisMethod.name;
		var of, timer;
		var cond = Condition();

		var func = {
			SynthDef(defName, {
				Line.kr(0, 1, 2, doneAction: 2)
			}).add;
		};

		ServerBoot.add(func, s);
		s.waitForBoot {
			var synth = Synth(defName);
			of = OSCFunc({
				cond.test_(true).signal;  // go immediately
			}, '/n_go', s.addr, argTemplate: [synth.nodeID]);
		};

		// timout of 5 seconds;
		timer = fork { 5.wait; cond.unhang };
		cond.hang;

		timer.stop;
		s.quit;
		ServerBoot.remove(func, s);
		of.free;

		this.assert(cond.test, "ServerBoot should run when the server is able to accept commands.");
	}

	// Check that ServerQuit does not run when the server boots.
	test_ServerQuitActionTiming {
		var cond = Condition();
		var func = { cond.test_(true).signal };

		ServerQuit.add(func, s);
		s.waitForBoot { cond.unhang };
		cond.wait;

		ServerQuit.remove(func, s);
		s.quit;

		this.assert(cond.test.not, "ServerQuit should not run when the server boots.");
	}

	// Check that we can never create duplicate nodeIDs once server.serverRunning
	// is true, even if the server has not been fully synced yet.
	test_allocWhileBooting {
		var prevNodeID = -1;
		var nodeID = 0;
		var failed = false;

		s.boot;

		// wait until serverRunning is true, for up to 5 seconds
		block { |break|
			5000.do {
				if(s.serverRunning) {
					break.value
				} {
					0.001.wait;
				}
			}
		};

		// exit early if server boot fails
		if(s.serverRunning.not) {
			this.assert(false, "Server did not boot after 5 seconds.");
			s.quit;
			^nil
		};

		100.do {
			".".post;
			nodeID = s.nextNodeID;
			if(nodeID <= prevNodeID) {
				failed = true;
				"failed: prevNodeID % and nodeID = %"
				.format(prevNodeID, nodeID).warn;
			};
			prevNodeID = nodeID;
		};

		"Done.".postln;

		this.assert(failed.not,
			"allocating nodeIDs while booting should not produce duplicate nodeIDs."
		);
		s.quit;
	}

	// Check that with s.waitForBoot, the four most common ways of creating sound processes will work.
	test_waitForBoot_fourWaysToPlaySound {
		var amps; // holds measured amplitudes of the 8 channels
		var flags; // whether or not the amplitudes in each pair of channels was nonzero
		var o = OSCFunc({ |msg| amps = msg.drop(3) }, '/the8Amps');
		var pbindPlayer;
		var cond = Condition(); // signalled at end of collecting results

		s.options.numOutputBusChannels = 8;
		s.waitForBoot {
			// 4 ways to make sounds on the first 8 chans
			pbindPlayer = Pbind(\legato, 1.1, \server, s).play;
			{ Saw.ar([220, 330], 0.1) }.play(s, 2);
			Synth(\default, [\out, 4], s);
			Ndef(\testX -> s.name, { PinkNoise.ar(0.1) ! 2 }).play(6);

			// get 8 sound levels
			{
				SendReply.kr(
					Impulse.kr(10), '/the8Amps',
					Amplitude.kr(InFeedback.ar(0, 8), 0.001, 1),
				)
			}.play(s);

			s.sync;
			1.wait;

			// clean up
			pbindPlayer.stop;
			Ndef.dictFor(s).clear;
			cond.unhang;
		};

		cond.hang;

		// clean up
		s.quit;
		o.free;

		// check whether each pair of channels was nonzero
		flags = amps.clump(2).collect(_.every(_ != 0));
		this.assert(flags[0],
			"Server: Pbind should play right after booting."
		);
		this.assert(flags[1],
			"Server: Function.play should play right after booting."
		);
		this.assert(flags[2],
			"Server: Synth(\default) should play right after booting."
		);
		this.assert(flags[3],
			"Server: Ndef should play right after booting."
		);
	}
}
