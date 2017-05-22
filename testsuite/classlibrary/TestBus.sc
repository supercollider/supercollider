
TestBus : UnitTest {

	ignore_test_free {
		var s,busses,numBusses;
		s = Server.default;
		s.newAllocators;

		numBusses = s.options.numAudioBusChannels - (s.options.numOutputBusChannels + s.options.numInputBusChannels);

		busses = Array.fill( numBusses,{
									Bus.audio(s,1);
							});
		this.assert(busses.every(_.notNil),"should be able to allocate all busses");
		this.assertEquals( busses.select(_.notNil).size, numBusses," should be numAudioBusChannels busses");

		busses.do({ |b| b.free });

		busses = Array.fill( numBusses,{
									Bus.audio(s,1);
							});

		this.assert(busses.every(_.notNil),"after freeing, should be able to re-allocate all busses");
		this.assertEquals( busses.select(_.notNil).size, numBusses," should be numAudioBusChannels busses");

	}
	test_controlFree {
		var s,busses;
		s = Server.default;
		s.newAllocators;

		busses = Array.fill( s.options.numControlBusChannels,{
									Bus.control(s,1);
							});
		this.assertEquals( busses.select(_.notNil).size, s.options.numControlBusChannels," should be numControlBusChannels busses");

		busses.do({ |b| b.free });

		busses = Array.fill( s.options.numControlBusChannels,{
									Bus.control(s,1);
							});

		this.assertEquals( busses.select(_.notNil).size, s.options.numControlBusChannels," should be numControlBusChannels busses able to allocate again after freeing all");
	}

	// note: server reboot does not de-allocate busses
	// and isn't supposed to

	test_get {
		var s, bus, set_value, get_value;
		s = this.s;
		this.bootServer;

		set_value = 88.88;
		bus = Bus.control(s);
		s.sync;

		bus.set(set_value);
		s.sync;

		get_value = 0;
		bus.get({ |value|
			get_value = value;
		});

		0.2.wait;

		// Some precision is lost when converting to 32-bit float, hence the use of
		// assertFloatEquals.
		this.assertFloatEquals(get_value, set_value, "Bus:get works", 0.001);

		bus.free;
	}

	test_getn {
		var s, bus, set_values, get_values;
		s = this.s;
		this.bootServer;

		set_values = [88.88, 8, 888.8, 8.88];
		bus = Bus.control(s, set_values.size);
		s.sync;

		bus.setn(set_values);
		s.sync;

		get_values = [0, 0];
		bus.getn(2, { |values|
			get_values = values;
		});

		0.2.wait;

		// See comments in test_get.
		this.assertArrayFloatEquals(get_values, set_values[0..1], "Bus:getn works", 0.001);

		bus.free;
	}

}
