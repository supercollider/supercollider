Order : SequenceableCollection {

	var <>array, <>indices;

	*new { arg size = 8;
		^super.new.clear(size)
	}

	*newFromIndices { arg array, indices;
		^super.newCopyArgs(array, indices)
	}

	clear { arg size;
		array = Array.new(size);
		indices = Array.new(size);
	}

	makeEmpty { this.clear }

	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy)
	}

	asArray { ^array.copy }

	do { arg function;
		indices.do { |index, i|
			function.value(array.at(i), index, i)
		};
	}

	doRange { arg function, from = 0, to;
		if(from <= indices.last) {
			if(to.isNil) { to = indices.size - 1 } { to = min(to, this.lastIndex) };
			for(this.slotFor(from), to) { |i|
				function.value(array.at(i), indices.at(i), i)
			}
		}
	}

	keysValuesDo { arg function;
		indices.do { |index, i|
			function.value(index, array.at(i), i)
		}
	}

	size { ^indices.size }

	lastIndex { ^indices.last }

	// current write position
	pos {
		var index = this.lastIndex;
		^if(index.isNil) { 0 } { index + 1 }
	}

	add { arg obj;
			array = array.add(obj);
			indices = indices.add(this.pos);
	}

	put { arg index, obj;
		this.prPutSlot(this.nextSlotFor(index), index, obj)
	}

	at { arg index;
		var slot = this.slotFor(index);
		^if(slot.notNil) {
			if(indices.at(slot) == index) {
				array.at(slot)
			}
		}
	}

	removeAt { arg index, obj;
		var nextSlot = this.nextSlotFor(index), slot;
		^if(nextSlot.notNil) {
			slot = max(0, nextSlot - 1);
			if(indices.at(slot) == index) { this.removeAtSlot(slot) }
		}
	}

	removeAtSlot { arg slot;
		indices.removeAt(slot);
		^array.removeAt(slot)
	}

	pop {
		indices.pop;
		^array.pop
	}

	collect { arg function;
		^this.copy.array_(array.collect(function));
	}

	select { arg function;
		var res = this.class.new;
		this.indicesDo { |item, i|
			if(function.value(item, i)) { res.put(i, item) }
		}
		^res
	}

	reject { arg function;
		var res = this.class.new;
		this.indicesDo { |item, i|
			if(function.value(item, i).not) { res.put(i, item) }
		}
		^res
	}

	removeAllSuchThat { arg function;
		var removedItems = this.class.new;
		var copy = this.copy;
		copy.indicesDo { | item, i |
			if ( function.value(item, i) )
			{
				this.remove(item);
				removedItems = removedItems.put(i, item);
			}
		};
		^removedItems
	}

	selectInPlace { arg function;
		indices.copy.do { |index, i|
			if(function.value(array.at(i), index).not) {
				this.removeAtSlot(i)
			}
		}
	}

	rejectInPlace { arg function;
		indices.copy.do { |index, i|
			if(function.value(array.at(i), index)) {
				this.removeAtSlot(i)
			}
		}
	}

	indicesDo { arg function;
		indices.do { |slot, i|
			function.value(array.at(i), slot, i)
		}
	}

	// private implementation

	resetIndices { arg step = 1, offset = 0;
		indices = (offset, step .. indices.size - 1);
	}

	nextSlotFor { arg index;
		^indices.indexOfGreaterThan(index) ?? { indices.size }
	}

	slotFor { arg index;
		^max(0, this.nextSlotFor(index) - 1)
	}

	prPutSlot { arg nextSlot, index, obj;
		var slot = max(0, nextSlot - 1);
		if(indices.at(slot) == index) {
			array.put(slot, obj)
		} {
			array = array.insert(nextSlot, obj);
			indices = indices.insert(nextSlot, index);
		}
	}

	choose {
		^array.choose
	}
	
	storeOn { arg stream;
		stream << this.class.name;
		stream << ".newFromIndices( " <<<* [ array, indices ] << " )"; 
	}
}


