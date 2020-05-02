
TestPmono : UnitTest {

	test_Pmono_makes_node_after_pause {
		var player, event;
		player = Pmono(
			\default,
			\degree, Pseries(-7, 1, inf),
			\dur, 0.01
		).play;
		// we are simulating the user action of
		// stopping an EventStreamPlayer while it's running
		// there is really no way to do that except a hardcoded wait
		// also the test is not valid unless at least one event
		// plays *before* stop. Nonzero wait time guarantees that.
		0.001.wait;
		player.stop;
		event = player.originalStream.next(Event.default);
		this.assertEquals(event[\type], \monoNote, "After stop, Pmono's next event should be monoNote");
	}

	test_PmonoArtic_makes_node_after_pause {
		var player, event;
		player = PmonoArtic(
			\default,
			\degree, Pseries(-7, 1, inf),
			\dur, 0.01,
			\sustain, 0.011,
		).play;
		0.001.wait;
		player.stop;
		event = player.originalStream.next(Event.default);
		this.assertEquals(event[\type], \monoNote, "After stop, PmonoArtic's next event should be monoNote");
	}
}
