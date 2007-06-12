Set : Collection {
	var <>array, <size=0;
	
	*new { arg n=3; ^super.new.initSet(this.sizeFor(n)) }
	*sizeFor { arg n=3; ^if (n < 0, {1}, { n * 3 div: 2 }) }
	species { ^this.class }
	copy { ^this.shallowCopy.array_( array.copy ) }
	makeEmpty { array.fill; }
	do { arg function;
		var i=0;
		if ( size > 0, {
			array.do({ arg item;
				if (item.notNil, {
					function.value(item, i);
					i = i + 1;
				})
			})
		})
	}
	
	includes { arg item; 
		^array.at(this.scanFor(item)).notNil;
	}
	findMatch { arg item; 
		// return an item that matches a given item
		^array.at(this.scanFor(item));
	}
	add { arg item;
		var index;
		if (item.isNil, { "A Set cannot contain nil.\n".error; this.halt; });
		index = this.scanFor(item);
		if ( array.at(index).isNil, { this.putCheck(index, item) });
	}
	remove { arg item;
		var index;
		index = this.scanFor(item);
		if ( array.at(index).notNil, {
			array.put(index, nil);
			size = size - 1;
			this.fixCollisionsFrom(index);
		});
	}
	choose {
		var index, val;
		if (size <= 0, { ^nil });
		while({
			index = array.size.rand;
			(val = array.at(index)).isNil;
		});
		^val
	}
	pop {
		var index = 0, val;
		while({
			(val = array.at(index)).isNil;
		},{ 
			index = index + 1 
		});
		this.remove(val);
		^val
	}
	
	sect { arg that;
		var result;
		result = Set.new;
		this.do({ arg item;
			if (that.includes(item), {
				result.add(item);
			});
		});
		^result
	}
	union { arg that;
		var result;
		result = Set.new;
		result.addAll(this);
		result.addAll(that);
		^result
	}
	
	// PRIVATE IMPLEMENTATION
	initSet { arg n; array = Array.newClear(n); }
	putCheck { arg index, item;
		array.put(index, item);
		size = size + 1;
		this.fullCheck;
	}
	fullCheck {
		//[\fullCheck, array.size, size * 3 div: 2, size].postln;
		if (array.size < (size * 3 div: 2), { this.grow });
	}
	growSize { ^max( array.size, 2 ) }
	grow {
		var oldElements;
		oldElements = array;
		array = Array.newClear(array.size + this.growSize);
		size = 0;
		oldElements.do({ arg item;
			if ( item.notNil, { this.noCheckAdd(item) })
		});
	}
	noCheckAdd { arg item;
		array.put(this.scanFor(item), item);
		size = size + 1;
	}
	scanFor { arg obj;
		var i, start, end, elem;
		
		start = obj.hash % array.size;
		end = array.size;
		i = start;
		while ({ i < end }, {
			elem = array.at(i);
			
			if ( elem.isNil or: { elem == obj }, { ^i });
			i = i + 1;
		});
		end = start - 1;
		i = 0;
		while ({ i <= end }, {
			elem = array.at(i);
			if ( elem.isNil or: { elem == obj }, { ^i });
			i = i + 1;
		});
		error("There is no free space in this set!\n");
		array.postln;
		^-1
	}
	
	fixCollisionsFrom { arg index; 
		var lastKeyIndex, oldIndex, newIndex, element;

		oldIndex = index;
		lastKeyIndex = array.size - 1;
		while ({
			if (oldIndex == lastKeyIndex, { oldIndex = 0 }, { oldIndex = oldIndex + 1 });
			(element = this.keyAt(oldIndex)).notNil
		},{
			newIndex = this.scanFor(element);
			if ( oldIndex != newIndex, { this.swap(oldIndex, newIndex) })
		})
	}
	swap { arg i, j; array.swap(i, j) }
	keyAt { arg index; ^array.at(index) }
	
}

IdentitySet : Set {
	scanFor { arg argKey;
		^array.atIdentityHash(argKey)
	/*
		var i, start, end, elem;
		
		start = obj.identityHash % array.size;
		end = array.size;
		i = start;
		while ({ i < end }, {
			elem = array.at(i);
			if ( elem.isNil or: { elem === obj }, { ^i });
			i = i + 1;
		});
		end = start - 1;
		i = 0;
		while ({ i < end }, {
			elem = array.at(i);
			if ( elem.isNil or: { elem === obj }, { ^i });
			i = i + 1;
		});
		^-1
	*/
	}
}