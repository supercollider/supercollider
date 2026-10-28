TestBus_Server : UnitTest {

	// note: server reboot does not de-allocate busses
	// and isn't supposed to

	test_get {
		var s, bus, set_value, get_value;
		var condition = CondVar.new;
		s = Server(thisMethod.name);
		this.bootServer(s);

		set_value = 88.88;
		bus = Bus.control(s);
		s.sync;

		bus.set(set_value);
		s.sync;

		get_value = 0;
		bus.get({ |value|
			get_value = value;
			condition.signalOne;
		});

		condition.waitFor(1);

		// Some precision is lost when converting to 32-bit float, hence the use of
		// assertFloatEquals.
		this.assertFloatEquals(get_value, set_value, "Bus:get works", 0.001);

		bus.free;
		s.quitSync;
		s.remove;
	}

	test_getn {
		var s, bus, set_values, get_values;
		var condition = CondVar.new;
		s = Server(thisMethod.name);
		this.bootServer(s);

		set_values = [88.88, 8, 888.8, 8.88];
		bus = Bus.control(s, set_values.size);
		s.sync;

		bus.setn(set_values);
		s.sync;

		get_values = [0, 0];
		bus.getn(2, { |values|
			get_values = values;
			condition.signalOne;
		});

		condition.waitFor(1);

		// See comments in test_get.
		this.assertArrayFloatEquals(get_values, set_values[0..1], "Bus:getn works", 0.001);

		bus.free;
		s.quitSync;
		s.remove;
	}

}
