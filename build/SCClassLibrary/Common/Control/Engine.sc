
PowerOfTwoBlock 
{
	var <address, <size, <>next;
	*new { arg address, size;
		^super.newCopyArgs(address, size)
	}
}

PowerOfTwoAllocator
{
	// THIS IS THE RECOMMENDED ALLOCATOR FOR BUSES AND BUFFERS
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
		var node, sizeClass;
		if((node = array.at(address)).notNil,{
			sizeClass = node.size.log2Ceil;
			node.next = freeLists.at(sizeClass);
			freeLists.put(sizeClass, node);
		});
	}
	blocks {
		^array.select({ arg b; b.notNil })
	}
}
		
LRUNumberAllocator
{
	// THIS IS THE RECOMMENDED ALLOCATOR FOR NODE ID'S
	// implements a least recently used ID allocator.
	
	var lo, hi, freeList, next;
	
	*new { arg lo, hi;
		^super.newCopyArgs(lo, hi).init
	}
	init {
		next = lo - 1;
		freeList = RingBuffer.new(hi-lo);
	}
	alloc {
		if (next < hi, { ^next = next + 1; });
		^freeList.pop;
	}
	free { arg inIndex; 
		freeList.add(inIndex); 
	}
}

RingBuffer
{
	// used by LRUNumberAllocator
	var array, size, head=0, tail=0;
	*new { arg size;
		^super.newCopyArgs(Array.newClear(size+1), size+1);
	}
	add { arg item;
		var nextIndex;
		nextIndex = (head+1) % size;
		if ( nextIndex == tail, { ^nil }); // full
		array.put(head, item);
		head = nextIndex;
	}
	pop { var item;
		if (head == tail, { ^nil }); // empty
		item = array.at(tail);
		tail = (tail+1) % size;
		^item
	}
}

StackNumberAllocator
{
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

RingNumberAllocator
{
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

