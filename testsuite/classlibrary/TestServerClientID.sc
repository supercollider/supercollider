/*
TestServer_clientID.run;
UnitTest.gui;

Tests for clientID, maxLogins and allocator creation based on these.

* clientID should be impossible to set to nonsense data,
it should always be between 0 and maxLogins - 1.

NOTE: All test cases currently have powerOfTwo numbers for maxLogins.
This is because the server currently rounds up maxLogins to nextPowerOfTwo;
so when the server IS NOT booted, odd numbers of maxLogins stay as they are;
when the server IS booted, the rounded-up maxLogins come back to each client.
If this server behavior changes to support non-powerOfTwo numbers of clients,
the tests should also include irregular numbers for maxLogins, e.g. 1, 2, 3, 5, 8.

*/

TestServer_clientID : UnitTest {
	// these while server is off
	test_clientIDChecks {
		var s = Server(\testID);
		this.assert(s.clientID == 0, "s.clientID should be 0 by default.");
		// test checks for bad input
		s.clientID = -1.sqrt;
		this.assert(s.clientID == 0, "s.clientID should block non-Integers.");
		s.clientID = -123;
		this.assert(s.clientID == 0, "s.clientID should block negative numbers.");
		s.options.maxLogins = 32;
		s.clientID = s.options.maxLogins;
		this.assert(s.clientID == 0, "s.clientID should block number >= maxLogins.");
		s.clientID = s.options.maxLogins - 1;
		this.assert(s.clientID == 31, "s.clientID should be settable if valid.");
		s.remove;
	}

	test_userSpecifiedClientID {
		var options = ServerOptions.new;
		var s = Server(\testserv, NetAddr("localhost", 57111), options, 1);
		this.assert(s == nil, "Making a server with invalid clientID should be blocked.");
		options.maxLogins_(8);
		s = Server(\testserv, NetAddr("localhost", 57111), options, 7);
		this.assert(s.clientID == 7, "Making a server with valid nonzero clientID should work.");
		s.remove;
	}

	test_allocatorRanges {
		var s = Server(\testAlloc);
		var prevClass = Server.nodeAllocClass;

		Server.nodeAllocClass = NodeIDAllocator;
		s.options.maxLogins = 1;
		s.newAllocators;

		this.assert(
			s.nodeAllocator.numIDs == (2 ** 26),
			"nodeAllocator should have its normal range."
		);
		this.assert(
			s.audioBusAllocator.size == (s.options.numAudioBusChannels - s.options.firstPrivateBus),
			"for a single client, audioBusAllocator should have full range minus hardware channels."
		);

		this.assert(
			s.controlBusAllocator.size == s.options.numControlBusChannels,
			"for a single client, controlBusAllocator should have full range."
		);
		this.assert(
			s.bufferAllocator.size == s.options.numBuffers,
			"for a single client, bufferAllocator should have full range."
		);

		s.options.maxLogins = 16;
		s.newAllocators;

		this.assert(
			s.audioBusAllocator.size ==
			(s.options.numAudioBusChannels - s.options.firstPrivateBus div: 16),
			"for 16 clients, controlBusAllocator should divide private bus range evenly."
		);

		this.assert(
			s.controlBusAllocator.size == (s.options.numControlBusChannels div: 16),
			"for 16 clients, controlBusAllocator should divide private bus range evenly."
		);
		this.assert(
			s.bufferAllocator.size == (s.options.numBuffers div: 16),
			"for 16 clients, bufferAllocator should divide range evenly."
		);

		Server.nodeAllocClass = prevClass;
		s.remove;

	}
}
