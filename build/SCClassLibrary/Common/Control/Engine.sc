
PowerOfTwoBlock 
{
	var <address, <size, <>next;
	*new { arg address, size;
		^super.newCopyArgs(address, size)
	}
}

PowerOfTwoAllocator
{
	var size, array, freeLists, pos=0;
	
	*new { arg size;
		^super.newCopyArgs(size, Array.newClear(size), Array.newClear(32), 0)
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
		node = array.at(address);
		sizeClass = node.size.log2Ceil;
		node.next = freeLists.at(sizeClass);
		freeLists.put(sizeClass, node);
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

