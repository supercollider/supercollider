
StackNumberAllocator
{
	var lo, hi, freeList, next;
	
	*new { arg lo, hi;
		^super.newCopyArgs(lo, hi).init
	}
	init {
		next = lo;
	}
	alloc {
		var out;
		if (freeList.size > 0, { ^freeList.pop });
		if (next < hi, { 
			out = next;
			next = next + 1;
			^out
		});
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
		next = lo;
	}
	alloc { var out; 
		out = next; 
		next = (next + 1).wrap(lo,hi); 
		^out 
	}
}

//Engine
//{
//	var audioBusAllocator, bufAllocator, noteAllocator;
//	init {
//		audioBusAllocator = StackNumberAllocator(
//}
