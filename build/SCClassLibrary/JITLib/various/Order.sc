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
		realIndex = indices.indexOf(index);
		^if(realIndex.notNil, {
			indices.removeAt(realIndex);
			array.removeAt(realIndex);
		}, {
			nil
		})
	}
	
	remove { arg item;
		var index;
		index = array.indexOf(item);
		if(index.notNil, {
			indices.removeAt(index);
			array.removeAt(index);
		})
	}
	
	pop {
		indices.pop;
		^array.pop
	}
	
	at { arg index;
		var realIndex;
		realIndex = indices.indexOf(index);
		^if(realIndex.notNil, {
			array.at(realIndex)
		})
	}
	
	
	put { arg index, obj;
		var realIndex, search;
		if(indices.notEmpty and: { index <= indices.last }, {
			realIndex = indices.detectIndex({ arg item; search = item; item >= index });
		});
		if(realIndex.isNil, {
			array = array.add(obj);
			indices = indices.add(index);
		}, {
			if(search == index, {
				array.put(realIndex, obj) //replace existing object
			}, {
				array = array.insert(realIndex, obj); //insert into order
				indices = indices.insert(realIndex, index);
			})
		})
			
	}
	
	copy {
		 ^this.class.newCopyArgs(array.copy, indices.copy)
	}
	
	do { arg function;
		array.do(function)
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
		array = array.select({ arg elem, i; 
			var bool;
			bool = function.value(elem,i);
			if(bool.not, { indices.removeAt(i) });
			bool
		});
	}
	
	rejectInPlace { arg function;
		array = array.reject({ arg elem, i; 
			var bool;
			bool = function.value(elem,i);
			if(bool, { indices.removeAt(i) });
			bool
		});
	}
	

}