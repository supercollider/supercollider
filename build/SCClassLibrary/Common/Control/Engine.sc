
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

RangeAllocator
{
	var freeList;
	
	*new { arg range;
		var freeList;
		freeList = SortedList(32, { arg a, b; a.size < b.size });
		freeList.add(range.copy);
		^super.newCopyArgs(freeList)
	}
	alloc { arg n;
		var index, newRange; 
		index = freeList.indexForInserting(Range(0,n));
		
		for (index, freeList.size-1, {	
			var range;
			range = freeList.at(index);		
			if (range.size == n, {
				freeList.remove(range);				
				^range;
			});
			if (range.size > n, {
				freeList.remove(range);
				newRange = range.split(n);
				freeList.add(range);
				^newRange;
			});
		});
	}
	free { arg range;
		// does not do coalescing.
		freeList.add(range);
	}
}

//Engine
//{
//	var audioBusAllocator, bufAllocator, noteAllocator;
//	init {
//		audioBusAllocator = StackNumberAllocator(
//}
