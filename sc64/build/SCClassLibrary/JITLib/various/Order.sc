// todo: write c code for optimization

Order : SequenceableCollection {
	var <>array, <>indices;
	
	*new { arg size = 8; 
		^super.new.clear(size)
	}
	
	*with { arg obj;
		^this.new.put(0, obj)
	}
	
	clear { arg size = 8;
		array = Array.new(size);
		indices = Array.new(size);
	}
	
	add { arg obj;
			var index;
			index = if(indices.isEmpty, { 0 }, { indices.last  + 1 });
			array = array.add(obj);
			indices = indices.add(index);
	}
	
		
	removeAt { arg index;
		var realIndex;
		realIndex = this.slotOf(index);
		^if(realIndex.notNil, {
			indices.removeAt(realIndex);
			array.removeAt(realIndex);
		}, {
			nil
		})
	}
	
	remove { arg item;
		var index, res;
		index = array.indexOf(item);
		^if(index.notNil, {
			indices.removeAt(index);
			res = array.removeAt(index);
		});
	}
	
	pop {
		indices.pop;
		^array.pop
	}
	
	makeEmpty { arg func;
		var item;
		while { item = this.pop; item.notNil } { func.value(item) }
	}
	
	at { arg index;
		var realIndex;
		realIndex = this.slotOf(index);
		^if(realIndex.notNil, {
			array.at(realIndex)
		})
	}
	
	put { arg index, obj;
		var nextIndex, nextSlot;
		index = index.asInteger;
		nextSlot = this.findSlotFor(index);
		if(nextSlot.isNil, {
			array = array.add(obj);
			indices = indices.add(index);
		}, {
			nextIndex = indices.at(nextSlot);
			if(nextIndex === index, {
				array.put(nextSlot, obj) //replace existing object
			}, {
				array = array.insert(nextSlot, obj); //insert into order
				indices = indices.insert(nextSlot, index);
			})
		})
			
	}
	
	doFrom { arg func, index=0;
		var slot;
		slot = this.findSlotFor(index);
		if(slot.notNil) {
			for(slot, indices.size-1, { arg i;
				func.value(array[i],i)
			})
		}
	}
		
	slotOf { arg index;
		var slot;
		slot = this.findSlotFor(index);
		^if(slot.isNil or: { index !== indices[slot] }) { nil } { slot }
	}

	findSlotFor { arg index; //including the same one.
		var max, slot=0, j;
		if(indices.isEmpty) { ^nil };
		j = indices[index];
		if(j.notNil) {
			if(j === index) { ^j } { // most common possibility.
				if(j < index) { slot = index }; // else start search here.
			}
		};
	
		max = indices.size;
		while {  slot < max } {  
			if(indices[slot] >= index){ ^slot };
			slot = slot + 1;
		}
		^nil
	}
	
	detect { arg function, start;
		var slot, max, elem;
		slot = if(start.isNil) { 0 } { this.findSlotFor(start) };
		max = indices.size - 1;
		if(slot.isNil or: { slot > max}) { ^nil };
		while { slot <= max } {
			elem = array[slot];
			if (function.value(elem, slot)) { ^elem };
			slot = slot + 1;
		};
		^nil
	}
	
	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy)
	}
	
	do { arg function;
		array.do(function)
	}
	size { ^indices.size }
	keysValuesDo { arg func;
		indices.do { arg index, i;
			func.value(index, array[i], i)
		}
	}
		
	collect { arg func;
		var res;
		res = this.class.new.indices_(indices.copy);
		res.array = array.collect(func);
		^res
	}
	
	select { arg func;
		^this.copy.selectInPlace(func);
	}
	
	reject { arg func;
		^this.copy.rejectInPlace(func);
	}
	
	collectInPlace { arg function;
		array = array.collect(function)
	}
	
	selectInPlace { arg function;
		var count=0;
		array = array.select({ arg elem, i; 
			var bool;
			bool = function.value(elem,i);
			if(bool.not, { indices.removeAt(i-count); count = count + 1 });
			bool
		});
	}
	
	rejectInPlace { arg function;
		var count=0;
		array = array.reject({ arg elem, i; 
			var bool;
			bool = function.value(elem,i);
			if(bool, { indices.removeAt(i-count); count = count + 1  });
			bool
		});
	}
	

}

