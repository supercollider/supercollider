SortedList : List {
	var <>function;

	*new { arg size = 8, function;
		function = function ? { arg a, b; a < b }
		^super.new(size).function_(function)
	}
	add { arg item;
		var nextIndex;

		if ( this.isEmpty, {
			^super.add(item);
		});
		nextIndex = this.indexForInserting(item);
		this.insert(nextIndex, item);
	}
	addAll { arg aCollection;
		aCollection = aCollection.asCollection;
		if ( aCollection.size > (this.size div: 3), {
			// Faster to add the new elements and resort
			aCollection.do({ arg each; super.add(each) });
			this.sort
		},{	// Faster to add the elements individually in their proper places
			aCollection.do({ arg each; this.add(each) });
		});
	}

	copyRange { arg start, end; ^this.class.newUsing(array.copyRange(start, end)).function_(function) }
	copySeries { arg first, second, last;
		^this.class.newUsing(array.copySeries(first, second, last)).function_(function)
	}

	// PRIVATE
	indexForInserting { arg newObject;
		var index;
		var low = 0;
		var high = this.size-1;
		while ({
			index = high + low div: 2;
			low <= high;
		},{
			if (function.value(array.at(index), newObject), {
				low = index + 1;
			},{
				high = index - 1;
			});
		});
		^low
	}

	sort { this.sortRange(0, array.size - 1) }

	sortRange { arg i, j;
		//Sort elements i through j of this to be nondescending according to
		// function.

		var di, dij, dj, tt, ij, k, l, n;
		// The prefix d means the data at that index.
		if ((n = j + 1  - i) <= 1, { ^this });	// Nothing to sort.

		//Sort di,dj.
		di = array.at(i);
		dj = array.at(j);
		if (function.value(di, dj).not, { // i.e., should di precede dj?
			array.swap(i,j);
				 tt = di;
				 di = dj;
				 dj = tt;
		});
		if ( n > 2, { // More than two elements.
			ij = (i + j) div: 2;  // ij is the midpoint of i and j.
			dij = array.at(ij);  // Sort di,dij,dj.  Make dij be their median.
			if (function.value(di,  dij), {  // i.e. should di precede dij?
				if (function.value(dij, dj).not, {  // i.e., should dij precede dj?
					array.swap(j, ij);
					dij = dj;
				})
			},{ // i.e. di should come after dij"
				array.swap(i, ij);
				dij = di;
			});
			if ( n > 3, { // More than three elements.
				// Find k>i and l<j such that dk,dij,dl are in reverse order.
				// Swap k and l.  Repeat this procedure until k and l pass each other.
				k = i;
				l = j;
				while ({
				 	while ({
				 		l = l - 1;
				 		k <= l and: { function.value(dij, array.at(l)) }
				 	}); // i.e. while dl succeeds dij
				 	while ({
				 		k = k + 1;
				 		k <= l and: { function.value(array.at(k), dij) };
				 	}); // i.e. while dij succeeds dk
				 	k <= l
				},{
					array.swap(k, l);
				});
		// Now l<k (either 1 or 2 less), and di through dl are all less than or equal to dk
		// through dj.  Sort those two segments.
				this.sortRange(i, l);
				this.sortRange(k, j);
			});
		});
	}
}
