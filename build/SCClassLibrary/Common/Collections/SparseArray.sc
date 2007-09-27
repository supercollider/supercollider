
Order : SequenceableCollection {
	
	var <>array, <>indices;
	
	*new { arg size; 
		^super.new.clear(size)
	}
	
	*newFromIndices { arg array, indices;
		^super.newCopyArgs(array, indices)
	}
	
	
	clear { arg size = (8);
		array = Array.new(size);
		indices = Array.new(size);
	}

	makeEmpty { this.clear }

	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy)
	}
	
	do { arg function;
		indices.do { |index, i|
			function.value(array.at(i), index, i)
		};
	}
	
	doFrom { arg function, index = 0;
		if(index <= indices.last) {
			for(this.slotFor(index), indices.size-1, { arg i;
				function.value(array.at(i), indices.at(i))
			})
		}
	}
	
	keysValuesDo { arg function;
		indices.do { arg index, i;
			function.value(index, array[i], i)
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
	
	
}


SparseArray : Order {

	var <>default, <>defaultSize, <>defaultIndex = 0;
	
	
	*reduceArray { arg array, default;
		var res = this.new.default_(default);
		array.do { |item, i| res.putIfNotDefault(i, item) };
		res.defaultSize = if(default.isNil) { res.size } { array.size };
		^res
	}
	
	putIfNotDefault { arg i, item;
		if(item != default) { this.put(i, item) }
	}
		
	clear { arg size;
		super.clear(size);
		defaultSize = size;
	}

	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy, default, defaultSize)
	}

	at { arg index;
		^super.at(index) ? default
	}
	
	do { arg function;
		if(this.isEmpty) { ^this };
		this.size.do { |i|
			function.value(this.at(i), i)
		}
	}
	
	// should maybe be doRange
	doFrom { arg function, index = 0;
		for(index, this.size - 1, { |i|
			function.value(this.at(i), i)
		})
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
	
	collect { arg function;
		^this.class.reduceArray(
			this.asArray.collect(function), 
			default !? { function.value(default, defaultIndex) }		)
	}
	
	select { arg function;
		^this.class.reduceArray(
			this.asArray.select(function), 
			if(default.notNil and: { function.value(default, defaultIndex) }) { default }
		)
	}
	
	reject { arg function;
		^this.class.reduceArray(
			this.asArray.reject(function), 
			if(default.notNil and: { function.value(default, defaultIndex).not }) { default }
		)
	}

		
	// does not pass the index to each default item: faster than collect
	sparseCollect { arg function;
		var res = super.collect(function);
		default !? { res.default = function.value(default) };
		^res
	}
	
	sparseSelect { arg function;
		var res = super.select(function);
		if(default.notNil and: { function.value(default, defaultIndex) }) { res.default = default }
		^res
	}
	
	sparseReject { arg function;
		var res = super.reject(function);
		if(default.notNil and: { function.value(default, defaultIndex).not }) 
			{ res.default = default }
		^res
	}
	
	sparseRemoveAt { arg index;
		^super.removeAt(index)
	}
	
	// removeAt acts still not as one would expect from a normal array.
		
	compress {
		var ind, list;
		array.do { |item, i|
			if(item != default) { 
				list = list.add(item); 
				ind = ind.add(indices.at(i)) 
			};
		};
		^this.newFromIndices(list, ind)
	}
	
	putSeries { arg first, second, last, value; 
		(first, second..last).do { |index|
			this.put(index, value)
		}
	}
	atSeries { arg first, second, last, value; 
		^(first, second..last).collect { |index|
			this.at(index)
		}
	}
	
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

