
NodeIDAllocator {
	var <user, initTemp, temp, perm, mask, permFreed;
	var <numIDs = 0x04000000; // 2 ** 26
	// support 32 users

	*new { arg user=0, initTemp = 1000;
		if (user > 31) { "NodeIDAllocator user id > 31".error; ^nil };
		^super.newCopyArgs(user, initTemp).reset
	}

	idOffset { ^numIDs * user }

	reset {
		mask = user << 26;
		temp = initTemp;
		perm = 2;
		permFreed = IdentitySet.new;
	}
	alloc {
		var x;
		x = temp;
		temp = (x + 1).wrap(initTemp, 0x03FFFFFF);
		^x | mask
	}
	allocPerm {
		var x;
		if(permFreed.size > 0) {
			x = permFreed.minItem;
			permFreed.remove(x);
		} {
			x = perm;
			perm = (x + 1).min(initTemp - 1);
		}
		^x | mask
	}
	freePerm { |id|
			// should not add a temp node id to the freed-permanent collection
		id = id bitAnd: 0x03FFFFFF;
		if(id < initTemp) { permFreed.add(id) }
	}
}


PowerOfTwoBlock {
	var <address, <size, <>next;
	*new { arg address, size;
		^super.newCopyArgs(address, size)
	}
}

PowerOfTwoAllocator {
	var size, array, freeLists, pos=0;

	*new { arg size, pos=0;
		^super.newCopyArgs(size, Array.newClear(size), Array.newClear(32), pos)
	}
	alloc { arg n;
		var sizeClass, node, address;
		n = n.nextPowerOfTwo;
		sizeClass = n.log2Ceil;

		node = freeLists.at(sizeClass);
		if (node.notNil, {
			freeLists.put(sizeClass, node.next);
			^node.address
		});
		if (pos + n <= size, {
			array.put(pos, PowerOfTwoBlock(pos, n));
			address = pos;
			pos = pos + n;
			^address
		});
		^nil
	}
	free { arg address;
		var node, sizeClass,next;

		if((node = array.at(address)).notNil,{

			sizeClass = node.size.log2Ceil;
			node.next = freeLists.at(sizeClass);
			freeLists.put(sizeClass, node);
			array.put(address, nil);

		});
	}
	blocks {
		^array.select({ arg b; b.notNil })
	}
}

LRUNumberAllocator {
	// implements a least recently used ID allocator.

	var lo, hi;
	var array, size, head=0, tail=0;

	*new { arg lo, hi;
		^super.newCopyArgs(lo, hi).init
	}
	init {
		size = hi-lo+1;
		array = Array.newClear(size);
		for(lo, hi-1, { arg i, j; array.put(j, i) });
		head = size-1;
		tail=0;
	}
	free { arg id;
		var nextIndex;
		nextIndex = (head+1) % size;
		if ( nextIndex == tail, { ^nil }); // full
		array.put(head, id);
		head = nextIndex;
	}
	alloc {
		var id;
		if (head == tail, { ^nil }); // empty
		id = array.at(tail);
		tail = (tail+1) % size;
		^id
	}
}

StackNumberAllocator {
	var lo, hi, freeList, next;

	*new { arg lo, hi;
		^super.newCopyArgs(lo, hi).init
	}
	init {
		next = lo - 1;
	}
	alloc {
		if (freeList.size > 0, { ^freeList.pop });
		if (next < hi, { ^next = next + 1; });
		^nil
	}
	free { arg inIndex;
		freeList = freeList.add(inIndex);
	}
}

RingNumberAllocator {
	var lo, hi, next;

	*new { arg lo, hi;
		^super.newCopyArgs(lo, hi).init
	}
	init {
		next = hi;
	}
	alloc {
		^next = (next + 1).wrap(lo,hi)
	}
}


// by hjh: for better handling of dynamic allocation

