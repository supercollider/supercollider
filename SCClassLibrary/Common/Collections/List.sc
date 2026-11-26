List : SequenceableCollection {
	var <>array;

	*new { |size = 8| ^super.new.setCollection(Array.new(size)) }
	*newClear { |size = 0|
		if(size == 0) {
			^super.new.setCollection(Array.new(8))
		} {
			^super.new.setCollection(Array.newClear(size))
		}
	}
	*copyInstance { |aList| ^super.new.array_(aList.array.copy) }
	*newUsing { |anArray|
		// an array is supplied to use directly
		^super.new.setCollection(anArray)
	}

	asArray { ^array.copy }
	copy { ^this.class.copyInstance(this) }
	copyRange { |start, end| ^this.class.newUsing(array.copyRange(start, end)) }
	copySeries { |first, second, last|
		^this.class.newUsing(array.copySeries(first, second, last))
	}
	putSeries { |first, second, last, value|
		array.putSeries(first, second, last, value)
	}

	grow { |sizeIncrease| array = array.grow(sizeIncrease) }
	size { ^array.size }
	dump { "List's array:\n".post; array.dump }

	clear {
		this.setCollection(Array.new(8))
	}

	// accessing
	indexOf { |item| ^array.indexOf(item) }

	at { |i| ^array.at(i) }
	clipAt { |i| i = i.asInteger.clip(0, this.size - 1); ^array.at(i) }
	wrapAt { |i| i = i.asInteger.wrap(0, this.size - 1); ^array.at(i) }
	foldAt { |i| i = i.asInteger.fold(0, this.size - 1); ^array.at(i) }

	put { |i, item| array.put(i, item) }
	clipPut { |i, item| i = i.asInteger.clip(0, this.size - 1); ^array.put(i, item) }
	wrapPut { |i, item| i = i.asInteger.wrap(0, this.size - 1); ^array.put(i, item) }
	foldPut { |i, item| i = i.asInteger.fold(0, this.size - 1); ^array.put(i, item) }

	add { |item| array = array.add(item) }
	addFirst { |item| array = array.addFirst(item) }
	insert { |index, item| array = array.insert(index, item) }
	boundedInsert { |index, item| array = array.boundedInsert(index, item) }
	removeAt { |index| ^array.removeAt(index) }
	pop { ^array.pop }
	first { if(this.size > 0) { ^array.at(0) } { ^nil } }
	last { if(this.size > 0) { ^array.at(this.size - 1) } { ^nil } }
	fill { |item| array.fill(item) }

	// enumerating
	do { |function|
		array.do(function)
	}
	reverseDo { |function|
		array.reverseDo(function)
	}
	pairsDo { |function|
		array.pairsDo(function)
	}
	doAdjacentPairs { |function|
		array.doAdjacentPairs(function)
	}

	// ordering
	swap { |i, j| array.swap(i, j) }

	reverse {
		^this.class.newUsing(array.reverse)
	}
	rotate { |n = 1|
		^this.class.newUsing(array.rotate(n))
	}
	stutter { |n = 2|
		^this.dupEach(n)
	}
	dupEach { |n = 2|
		^this.class.newUsing(array.dupEach(n))
	}
	mirror {
		^this.class.newUsing(array.mirror)
	}
	mirror2 {
		^this.class.newUsing(array.mirror2)
	}
	mirror1 {
		^this.class.newUsing(array.mirror1)
	}
	scramble {
		^this.class.newUsing(array.scramble)
	}
	permute { |nthPermutation|
		^this.class.newUsing(array.permute(nthPermutation))
	}
	wrapExtend { |length|
		^this.class.newUsing(array.wrapExtend(length))
	}
	foldExtend { |length|
		^this.class.newUsing(array.foldExtend(length))
	}
	pyramid { |patternType = 1| // an integer from 1-10
		^this.class.newUsing(array.pyramid(patternType))
	}
	lace { |length|
		^this.class.newUsing(array.lace(length))
	}
	slide { |windowLength = 3, stepSize = 1|
		^this.class.newUsing(array.slide(windowLength, stepSize))
	}

	// PRIVATE:
	setCollection { |aColl|
		array = aColl.asArray
	}

	asList { ^this }
}
