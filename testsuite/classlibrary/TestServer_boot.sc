TestServer_boot : UnitTest {

	test_waitForBoot {
		var options = ServerOptions.new;
		var s = Server(\testserv1, NetAddr("localhost", 57111), options);
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

		s.quit;
		s.remove;
		of.free;
	}

	test_bootSync {
		var options = ServerOptions.new;
		var s = Server(\testserv1, NetAddr("localhost", 57111), options);
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

		s.quit;
		s.remove;
		of.free;
	}

	test_allocWhileBooting {
		var s = Server(\test), done = false, count = 0;
		var prevNodeID = -1, nodeID = 0, failed = false;
		// 5000 * 0.001 = 5 sec to boot and reach serverRunning
		var timeoutCount = 5000;

		s.boot;
		while {
			timeoutCount = timeoutCount - 1;
			(timeoutCount > 0)
			and: { done.not and: { failed.not } }
		} {
			if(s.serverRunning) {
				count = count + 1;
				nodeID = s.nextNodeID;
				if(nodeID <= prevNodeID) {
					failed = true;
					"failed: prevNodeID % and nodeID = % "
					.format(prevNodeID, nodeID).warn;
				};

				prevNodeID = nodeID;
				done = count > 1000;
			};
			0.001.wait;
		};
		if (timeoutCount <= 0) {
			"% - server timed out and did not reach serverRunning = true."
			.warn;
			failed = true
		};
		this.assert(failed.not,
			"allocating nodeIDs while booting should not produce duplicate nodeIDs."
		);
	}

	test_fourWaysToPlaySound {
		var options = ServerOptions.new;
		var s = Server(\testserv1, NetAddr("localhost", 57111), options);
		var amps, flags;
		var o = OSCFunc({ |msg| amps = msg.drop(3) }, '/the8Amps');
		var cond = Condition();

		s.options.numOutputBusChannels = 8;

		s.waitForBoot({
			var amps = List[];
			// should work without wait eventually
			// 1.wait;

			// 4 ways to make sounds on the first 8 chans
			Pbind(\legato, 1.1, \server, s).play;
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

			2.wait;

			s.quit;
			cond.unhang;
		});

		cond.hang;

		flags = amps.clump(2).collect(_.every(_ > 0.01)).postln;
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

		Ndef.dictFor(s).clear;
		s.quit;
		s.remove;
		o.free;
	}
}