ContiguousBlock {

	var <start, <size, <>used = false;  // assume free; owner must say otherwise

	*new { |start, size| ^super.newCopyArgs(start, size) }

	address { ^start }

	adjoins { |block|
		^(start < block.start and: { start + size >= block.start })
			or: { start > block.start and: { block.start + block.size >= start } }
	}

	join { |block|
		var newstart;
		if(this.adjoins(block)) {
			^this.class.new(newstart = min(start, block.start),
				max(start + size, block.start + block.size) - newstart)
		} {
			^nil
		};
	}

	split { |span|
		if(span < size) {
			^[this.class.new(start, span),
				this.class.new(start + span, size - span)]
		} {
			if(span == size) {
				^[this, nil]
			} { ^nil }
		};
	}

	storeArgs { ^[start, size, used] }
	printOn { |stream| this.storeOn(stream) }
}

// pos is offset for reserved numbers,
// addrOffset is offset for clientID * size
// THIS IS THE RECOMMENDED ALLOCATOR FOR BUSES AND BUFFERS
ContiguousBlockAllocator {
	var <size, array, freed, <pos, <top, <addrOffset;
	// pos is offset for reserved numbers,
	// addrOffset is offset for clientID * size

	// Array structure is sparse.
	// If you allocate 3 size-2 blocks, it should be:
	// array[0]: ContiguousBlock(0, 2, true)
	// array[1]: nil
	// array[2]: ContiguousBlock(2, 2, true)
	// array[3]: nil
	// array[4]: ContiguousBlock(4, 2, true)
	// array[5]: nil
	// array[6]: ContiguousBlock(6, ...a big number..., false)

	// For every ContiguousBlock in the array,
	// there should ALWAYS be the next ContiguousBlock at
	// block.start + block.size.
	// E.g. block at 0 above --> 0+2 --> 2 and there's a block at index 2.
	// If this is not true, the allocator is in an inconsistent state.
	// This should never happen by following the public interface:
	// alloc(), reserve(), free()

	// Array indices are always adjusted by the `addrOffset` (usually 0).

	*new { |size, pos = 0, addrOffset = 0|
		var shiftedPos = pos + addrOffset;
		^super.newCopyArgs(size,
			Array.newClear(size).put(pos, ContiguousBlock(shiftedPos, size-pos)),
			IdentityDictionary.new,
			shiftedPos, shiftedPos, addrOffset);
	}

	alloc { |n = 1|
		var block = this.findAvailable(n);
		if(block.notNil) {
			^this.prReserve(block.start, n, block).start
		} { ^nil };
	}

	reserve { |address, size = 1, warn = true|
		var block = array[address] ?? { this.prFindNext(address) };
		var new;
		if(block.notNil and:
			{ block.used and:
				{ address + size > block.start }
		}) {
			if(warn) {
				"The block at (%, %) is already in use and cannot be reserved."
				.format(address, size).warn;
			};
		} {
			if(block.start == address) {
				new = this.prReserve(address, size, block);
				^new
			} {
				block = this.prFindPrevious(address);
				if(block.notNil and:
					{ block.used and:
						{ block.start + block.size > address }
				}) {
					if(warn) {
						"The block at (%, %) is already in use and cannot be reserved."
						.format(address, size).warn;
					};
				} {
					new = this.prReserve(address, size, nil, block);
					^new
				};
			};
		};
		^nil
	}

	free { |address|
		var block, prev, next, temp;
		// this 'if' prevents an error if a Buffer object is freed twice
		if(address.isNil) { ^this };
		block = array[address - addrOffset];
		if(block.notNil and: { block.used }) {
			block.used = false;
			this.addToFreed(block);
			prev = this.prFindPrevious(address);
			// We are freeing block B.
			// If block A (immediately before it) is unused,
			// then join A and B into a new, single block
			// spanning the entire width of A and B together.
			// (On-the-fly defragmentation of the address space.)
			if(prev.notNil and: { prev.used.not }) {
				temp = prev.join(block);
				if(temp.notNil) {
					// if block is the last one, reduce the top
					if(block.start == top) { top = temp.start };
					array[temp.start - addrOffset] = temp;
					array[block.start - addrOffset] = nil;
					this.removeFromFreed(prev).removeFromFreed(block);
					if(top > temp.start) { this.addToFreed(temp) };
					block = temp;
				};
			};
			// and if this also touches the next block, join them
			next = this.prFindNext(block.start);
			if(next.notNil and: { next.used.not }) {
				temp = next.join(block);
				if(temp.notNil) {
					// if next is the last one, reduce the top
					if(next.start == top) { top = temp.start };
					array[temp.start - addrOffset] = temp;
					array[next.start - addrOffset] = nil;
					this.removeFromFreed(next).removeFromFreed(block);
					if(top > temp.start) { this.addToFreed(temp) };
				};
			};
		};
	}

	blocks {
		^array.select({ arg b; b.notNil and: { b.used } })
	}

	// Returns an available index for the given number of channels
	// or nil if none is found.
	findAvailable { |n|
		if(freed[n].size > 0) { ^freed[n].choose };

		freed.keysValuesDo({ |size, set|
			(size >= n and: { set.size > 0 }).if({
				^set.choose
			});
		});

		if(top + n - addrOffset > size or: { array[top - addrOffset].used }) { ^nil };
		^array[top - addrOffset]
	}

	addToFreed { |block|
		if(freed[block.size].isNil) { freed[block.size] = IdentitySet.new };
		freed[block.size].add(block);
	}

	removeFromFreed { |block|
		freed[block.size].tryPerform(\remove, block);
		// I tested without gc; performance is about half as efficient without it
		if(freed[block.size].size == 0) { freed.removeAt(block.size) };
	}

	// At any address, look backward for the immediately preceding block.
	// If `address` is the first block, this will correctly be nil.
	prFindPrevious { |address|
		forBy(address-1, pos, -1, { |i|
			if(array[i - addrOffset].notNil) { ^array[i - addrOffset] };
		});
		^nil
	}

	// At any address, look forward for the immediately following block.
	// If `address` is within the last block, this will correctly be nil.
	prFindNext { |address|
		var temp = array[address - addrOffset];
		var indexOfNextBlock;
		// Is there already a block at 'address'?
		if(temp.notNil) {
			// Yes.
			// In that case (see comments at the top of the class),
			// the next block is expected to be at temp.start + temp.size.
			// So the 'else' block's search is redundant.
			indexOfNextBlock = temp.start + temp.size;
		} {
			// No.
			// We know there is no block at 'address',
			// so start searching in the next slot.
			indexOfNextBlock = address + 1;
			// 'top' points to the last ContiguousBlock.
			// Indices > top must be nil. So, no need to search them.
			while { indexOfNextBlock <= top and: { array[indexOfNextBlock - addrOffset].isNil } } {
				indexOfNextBlock = indexOfNextBlock + 1;
			};
		};
		// At this point, indexOfNextBlock points either to the next block, or to nil.
		// It may be past the array's upper bound. THIS IS OK.
		// SC returns nil for array indices out of bounds.
		// If there is no next block, it is correct and expected to return nil!
		^array[indexOfNextBlock - addrOffset]
	}

	prReserve { |address, size, availBlock, prevBlock|
		var new, leftover;
		if(availBlock.isNil and: { prevBlock.isNil }) {
			prevBlock = this.prFindPrevious(address);
		};
		availBlock = availBlock ? prevBlock;
		if(availBlock.start < address) {
			#leftover, availBlock = this.prSplit(availBlock,
				address - availBlock.start, false);
		};
		^this.prSplit(availBlock, size, true)[0];
	}

	prSplit { |availBlock, n, used = true|
		var new, leftover;
		#new, leftover = availBlock.split(n);
		new.used = used;
		this.removeFromFreed(availBlock);
		if(used.not) { this.addToFreed(new) };
		array[new.start - addrOffset] = new;
		if(leftover.notNil) {
			array[leftover.start - addrOffset] = leftover;
			top = max(top, leftover.start);
			if(top > leftover.start) { this.addToFreed(leftover) };
		};
		^[new, leftover]
	}

	debug { |text|
		Post << text << ":\n\nArray:\n";
		array.do({ |item, i|
			item.notNil.if({ Post << i << ": " << item << "\n"; });
		});
		Post << "\nFree sets:\n";
		freed.keysValuesDo({ |size, set|
			Post << size << ": " <<< set << "\n";
		});
	}
}
