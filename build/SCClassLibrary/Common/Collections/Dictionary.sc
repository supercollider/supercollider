Dictionary : Set {
		
	*new { arg n=3; ^super.new(n*2) }
	*newFrom { arg aCollection;
		var newCollection;
		newCollection = this.new(aCollection.size);
		aCollection.keysValuesDo({ arg k,v, i; newCollection.put(k,v) });
		^newCollection
	}
	// accessing
	at { arg key;
		^array.at(this.scanFor(key) + 1)
	}
	atFail { arg key, function;
		var val;
		val = array.at(this.scanFor(key) + 1);
		if ( val.isNil, { ^function.value }, { ^val });
	}
	add { arg anAssociation;
		this.put(anAssociation.key, anAssociation.value);
	}
	put { arg key, obj;
		var index, atKey;
		index = this.scanFor(key);
		array.put(index+1, obj);
		if ( array.at(index).isNil, {
			array.put(index, key);
			size = size + 1;
			if (array.size < (size * 3), { this.grow });
		});
	}
	putAll { arg aDictionary; 
		aDictionary.keysValuesDo({ arg key, value; 
			this.put(key, value) 
		}) 
	}
	associationAt { arg key;
		var index, anAssociation;
		index = this.scanFor(key);
		if (index >= 0, {
			^Association.new(array.at(index), array.at(index+1));
		},{
			^nil
		});
	}
	associationAtFail { arg argKey, function;
		var index, key;
		index = this.scanFor(argKey);
		key = array.at(index);
		if ( key.isNil, { ^function.value }, { 
			^Association.new(key, array.at(index+1)) });
	}
	keys {
		var set;
		set = Set.new(size);
		this.keysDo({ arg key; set.add(key) });
		^set
	}
	values {
		var list;
		list = List.new(size);
		this.do({ arg value; list.add(value) });
		^list
	}
	
	// testing
	includes { arg item1; 
		this.do({ arg item2; if (item1 == item2, {^true}) });
		^false
	}
	includesKey { arg item1; 
		this.keysDo({ arg item2; if (item1 == item2, {^true}) });
		^false
	}
	
	// removing
	removeAt { arg key;
		var index, atKeyIndex, val;
		index = this.scanFor(key);
		atKeyIndex = array.at(index);
		if ( atKeyIndex.isNil, { ^nil });
		val = array.at(index+1);
		array.put(index, nil);
		array.put(index+1, nil);
		size = size - 1;
		this.fixCollisionsFrom(index);
		^val
	}	
	removeAtFail { arg key, function;
		var index, atKeyIndex, val;
		index = this.scanFor(key);
		atKeyIndex = array.at(index);
		if ( atKeyIndex.isNil, { ^function.value });
		val = array.at(index+1);
		array.put(index, nil);
		array.put(index+1, nil);
		size = size - 1;
		this.fixCollisionsFrom(index);
		^val
	}	
		
	remove { ^this.shouldNotImplement(thisMethod) }
	removeFail { ^this.shouldNotImplement(thisMethod) }
	
	// enumerating
	keysValuesDo { arg function;
		this.keysValuesArrayDo(array, function);
	}
	keysValuesChange { arg function;
		this.keysValuesDo({ arg key, value, i;
			this.put(key, function.value(key, value, i));
		})
	}
	do { arg function;
		this.keysValuesDo({ arg key, value, i;
			function.value(value, i);
		})
	}
	keysDo { arg function;
		this.keysValuesDo({ arg key, val, i;
			function.value(key, i);
		})
	}
	associationsDo { arg function;
		this.keysValuesDo({ arg key, val, i;
			function.value( Association.new(key, val), i);
		})
	}

	findKeyForValue { arg argValue;
		this.keysValuesArrayDo(array, { arg key, val, i;
			if (argValue == val, { ^key })
		});
		^nil
	}
	
    choose {
        var index, key, val;
        if (this.size <= 0, { ^nil }); // empty dictionary
        while({
            index = (array.size >> 1).rand << 1; // generate an even index.
            array.at(index).isNil;              // key is at even index.
        });
        // return the value for the first non Nil key we find.
        // the value is at the odd index.
        ^array.at(index + 1); 
    }
	
	// Pattern support
	transformEvent { arg event;
		^event.putAll(this);
	}
	embedInStream { arg event;
		^event.putAll(this).yield;
	}
	
	asSortedArray {
		var array;
		this.keysValuesDo({ arg key, value; array = array.add([key,value]); });
		array = array.sort({ arg a, b; a.at(0) < b.at(0) });
		^array.flop;
	}
	
	// PRIVATE IMPLEMENTATION
	keysValuesArrayDo { arg argArray, function;
		// special byte codes inserted by compiler for this method
		var i=0, j=0, arraySize, key, val;
		arraySize = argArray.size;			
		while ({ i < arraySize },{
			key = argArray.at(i);
			if (key.notNil, {
				val = argArray.at(i+1);
				function.value(key, val, j);
				j = j + 1;
			});
			i = i + 2;
		});
	}
	*sizeFor { arg n=3; ^if (n < 0, {1}, { n * 3 }) }
	grow {
		var oldElements, index;
		oldElements = array;
		array = Array.newClear(array.size + this.growSize);
		this.keysValuesArrayDo(oldElements, 
		{ arg key, val;
			index = this.scanFor(key);
			array.put(index, key);
			array.put(index+1, val);
		});
	}
	fixCollisionsFrom { arg index; 
		var lastKeyIndex, oldIndex, newIndex, key;

		oldIndex = index;
		lastKeyIndex = array.size - 2;
		while ({
			if (oldIndex == lastKeyIndex, { oldIndex = 0 }, { oldIndex = oldIndex + 2 });
			(key = array.at(oldIndex)).notNil
		},{
			newIndex = this.scanFor(key);
			if ( oldIndex != newIndex, { 
				this.swap(oldIndex, newIndex);
				this.swap(oldIndex+1, newIndex+1) 
			})
		})
	}
	scanFor { arg argKey;
		var i, start, end, key, maxHash;
		
		maxHash = array.size div: 2;
		start = (argKey.hash % maxHash) * 2;
		end = array.size-1;
		i = start;
		forBy( start, end, 2, { arg i;
			key = array.at(i);
			if ( key.isNil or: { key == argKey }, { ^i });
		});
		end = start - 1;
		forBy( 0, start-2, 2, { arg i;
			key = array.at(i);
			if ( key.isNil or: { key == argKey }, { ^i });
		});
		^-2
	}
	
	storeItemsOn { arg stream, itemsPerLine = 5;
		var last, itemsPerLinem1;
		itemsPerLinem1 = itemsPerLine - 1;
		last = this.size - 1;
		this.associationsDo({ arg item, i;
			item.storeOn(stream);
			if (i < last, { stream.comma.space;
				if (i % itemsPerLine == itemsPerLinem1, { stream.nl.space.space });
			});
		});
	}
	printItemsOn { arg stream, itemsPerLine = 5;
		var last, itemsPerLinem1;
		itemsPerLinem1 = itemsPerLine - 1;
		last = this.size - 1;
		this.associationsDo({ arg item, i;
			item.printOn(stream);
			if (i < last, { stream.comma.space;
				if (i % itemsPerLine == itemsPerLinem1, { stream.nl.space.space });
			});
		});
	}
}

