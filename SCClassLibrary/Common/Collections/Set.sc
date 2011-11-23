Set : Collection {
	var <>array, <size=0;

	*new { arg n=2; ^super.new.initSet(max(n,2)*2) }
	species { ^this.class }
	copy { ^this.shallowCopy.array_( array.copy ) }
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

	clear { array.fill; size=0 }
	makeEmpty { this.clear; }

	includes { arg item;
		^array.at(this.scanFor(item)).notNil;
	}
	findMatch { arg item;
		// return an item that matches a given item
		^array.at(this.scanFor(item));
	}
	add { arg item;
		var index;
		if (item.isNil, { Error("A Set cannot contain nil.\n").throw; });
		index = this.scanFor(item);
		if ( array.at(index).isNil, { this.putCheck(index, item) });
	}
	remove { arg item;
		var index = this.scanFor(item);
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
			(index < array.size) and: { (val = array.at(index)).isNil }
		},{
			index = index + 1
		});
		if (index < array.size, {
			this.remove(val);
			^val
		}, {
			^nil
		});
	}
//	powerset {
//		^this.asArray.sort.powerset
//	}
	unify {
		var result = this.species.new;
		this.do {|x| result.addAll(x) }
		^result
	}
	sect { arg that;
		var result = this.species.new;
		this.do({ arg item;
			if (that.includes(item), {
				result.add(item);
			});
		});
		^result
	}
	union { arg that;
		var result = this.species.new;
		result.addAll(this);
		result.addAll(that);
		^result
	}
	difference { arg that;
		^this.copy.removeAll(that);
	}
	symmetricDifference { arg that;
		var result = this.species.new;
		this.do({ arg item;
			if (that.includes(item).not, {
				result.add(item);
			});
		});
		that.do({ arg item;
			if (this.includes(item).not, {
				result.add(item);
			});
		});
		^result;
	}
	isSubsetOf { | that | ^that.includesAll(this) }

	& { arg that; ^this.sect(that) }
	| { arg that; ^this.union(that) }
	- { arg that; ^this.difference(that) }
	-- { arg that; ^this.symmetricDifference(that) }


	// PRIVATE IMPLEMENTATION
	initSet { arg n; array = Array.newClear(n); size = 0; }
	putCheck { arg index, item;
		array.put(index, item);
		size = size + 1;
		this.fullCheck;
	}
	fullCheck {
		if (array.size < (size * 2), { this.grow });
	}
	grow {
		var oldElements = array;
		array = Array.newClear(array.size * 2);
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
		var elem;
		var start = obj.hash % array.size;
		var end = array.size;
		var i = start;

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
		var newIndex, element;
		var oldIndex = index;
		var lastKeyIndex = array.size - 1;

		while ({
			if (oldIndex == lastKeyIndex, { oldIndex = 0 }, { oldIndex = oldIndex + 1 });
			(element = this.keyAt(oldIndex)).notNil
		},{
			newIndex = this.scanFor(element);
			if ( oldIndex != newIndex, { array.swap(oldIndex, newIndex) })
		})
	}
	keyAt { arg index; ^array.at(index) }

	asSet { ^this }
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


OrderedIdentitySet : IdentitySet {
	var >items;

	copy {
		^this.shallowCopy
		.array_( array.copy )
		.items_( items.copy )
	}

	do { arg function;
		items.do(function)
	}

	clear {
		super.clear;
		items = nil;
	}

	remove { arg item;
		super.remove(item);
		items.remove(item);
	}
	sort { arg func;
		items.sort(func)
	}

	// private

	putCheck { arg index, item;
		super.putCheck(index, item);
		items = items.add(item);
	}
}
