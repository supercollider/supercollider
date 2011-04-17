List : SequenceableCollection {
	var <>array;

	*new { arg size = 8; ^super.new.setCollection(Array.new(size)); }
	*newClear { arg size = 0;
		if ( size == 0, {
			^super.new.setCollection(Array.new(8));
		},{
			^super.new.setCollection(Array.newClear(size));
		});
	}
	*copyInstance { arg aList; ^super.new.array_( aList.array.copy ) }
	*newUsing { arg anArray;
		// an array is supplied to use directly
		^super.new.setCollection( anArray )
	}

	asArray { ^array.copy }
	copy { ^this.class.copyInstance(this) }
	copyRange { arg start, end; ^this.class.newUsing(array.copyRange(start, end)) }
	copySeries { arg first, second, last;
		^this.class.newUsing(array.copySeries(first, second, last))
	}
	putSeries { arg first, second, last, value;
		array.putSeries(first, second, last, value);
	}

	grow { arg sizeIncrease; array = array.grow(sizeIncrease); }
	size { ^array.size }
	dump { "List's array:\n".post; array.dump }

	clear {
		this.setCollection(Array.new(8));
	}

	// accessing

	at { arg i; ^array.at(i) }
	clipAt { arg i; i = i.asInteger.clip(0, this.size - 1); ^array.at(i) }
	wrapAt { arg i; i = i.asInteger.wrap(0, this.size - 1); ^array.at(i) }
	foldAt { arg i; i = i.asInteger.fold(0, this.size - 1); ^array.at(i) }

	put { arg i, item; array.put(i, item) }
	clipPut { arg i, item; i = i.asInteger.clip(0, this.size - 1); ^array.put(i, item) }
	wrapPut { arg i, item; i = i.asInteger.wrap(0, this.size - 1); ^array.put(i, item) }
	foldPut { arg i, item; i = i.asInteger.fold(0, this.size - 1); ^array.put(i, item) }

	add { arg item; array = array.add(item); }
	addFirst { arg item; array = array.addFirst(item); }
	insert { arg index, item; array = array.insert(index, item); }
	removeAt { arg index; ^array.removeAt(index); }
	pop { ^array.pop }
	first { if (this.size > 0, { ^array.at(0) }, { ^nil }) }
	last { if (this.size > 0, { ^array.at(this.size - 1) }, { ^nil }) }
	fill { arg item; array.fill(item) }

	// enumerating
	do { arg function;
		array.do(function);
	}
	reverseDo { arg function;
		array.reverseDo(function);
	}
	pairsDo { arg function;
		array.pairsDo(function);
	}
	doAdjacentPairs { arg function;
		array.doAdjacentPairs(function);
	}

	// ordering
	swap { arg i,j; array.swap(i, j) }

	reverse {
		^this.class.newUsing(array.reverse);
	}
	rotate { arg n=1;
		^this.class.newUsing(array.rotate(n));
	}
	stutter { arg n=2;
		^this.class.newUsing(array.stutter(n));
	}
	mirror {
		^this.class.newUsing(array.mirror);
	}
	mirror2 {
		^this.class.newUsing(array.mirror2);
	}
	mirror1 {
		^this.class.newUsing(array.mirror1);
	}
	scramble {
		^this.class.newUsing(array.scramble);
	}
	permute { arg nthPermutation;
		^this.class.newUsing(array.permute(nthPermutation));
	}
	wrapExtend { arg length;
		^this.class.newUsing(array.wrapExtend(length));
	}
	foldExtend { arg length;
		^this.class.newUsing(array.foldExtend(length));
	}
	pyramid { arg patternType=1; // an integer from 1-10
		^this.class.newUsing(array.pyramid(patternType));
	}
	lace { arg length;
		^this.class.newUsing(array.lace(length))
	}
	slide { arg windowLength=3, stepSize=1;
		^this.class.newUsing(array.slide(windowLength, stepSize));
	}

	// PRIVATE:
	setCollection { arg aColl;
		array = aColl.asArray;
	}

	asList { ^this }
}
