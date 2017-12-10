TestServer_boot : UnitTest {

	test_volume {
		var s = Server(\test_Volume, NetAddr("localhost", 57111));
		var queryReply;
		var correctReply = [ '/g_queryTree.reply', 0, 0, 2, 1, 0, 1000, -1, 'volumeAmpControl2' ];

		// set volume so its synthdef, synth and set get sent right after boot
		s.volume.volume = -1;
		s.bootSync;

		OSCFunc({ |msg|
			queryReply = msg;
		},'/g_queryTree.reply', s.addr).oneShot;
		s.sendMsg("/g_queryTree", 0);
		s.sync;

		this.assert(queryReply == correctReply,
			"Server boot should send volume synthdef and create synth immediately when set to nonzero volume.");

		s.quit.remove;
	}

	test_waitForBoot {
		var options = ServerOptions.new;
		var s = Server(\test_waitForBoot, NetAddr("localhost", 57112), options);
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
		s.quit.remove;
	}

	test_bootSync {
		var options = ServerOptions.new;
		var s = Server(\test_bootSync, NetAddr("localhost", 57113), options);
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
		s.quit.remove;
	}

	test_allocWhileBooting {
		var s = Server(\test_allocWhileBooting), done = false, count = 0;
		var prevNodeID = -1, nodeID = 0, failed = false;
		var timer = fork {
			5.wait;
			failed = true;
			"% - server boot timed out.".format(thisMethod).warn;
		};

		s.boot;

		while {
			count < 1000 and: { failed.not }
		} {
			if(s.serverRunning) {
				".".post;
				count = count + 1;
				nodeID = s.nextNodeID;
				if(nodeID <= prevNodeID) {
					failed = true;
					"failed: prevNodeID % and nodeID = % "
					.format(prevNodeID, nodeID).warn;
				};
				prevNodeID = nodeID;
			};
			0.001.wait;
		};
		"end of while loop, timer stops...".postln;

		timer.stop;

		this.assert(failed.not,
			"allocating nodeIDs while booting should not produce duplicate nodeIDs."
		);
		s.quit.remove;
	}

	test_fourWaysToPlaySound {
		var options = ServerOptions.new;
		var s = Server(\test_fourWaysToPlaySound, NetAddr("localhost", 57114), options);
		var amps, flags;
		var o = OSCFunc({ |msg| amps = msg.drop(3) }, '/the8Amps');
		var cond = Condition();

		s.options.numOutputBusChannels = 8;

		s.waitForBoot({
			var amps = List[];

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
		s.remove;
		o.free;
	}
}
