TestReadableNodeIDAllocator : UnitTest {

	// test proper creation from Server
	test_readableNodeRange {
		var s = Server(\testRNIA);
		var prevClass = Server.nodeAllocClass;
		Server.nodeAllocClass = ReadableNodeIDAllocator;

		s.options.maxLogins = 1;
		s.newAllocators;
		this.assert(
			(s.nodeAllocator.numIDs == (10 ** 9)),
			"for a single client, (readable) nodeAllocator has maximum range."
		);

		s.options.maxLogins = 16;
		s.newAllocators;
		this.assert(
			(s.nodeAllocator.numIDs == (10 ** 8)),
			"for 16 clients, (readable) nodeAllocator has equally divided range."
		);

		s.remove;
		Server.nodeAllocClass = prevClass;
	}

	test_permIDs {
		var alloc = ReadableNodeIDAllocator(8191, 1000, 8191);
		var nextPermID, permIDs = List[], permIDToFree;

		thisThread.randSeed_(4711);

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
			"ReadableNodeIDAllocator should begin at first permID after freeing all permanent IDs in mixed alloc/free use."
		);
	}

	test_fullRange {
		var alloc = ReadableNodeIDAllocator(8191, 1000, 8191);
		var tempID, prevID = -1, count = 0;

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
	}
}
