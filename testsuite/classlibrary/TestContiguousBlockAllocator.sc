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

	test_CBAllocator_can_reserve_multiple_range_types {
		var alloc = ContiguousBlockAllocator(50);
		var block;
		var ok;

		try {
			block = alloc.reserve(0, 10);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator can reserve from the start of its range");

		block = nil;
		try {
			block = alloc.reserve(20, 10);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator can reserve in the last free region");

		block = nil;
		try {
			block = alloc.reserve(12, 5);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator can reserve within an interior free region");
	}

	test_CBAllocator_can_reserve_with_nonzero_offset {
		var offset = 10;
		var alloc = ContiguousBlockAllocator(50, addrOffset: offset);
		var block;
		var ok;

		try {
			block = alloc.reserve(offset, 10);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator with offset > 0 can reserve from the start of its range");

		block = nil;
		try {
			block = alloc.reserve(offset + 20, 10);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator with offset > 0 can reserve in the last free region");

		block = nil;
		try {
			block = alloc.reserve(offset + 12, 5);
		};
		this.assert(block.notNil, "ContiguousBlockAllocator with offset > 0 can reserve within an interior free region");
	}
}
