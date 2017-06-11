ContiguousBlockAllocatorWithOffset {
	var	<size, array, freed, <pos, top, <addrOffset;

	*new { |size, pos = 0, addrOffset = 0|
		^super.newCopyArgs(size,
			Array.newClear(size).put(pos, ContiguousBlock(pos, size-pos)),
			IdentityDictionary.new,
			pos, pos, addrOffset);
	}

	alloc { |n = 1|
		var	block;
		(block = this.findAvailable(n)).notNil.if({
			^this.prReserve(block.start, n, block).start
			+ addrOffset;
		}, { ^nil });
	}

	reserve { |address, size = 1, warn = true|
		var	block, new;
		address = address - addrOffset;

		((block = array[address] ?? { this.findNext(address) }).notNil and:
				{ block.used and:
				{ address + size > block.start } }).if({
			warn.if({ "The block at (%, %) is already in use and cannot be reserved."
				.format(address, size).warn; });
		}, {
			(block.start == address).if({
				new = this.prReserve(address, size, block);
				^new;
			}, {
				((block = this.findPrevious(address)).notNil and:
						{ block.used and:
						{ block.start + block.size > address } }).if({
					warn.if({ "The block at (%, %) is already in use and cannot be reserved."
						.format(address, size).warn; });
				}, {
					new = this.prReserve(address, size, nil, block);
					^new;
				});
			});
		});
		^nil
	}

	free { |address|
		var	block, prev, next, temp;
		// this 'if' prevents an error if a Buffer object is freed twice
		address = address - addrOffset;

		if(address.isNil) { ^this };
		((block = array[address]).notNil and: { block.used }).if({
			block.used = false;
			this.addToFreed(block);
			((prev = this.findPrevious(address)).notNil and: { prev.used.not }).if({
				(temp = prev.join(block)).notNil.if({
						// if block is the last one, reduce the top
					(block.start == top).if({ top = temp.start });
					array[temp.start] = temp;
					array[block.start] = nil;
					this.removeFromFreed(prev).removeFromFreed(block);
					(top > temp.start).if({ this.addToFreed(temp); });
					block = temp;
				});
			});
			((next = this.findNext(block.start)).notNil and: { next.used.not }).if({
				(temp = next.join(block)).notNil.if({
						// if next is the last one, reduce the top
					(next.start == top).if({ top = temp.start });
					array[temp.start] = temp;
					array[next.start] = nil;
					this.removeFromFreed(next).removeFromFreed(block);
					(top > temp.start).if({ this.addToFreed(temp); });
				});
			});
		});
	}

	blocks {
		^array.select({ arg b; b.notNil and: { b.used } })
	}

	findAvailable { |n|
		(freed[n].size > 0).if({ ^freed[n].choose });

		freed.keysValuesDo({ |size, set|
			(size >= n and: { set.size > 0 }).if({
				^set.choose
			});
		});

		(top + n > size or: { array[top].used }).if({ ^nil });
		^array[top]
	}

	addToFreed { |block|
		freed[block.size].isNil.if({ freed[block.size] = IdentitySet.new });
		freed[block.size].add(block);
	}

	removeFromFreed { |block|
		freed[block.size].tryPerform(\remove, block);
			// I tested without gc; performance is about half as efficient without it
		(freed[block.size].size == 0).if({ freed.removeAt(block.size) });
	}

	findPrevious { |address|
		address = address - addrOffset;
		forBy(address-1, pos, -1, { |i|
			array[i].notNil.if({ ^array[i] });
		});
		^nil
	}

	findNext { |address|
		var	temp;
		address = address - addrOffset;
		(temp = array[address]).notNil.if({
			^array[temp.start + temp.size]
		}, {
			for(address+1, top, { |i|
				array[i].notNil.if({ ^array[i] });
			});
		});
		^nil
	}

	prReserve { |address, size, availBlock, prevBlock|
		var	new, leftover;
		address = address - addrOffset;
		(availBlock.isNil and: { prevBlock.isNil }).if({
			prevBlock = this.findPrevious(address);
		});
		availBlock = availBlock ? prevBlock;
		(availBlock.start < address).if({
			#leftover, availBlock = this.prSplit(availBlock,
				address - availBlock.start, false);
		});
		^this.prSplit(availBlock, size, true)[0];
	}

	prSplit { |availBlock, n, used = true|
		var	new, leftover;
		#new, leftover = availBlock.split(n);
		new.used = used;
		this.removeFromFreed(availBlock);
		used.not.if({ this.addToFreed(new) });
		array[new.start] = new;
		leftover.notNil.if({
			array[leftover.start] = leftover;
			top = max(top, leftover.start);
			(top > leftover.start).if({ this.addToFreed(leftover); });
		});
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