SparseArray : Order {

	var <>default, <>defaultSize;

	*newClear { arg size, default;
		^super.new(size).defaultSize_(size).default_(default)
	}

	*reduceArray { arg array, default;
		var res = this.new.default_(default);
		array.do { |item, i| res.putIfNotDefault(i, item) };
		res.defaultSize = if(default.isNil) { res.size } { array.size };
		^res
	}

	clear { arg size;
		size = size.min(67108864 / 2); // For very large data we must assume sparse indices or we can't initialise the indices here
		array = Array.new(size);
		indices = Array.new(size);
	}

	putIfNotDefault { arg i, item;
		if(item != default) { this.put(i, item) }
	}

	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy, default, defaultSize)
	}

	asArray { ^this[_] ! this.size }

	at { arg index;
		^super.at(index) ? default
	}

	do { arg function;
		if(this.isEmpty) { ^this };
		this.size.do { |i|
			function.value(this.at(i), i)
		}
	}

	size {
		var last = super.lastIndex ? (-1);
		^if(defaultSize.isNil) {
			last + 1
		} {
			max(last + 1, defaultSize)
		}
	}

	lastIndex {
		var last = super.lastIndex ? 0;
		^if(defaultSize.isNil) {
			last
		} {
			max(last, defaultSize)
		}
	}

	// current write position
	pos {
		var index = super.lastIndex;
		^if(index.isNil) { 0 } { index + 1 }
	}

	collect { arg function;
		^this.class.reduceArray(
			this.asArray.collect(function),
			default !? { function.value(default, 0) }		)
	}

	select { arg function;
		^this.class.reduceArray(
			this.asArray.select(function),
			if(default.notNil and: { function.value(default, 0) }) { default }
		)
	}

	reject { arg function;
		^this.class.reduceArray(
			this.asArray.reject(function),
			if(default.notNil and: { function.value(default, 0).not }) { default }
		)
	}

	sum { | function |
		var sum = 0;
		^if (function.isNil or: {function.numArgs < 2}) { // optimized version if no function, or if index is irrelevant
			this.sparseSum
		}{
			this.do {|elem, i| sum = sum + function.value(elem, i); };
			sum
		}
	}
	// if index is irrelevant, assume that the result for all implicit elements is the same
	sparseSum { | function |
		var sum = 0;
		"sparseSum : inner array size is %".format(array.size).postln;
		if (function.isNil) { // optimized version if no function
			array.do { | elem | sum = sum + elem; };
			sum = sum + (default * (this.size-array.size));
		}{
			array.do {|elem| sum = sum + function.value(elem); };
			sum = sum + (function.value(default) * (this.size-array.size));
		}
		^sum;
	}



	// does not pass the index to each default item: faster than collect
	sparseCollect { arg function;
		var res = super.collect(function);
		default !? { res.default = function.value(default) };
		^res
	}

	sparseSelect { arg function;
		var res = super.select(function);
		if(default.notNil and: { function.value(default, 0) }) { res.default = default }
		^res
	}

	sparseReject { arg function;
		var res = super.reject(function);
		if(default.notNil and: { function.value(default, 0).not })
			{ res.default = default }
		^res
	}

	sparseRemoveAt { arg index;
		^super.removeAt(index)
	}

	sparseRemove { arg item;
		var index = super.indexOf(item);
		^if(index.notNil) { super.removeAt(index) } { nil }
	}

	removeAt { arg index;
		//^this.notYetImplemented(thisMethod)
		var res, slot = this.slotFor(index), size = indices.size;
		if(index >= this.size) { ^nil };

		if(indices[slot] == index) {
			res = this.removeAtSlot(slot);
			slot =  slot - 1;
		} {
			if(size > 0) { res = default };
			if(indices.first > index) { slot = -1 };
		};
		indices = indices[..slot] ++ (indices[slot+1..] - 1);

		if(defaultSize.notNil and: { defaultSize > 0 } and: { index < defaultSize }) {
				defaultSize = defaultSize - 1;
		};
		^res

	}

	firstGap { arg from = 0, to;
		if(indices.first == 0) { ^nil };
		to = to ?? { indices.size };
		(from..to).do { |i|
			if(indices[i] != i) { ^i };
		};
		^nil
	}

	indexOf { arg item;
		var slot = array.indexOf(item), res;
		if(item == default) {
				res = this.firstGap(0, slot);
				if(res.notNil) { ^res };
		};
		^if(slot.isNil) { nil } { indices[slot] }
	}

	compress {
		var ind, list, size = defaultSize ?? { this.size };
		array.do { |item, i|
			if(item != default) {
				list = list.add(item);
				ind = ind.add(indices.at(i))
			};
		};
		^this.class.newFromIndices(list, ind).default_(default).defaultSize_(size)
	}

	pop {
		^if(defaultSize.notNil and: { defaultSize > indices.last }) {
			defaultSize = defaultSize - 1;
			default
		} {
			super.pop
		}
	}


	++ { arg coll;
		var res = this.copy.sparseAddAll(coll);
		if(defaultSize.notNil) { res.defaultSize_(this.size + coll.size) };
		^res
	}

	sparseAddAll { arg coll;
		var slot = this.size;
		coll.do { |item, i|
			if(item != default) { this.put(slot + i, item) }
		};
	}

	putSeries { arg first, second, last, value;
		(first, second..last).do { |index|
			this.put(index, value)
		}
	}
	atSeries { arg first, second, last;
		^(first, second..last).collect { |index|
			this.at(index)
		}
	}

	minItem { |function|
		^if(function.isNil or: {function.numArgs < 2}){
			if(array.size == this.size){ // full up! default not used (weird)
				array.minItem(function)
			}{
				array.minItem(function).min(if(function.isNil, default, function.value(default)))
			}
		}{
			super.minItem(function);
		}
	}
	maxItem { |function|
		^if(function.isNil or: {function.numArgs < 2}){
			if(array.size == this.size){ // full up! default not used (weird)
				array.maxItem(function)
			}{
				array.maxItem(function).max(if(function.isNil, default, function.value(default)))
			}
		}{
			super.maxItem(function);
		}
	}
	
	storeOn { | stream |
		if (stream.atLimit) { ^this };
		stream << this.class.name << "[ " ;
		this.storeItemsOn(stream);
		stream << " ]" ;
	}

	// private implementation

	prPutSlot { arg nextSlot, index, obj;
		var slot = max(0, nextSlot - 1);
		index = index.asInteger; // SparseArray supports only integer indices
		if(indices.at(slot) == index) {
			array.put(slot, obj)
		} {
			array = array.insert(nextSlot, obj);
			indices = indices.insert(nextSlot, index);
		}
	}
}
