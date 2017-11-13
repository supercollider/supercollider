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
			if(s.isFullyBooted) {
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
}
