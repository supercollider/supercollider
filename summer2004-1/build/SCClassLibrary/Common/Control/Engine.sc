
NodeIDAllocator
{
	var <user, temp, perm, mask;
	// support 32 users
	
	*new { arg user=0;
		if (user > 31) { "NodeIDAllocator user id > 31".error; ^nil };
		^super.newCopyArgs(user).reset
	}
	reset {
		mask = user << 26;
		temp = 1000;
		perm = 2;
	}
	alloc {
		var x;
		x = temp;
		temp = (x + 1).wrap(1000, 0x03FFFFFF);
		^x | mask
	}
	allocPerm {
		var x;
		x = perm;
		perm = (x + 1).min(999);
		^x | mask
	}
}


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
		var node, sizeClass,next;
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

