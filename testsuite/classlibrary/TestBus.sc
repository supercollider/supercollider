//# VERSION 3.16

TestBus : UnitTest {

	ignore_test_free {
		var s = Server(thisMethod.name);
		s.newAllocators;

		var numBusses = s.options.numAudioBusChannels - (s.options.numOutputBusChannels + s.options.numInputBusChannels);

		var busses = Array.fill( numBusses,{
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

		s.remove;
	}
	test_controlFree {
		var s = Server(thisMethod.name);
		s.options.numControlBusChannels = 10;
		s.newAllocators;

		var busses = Array.fill( s.options.numControlBusChannels,{
									Bus.control(s,1);
							});
		this.assertEquals( busses.select(_.notNil).size, s.options.numControlBusChannels," should be numControlBusChannels busses");

		busses.do({ |b| b.free });

		busses = Array.fill( s.options.numControlBusChannels,{
									Bus.control(s,1);
							});

		this.assertEquals( busses.select(_.notNil).size, s.options.numControlBusChannels," should be numControlBusChannels busses able to allocate again after freeing all");

		s.remove;
	}

	// note: server reboot does not de-allocate busses
	// and isn't supposed to

}
