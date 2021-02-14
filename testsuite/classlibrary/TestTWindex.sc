TestTWindex : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_TWindex_ar_skips_zero_probability {
		var cond = Condition.new,
		success = true,
		node, failResp;

		server.bootSync;

		// testing a specific bug:
		// TWindex should initialize its 'sum' variable per trigger
		// older versions do not do that, so they may output an index
		// for a zero probability
		node = {
			var trig = Impulse.ar(ControlRate.ir * 2),  // 2 triggers / control block
			index = TWindex.ar(trig, [0, 1]);
			SendReply.ar(trig * (index <= 0), '/badIndex');
			Line.ar(0, 0, ControlDur.ir, doneAction: 2);
			Silent.ar(1);
		}.play(server);
		node.onFree {
			// success == true means the OSCFunc didn't unhang
			// other case, unnecessary to re-unhang
			if(success) { cond.unhang }
		};

		failResp = OSCFunc({
			success = false;
			cond.unhang;
		}, '/badIndex', argTemplate: [node.nodeID]);

		cond.hang;

		this.assert(success, "TWindex.ar should output only for nonzero probabilities");
	}
}
