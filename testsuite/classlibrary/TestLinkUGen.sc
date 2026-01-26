TestLinkUGens : UnitTest {
	var server;

	setUp {
		var condition = Condition();
		server = Server(this.class.name);
		server.bootSync;

		LinkPhase.start(server);
		server.sync;
	}

	tearDown {
		LinkPhase.stop(server);
		server.sync;
		server.quit;
		server.remove;
	}

	test_getAndSetTempo {
		var condition = Condition();

		LinkPhase.setTempo(3.0, server: server);
		server.sync;

		{LinkTempo.kr()}.loadToFloatArray(0.2, server, {|sig|
			var success = sig.any(_==3.0);
			this.assert(success, "LinkTempo should be able to get the correct tempo", report: true);
			condition.unhang;
		});

		condition.hang;
	}

	test_syncWithLinkClock {
		var condition = Condition();

		var clock = LinkClock(1.0);
		// since the clock is network bound we need to wait a bit here...
		0.2.wait;
		clock.tempo_(4.0);
		0.2.wait;

		{LinkTempo.kr()}.loadToFloatArray(0.2, server, {|sig|
			var success = sig.any({|x| (x-4.0).abs < 0.01});
			this.assert(success, "LinkTempo should be able to follow LinkClock", report: true);
			condition.unhang;
		});

		condition.hang;
	}

	test_phase {
		var condition = Condition();

		// 40/3 cps = 800 bpm
		LinkPhase.setTempo(40/3, server: server);
		server.sync;
		0.2.wait;

		{
			var phase = LinkPhase.kr(quantum: 1.0)-0.5;
			var oldPhase = FOS.kr(phase, a1: 1.0);
			var crossing = (phase >= 0.0) * (oldPhase < 0.0);
			Integrator.kr(crossing);
		}.loadToFloatArray(1.0, server, {|sig|
			var numCrossings = sig.last.asInteger;
			// 800 bpm / 60sec = 13.3 bps
			// numCrossings should therefore be either 13 or 14, based on when we start
			var success = (numCrossings == 13).or(numCrossings == 14);
			this.assert(success, "LinkPhase should be usable to count phase wraps according to cps", report: true);
			condition.unhang;
		});

		condition.hang;
	}
}
