/*
TestClientAllocators.run;
UnitTest.gui;

Tests for clientID and allocator mechanisms:
* clientID should be impossible to set to nonsense data,
  it should always be between 0 and maxLogins - 1.
* when clientID was set on creation of server, it should stay
- when no user-set clientID, adjust by server response.
* maxLogins always adjust by server response
* allocators should share ranges evenly for numClients/maxLogins.

NOTE: All test cases currently have powerOfTwo numbers for maxLogins.
  This is because the server currently rounds up maxLogins to nextPowerOfTwo;
  so when the server IS NOT booted, odd numbers of maxLogins stay as they are;
  when the server IS booted, the rounded-up maxLogins come back to each client.
  If this server behavior changes to support non-powerOfTwo numbers of clients,
  the tests should also include irregular numbers for maxLogins, e.g. 1, 2, 3, 5, 8.

*/

TestClientAllocators : UnitTest {
	// these while server is off
	test_ClientIDChecks {
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
		Server.named.removeAt(s.name); Server.all.remove(s);
	}

	test_UserSetClientID {
		var options = ServerOptions.new;
		var s = Server(\testserv, NetAddr("localhost", 57111), options, 1);
		this.assert(s == nil, "Making a server with invalid clientID should be blocked.");
		options.maxLogins_(8);
		s = Server(\testserv, NetAddr("localhost", 57111), options, 7);
		this.assert(s.clientID == 7, "Making a server with valid nonzero clientID should work.");
		Server.named.removeAt(s.name); Server.all.remove(s);
	}

	test_ClientIDByServer {
		var options = ServerOptions.new;
		var s;
		s = Server(\testserv1, NetAddr("localhost", 57111), options);

		s.options.maxLogins = 4; s.clientID = 3;
		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 0, "Non-user-defined clientID should be reset by the server.");
		Server.named.removeAt(s.name); Server.all.remove(s);
		s.quit;
		1.wait;

		s = Server(\testserv2, NetAddr("localhost", 57112), options, 3);

		this.bootServer(s);
		s.sync;
		1.wait;
		this.assert(s.clientID == 3, "User-defined clientID should be left as is by the server.");
		Server.named.removeAt(s.name); Server.all.remove(s);
	}

	test_AllocRanges {
		var s = Server(\testAlloc);
		s.options.maxLogins = 1; s.newAllocators;
		this.assert(
			(s.nodeAllocator.numIDs == 1000000000) and:
			(s.audioBusAllocator.size == s.options.numAudioBusChannels) and:
			(s.controlBusAllocator.size == s.options.numControlBusChannels) and:
			(s.bufferAllocator.size == s.options.numBuffers),
			"for single client, all allocators should have full range."
		);
		s.options.maxLogins = 16; s.newAllocators;
		this.assert(
			(s.nodeAllocator.numIDs == 100000000)
			and: (s.audioBusAllocator.size == (s.options.numAudioBusChannels div: 16))
			and: (s.controlBusAllocator.size == (s.options.numControlBusChannels div: 16))
			and: (s.bufferAllocator.size == (s.options.numBuffers div: 16)),
			"for 16 clients, all allocators should divide range evenly."
		);
		s.options.maxLogins = 1; s.newAllocators;
		Server.named.removeAt(s.name); Server.all.remove(s);
	}

	test_CBAllocator {
		var alloc = ContiguousBlockAllocator(1024, 0, 1024 * 8);
		var remaining = alloc.size, lastID;
		var allIDs = List[], newID, maxPos;
		var sizes = (1024 * (0.5 ** (1.. 1024.log2))).asInteger;

		// sizes.sum = 1023, + [1, 1] goes over max by 1
		(sizes.scramble ++ [1, 1]).do { |size|
			newID = alloc.alloc(size);
			allIDs.add(newID);
		};

		this.assert(
			allIDs.drop(-1).every(_.isNumber) and: allIDs.last.isNil,
			"ContiguousBlockAllocator hands out its full range for variable block sizes."
		);

		allIDs.size.do { allIDs.remove(allIDs.choose); };

		this.assert(
			alloc.pos == alloc.addrOffset,
			"ContiguousBlockAllocator should reclaim its full range after all blocks are removed."
		);

		while {
			// try removing first
			if (0.5.coin) { allIDs.remove(allIDs.choose) };
			// try allocating random block size
			newID = alloc.alloc([1, 2, 3, 5, 8].choose);
			// if random block too big, try single id
			if (newID.isNil) { newID = alloc.alloc; };
			newID.notNil;
		} {
			allIDs.add(newID);
		};
		this.assert(
			alloc.top == (alloc.addrOffset + alloc.size - 1),
			"ContiguousBlockAllocator should hand out its full range in mixed allocation/removal."
		);

		while {
			allIDs.remove(allIDs.choose);
			allIDs.notEmpty;
		} {
			if (0.5.coin) {
				newID = alloc.alloc([1, 2, 3, 5, 8].choose);
				// if random block too big, try single id
				if (newID.isNil) { newID = alloc.alloc; };
				if (newID.notNil) { allIDs.add(newID) };
			};
		};
		this.assert(
			alloc.top == (alloc.addrOffset + alloc.size - 1),
			"ContiguousBlockAllocator should reclaim its full range after mixed allocation/removal.")
	}

	test_RNIDAllocator {
		var alloc = ReadableNodeIDAllocator(8191, 1000, 8191);
		var tempID, prevID = -1, count = 0;
		var nextPermID, permIDs = List[], permIDToFree;

		while {
			tempID = alloc.alloc;
			tempID > prevID
		} {
			count = count + 1;
			prevID = tempID;
		};
		this.assert(
			count == (alloc.numIDs - alloc.lowestTempID + 1)
			and: tempID == (alloc.idOffset + alloc.lowestTempID),
			"ReadableNodeIDAllocator should recycle tempIDs after its full range has been used."
		);

		this.assert(
			alloc.isPerm(alloc.idOffset - 1).not
			and: alloc.isPerm(alloc.idOffset)
			and: alloc.isPerm(alloc.idOffset + alloc.lowestTempID - 1)
			and: alloc.isPerm(alloc.idOffset + alloc.lowestTempID).not,
			"ReadableNodeIDAllocator should identify its permanent IDs."
		);
		while {
			if (0.5.coin and: { permIDs.size > 0 }) {
				permIDToFree = permIDs.remove(permIDs.choose);
				alloc.freePerm(permIDToFree);
			};
			nextPermID = alloc.allocPerm;
			nextPermID.notNil;
		} {
			permIDs.add(nextPermID);
		};
		this.assert(
			permIDs.size.postln == (alloc.lowestTempID - 2),
			"ReadableNodeIDAllocator should hand out all permanent IDs in mixed alloc/free use."
		);

		while {
			if (0.5.coin) {
				nextPermID = alloc.allocPerm;
				if (nextPermID.notNil) {
					permIDs.add(nextPermID);
				};
			};
			permIDs.size > 0;
		} {
			permIDToFree = permIDs.remove(permIDs.choose);
			alloc.freePerm(permIDToFree);
		};
		this.assert(
			alloc.allocPerm == (alloc.idOffset + 2),
			"ReadableNodeIDAllocator should begin at first tempID after freeing all permanent IDs in mixed alloc/free use."
		);
	}
}
