TestContiguousBlockAllocator : UnitTest {

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
}
