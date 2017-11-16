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

		s.boot;
		while {
			done.not and: { failed.not }
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
		this.assert(failed.not,
			"allocating nodeIDs while booting should not produce duplicate nodeIDs."
		);
	}

	test_fourWaysToPlaySound {
		var s = Server(\testserv1, NetAddr("localhost", 57111));
		var amps, flags;
		var o = OSCFunc({ |msg| amps = msg.drop(3) }, '/the8Amps');
		var cond = Condition();

		s.options.numOutputBusChannels = 8;

		s.waitForBoot({
			var amps = List[];
			// should work without wait eventually
			// 1.wait;

			// 4 ways to make sounds on the first 8 chans
			Pbind(\legato, 2, \amp, 0.2, \dur, 0.25, \server, s).play;
			{ Saw.ar([220, 330], 0.1) }.play(s, 2);
			Synth(\default, [\out, 4, \amp, 0.2], s);
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

		flags = amps.clump(2).postln.collect(_.every(_ > 0.03)).postln;

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

	test_bootSequence {
		var a, cond, b1, b2, t1, t2, oldTFunc;
		var s = Server.default;
		var numBootFuncs = ServerBoot.objects !? { ServerBoot.objects[s].size } ? 0;
		var numTreeFuncs = ServerTree.objects !? { ServerTree.objects[s].size } ? 0;
		var expectedList = List[ '1_Bt', '2_Bt', '3_tr', '4_Tr', '5_Tr', '6_wt', '7_do' ];

		// list of function names to check
		a = List[];
		// add functions to run at all boot stages
		ServerBoot.add (b1 = { |sv| (sv.name + ": ").post; 0.1.wait; a.add('1_Bt'.postln); }, s);
		ServerBoot.add (b2 = { |sv|  (sv.name + ": ").post; 0.1.wait; a.add('2_Bt'.postln); }, s);
		oldTFunc = s.tree;
		s.tree =        { (s.name + ": ").post; 0.1.wait; a.add('3_tr'.postln); };
		ServerTree.add (t1 = { |sv| (sv.name + ": ").post; 0.1.wait; a.add('4_Tr'.postln); }, s);
		ServerTree.add (t2 = { |sv| (sv.name + ": ").post; 0.1.wait; a.add('5_Tr'.postln); }, s);

		s.quit;
		a = List[];
		s.waitForBoot  { 0.1.wait; s.post; a.add('6_wt'.postln); };
		s.doWhenBooted { 0.1.wait; s.post; a.add('7_do'.postln); };
		s.doWhenBooted { 2.wait; cond.unhang; };

		cond = Condition.new;
		cond.hang;

		0.1.wait;

		// "*** expected: ".postln;
		// expectedList.postcs;
		// "*** result:".postln;
		// a.postcs;

		UnitTest.new.assert(a.size == expectedList.size, "each func in boot sequence should happen exactly once.");
		UnitTest().assert(expectedList == a.copy.sort, "all funcs in boot sequence should happen in the correct order.");

		// cleanup
		ServerBoot.remove(b1, s);
		ServerBoot.remove(b2, s);
		ServerTree.remove(t1, s);
		ServerTree.remove(t2, s);
		s.tree = oldTFunc;

		s.quit;
	}
}
