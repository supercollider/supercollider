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

	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.remove;
	}

	test_default {
		this.assertEquals(server.clientID, 0, "s.clientID should be 0 by default.");
	}

	test_badInputTypes {
		server.clientID = -1.sqrt;
		this.assertEquals(server.clientID, 0, "s.clientID should block NaN.");
		server.clientID = -123;
		this.assertEquals(server.clientID, 0, "s.clientID should block negative numbers.");
		server.clientID = 0.57;
		this.assertEquals(server.clientID, 0, "s.clientID should block floating point numbers.");
	}

	test_tooHigh {
		server.options.maxLogins = 32;
		server.clientID = server.options.maxLogins;
		this.assertEquals(server.clientID, 0, "s.clientID should block number >= maxLogins.");
	}

	test_validRange {
		server.options.maxLogins = 32;
		server.clientID = server.options.maxLogins - 1;
		this.assertEquals(server.clientID, 31, "s.clientID should be settable if valid.");
	}

	test_userSpecifiedClientID_invalid {
		var options = ServerOptions.new;
		var s = Server(thisMethod.name, nil, options, 1);
		this.assert(s.clientID.isNil, "Making a server with invalid clientID should return a server with clientID nil.");
		s.remove
	}

	test_userSpecifiedClientID_valid {
		var options = ServerOptions().maxLogins_(8);
		var s = Server(thisMethod.name, nil, options, 7);
		this.assertEquals(s.clientID, 7, "Making a server with valid nonzero clientID should work.");
		s.remove;
	}

	test_allocatorRanges_singleClient {
		// remember the pre-test allocator
		var prevClass = Server.nodeAllocClass;

		Server.nodeAllocClass = NodeIDAllocator;
		server.options.maxLogins = 1;
		server.newAllocators;

		this.assertEquals(
			server.nodeAllocator.numIDs, 2 ** 26,
			"for a single client, nodeAllocator should have its normal range."
		);
		this.assertEquals(
			server.audioBusAllocator.size, server.options.numAudioBusChannels - server.options.firstPrivateBus,
			"for a single client, audioBusAllocator should have full range minus hardware channels."
		);

		this.assertEquals(
			server.controlBusAllocator.size, server.options.numControlBusChannels,
			"for a single client, controlBusAllocator should have full range."
		);
		this.assertEquals(
			server.bufferAllocator.size, server.options.numBuffers,
			"for a single client, bufferAllocator should have full range."
		);

		// reset pre-test allocator
		Server.nodeAllocClass = prevClass;
	}

	test_allocatorRanges_multiClient {
		// remember the pre-test allocator
		var prevClass = Server.nodeAllocClass;

		Server.nodeAllocClass = NodeIDAllocator;
		server.options.maxLogins = 16;
		server.newAllocators;

		this.assertEquals(
			server.nodeAllocator.numIDs, 2 ** 26,
			"for 16 clients, nodeAllocator should have its normal range."
		);
		this.assertEquals(
			server.audioBusAllocator.size,
			(server.options.numAudioBusChannels - server.options.firstPrivateBus) div: 16,
			"for 16 clients, controlBusAllocator should divide non-hardware channels evenly."
		);

		this.assertEquals(
			server.controlBusAllocator.size, server.options.numControlBusChannels div: 16,
			"for 16 clients, controlBusAllocator should divide private bus range evenly."
		);
		this.assertEquals(
			server.bufferAllocator.size, server.options.numBuffers div: 16,
			"for 16 clients, bufferAllocator should divide range evenly."
		);

		Server.nodeAllocClass = prevClass;
	}
}
