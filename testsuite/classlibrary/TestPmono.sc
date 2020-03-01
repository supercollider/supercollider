
TestPmono : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_Pmono_makes_node_after_pause {
		var player, event;
		this.bootServer;
		player = Pmono(
			\default,
			\degree, Pseries(-7, 1, inf),
			\dur, 0.01
		).play;
		// we are simulating the user action of
		// stopping an EventStreamPlayer while it's running
		// there is really no way to do that except a hardcoded wait
		0.05.wait;
		player.stop;
		event = player.originalStream.next(Event.default);
		this.assertEquals(event[\type], \monoNote, "After stop, Pmono's next event should be monoNote");
	}

	test_PmonoArtic_makes_node_after_pause {
		var player, event;
		this.bootServer;
		player = PmonoArtic(
			\default,
			\degree, Pseries(-7, 1, inf),
			\dur, 0.01,
			\sustain, 0.011,
		).play;
		0.05.wait;
		player.stop;
		event = player.originalStream.next(Event.default);
		this.assertEquals(event[\type], \monoNote, "After stop, PmonoArtic's next event should be monoNote");
	}
}