IdentityDictionary : Dictionary {
	at { arg key, value;
		_IdentDict_At
		^this.primitiveFailed
		/*^array.at(this.scanFor(key) + 1)*/
	}
	put { arg key, value;
		_IdentDict_Put
		^this.primitiveFailed
		/*
		var index, atKey;
		index = this.scanFor(key);
		array.put(index+1, obj);
		if ( array.at(index).isNil, {
			array.put(index, key);
			size = size + 1;
			if (array.size < (size * 3), { this.grow });
		});
		*/
	}
	putGet { arg key, value;
		_IdentDict_PutGet
		^this.primitiveFailed
		/*
		var index, atKey, prev;
		index = this.scanFor(key);
		prev = array.at(index);
		array.put(index+1, obj);
		if ( array.at(index).isNil, {
			array.put(index, key);
			size = size + 1;
			if (array.size < (size * 3), { this.grow });
		});
		^prev
		*/
	}
	findKeyForValue { arg argValue;
		this.keysValuesArrayDo(array, { arg key, val, i;
			if (argValue === val, { ^key })
		});
		^nil
	}
	scanFor { arg argKey;
		^array.atIdentityHashInPairs(argKey)
/*
		var i, start, end, key, maxHash;
		
		maxHash = array.size div: 2;
		start = (argKey.identityHash % maxHash) * 2;
		end = array.size;
		i = start;
		while ({ i < end }, {
			key = array.at(i);
			if ( key.isNil or: { key === argKey }, { ^i });
			i = i + 2;
		});
		end = start - 1;
		i = 0;
		while ({ i < end }, {
			key = array.at(i);
			if ( key.isNil or: { key === argKey }, { ^i });
			i = i + 2;
		});
		^-2
*/
	}
	
	// not the fastest way, but the simplest
	writeAsPlist { arg path;
		this.as(Dictionary).writeAsPlist(path);
	}
	
}

NameDictionary : IdentityDictionary
{
	add { arg aNamedObject;
		this.put(aNamedObject.name, aNamedObject);
	}
}

