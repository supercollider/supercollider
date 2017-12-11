TestServer_boot : UnitTest {

	var s; // mock server for testing

	setUp {
		s = Server(this.class.name, nil, ServerOptions());
	}

	tearDown {
		0.5.wait;
		s.remove;
	}

	test_volume {
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

		this.assertEquals(queryReply, correctReply,
			"Server boot should send volume synthdef and create synth immediately when set to nonzero volume.");

		s.quit;
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

	test_allocWhileBooting {
		var prevNodeID = -1;
		var nodeID = 0;
		var failed = false;

		this.bootServer(s);

		1000.do {
			".".post;
			nodeID = s.nextNodeID;
			if(nodeID <= prevNodeID) {
				failed = true;
				"failed: prevNodeID % and nodeID = %"
				.format(prevNodeID, nodeID).warn;
			};
			prevNodeID = nodeID;
		};

		this.assert(failed.not,
			"allocating nodeIDs while booting should not produce duplicate nodeIDs."
		);
		s.quit;
	}

	test_fourWaysToPlaySound {
		var amps; // holds measured amplitudes of the 8 channels
		var flags; // whether or not the amplitudes in each pair of channels was nonzero
		var o = OSCFunc({ |msg| amps = msg.drop(3) }, '/the8Amps');
		var pbindPlayer;

		s.options.numOutputBusChannels = 8;

		this.bootServer(s);

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
		2.wait;

		// clean up
		pbindPlayer.stop;
		Ndef.dictFor(s).clear;
		s.quit;

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

		o.free;
	}
}
