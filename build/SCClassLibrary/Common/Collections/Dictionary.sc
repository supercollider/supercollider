Dictionary : Set {
		
	*new { arg n=8; ^super.new(n*2) }
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
		val = this.at(key);
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
			if (array.size < (size * 4), { this.grow });
		});
	}
	putAll { arg ... dictionaries; 
		dictionaries.do {|dict| 
			dict.keysValuesDo { arg key, value; 
				this.put(key, value) 
			}
		}
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
		arg species;
		var set;
		species = species ? Set;
		set = species.new(size);
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
	collect { arg function;
		var i, res;
		res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; res.put(key, function.value(elem, key)) }
		^res;
	}
	select { arg function;
		var i, res;
		res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; if(function.value(elem, key)) { res.put(key, elem) } }
		^res;
	}
	reject { arg function;
		var i, res;
		res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; if(function.value(elem, key).not) 
			{ res.put(key, elem) } }
		^res;
	}



	findKeyForValue { arg argValue;
		this.keysValuesArrayDo(array, { arg key, val, i;
			if (argValue == val, { ^key })
		});
		^nil
	}

	sortedKeysValuesDo {
		arg function, sortFunc;
		var keys;

		keys = this.keys(Array);
		keys.sort(sortFunc);
		
		keys.do { arg key, i;
			function.value(key, this[key], i);
		};
	}

	choose {
		var index, key, val;
		if (this.size <= 0, { ^nil }); // empty dictionary
		while({
			index = (array.size >> 1).rand << 1; // generate an even index.
			array.at(index).isNil;			  // key is at even index.
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
		^event.copy.putAll(this).yield
	}
	
	asSortedArray {
		var array;
		this.keysValuesDo({ arg key, value; array = array.add([key,value]); });
		array = array.sort({ arg a, b; a.at(0) < b.at(0) });
		^array;
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
	grow {
		var oldElements, index;
		oldElements = array;
		array = Array.newClear(array.size * 2);
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
				array.swap(oldIndex, newIndex);
				array.swap(oldIndex+1, newIndex+1) 
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
	var <>proto; // inheritance of properties
	var <>parent; // inheritance of properties
	
	var <>know = false; 
	// if know is set to true then not understood messages will look in the dictionary
	// for that selector and send the value message to them.
	
	*new { arg n=8, proto, parent, know=false; 
		^super.new(n).proto_(proto).parent_(parent).know_(know)
	}
	
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
			if (array.size < (size * 4), { this.grow });
		});
		*/
	}
	putGet { arg key, value;
		_IdentDict_PutGet
		^this.primitiveFailed
		/*
		var index, atKey, prev;
		index = this.scanFor(key);
		prev = array.at(index + 1);
		array.put(index+1, obj);
		if ( array.at(index).isNil, {
			array.put(index, key);
			size = size + 1;
			if (array.size < (size * 4), { this.grow });
		});
		^prev
		*/
	}
	
	includesKey { arg item1; 
		this.keysDo({ arg item2; if (item1 === item2, {^true}) });
		^false
	}
	findKeyForValue { arg argValue;
		this.keysValuesArrayDo(array, { arg key, val, i;
			if (argValue === val, { ^key })
		});
		^nil
	}
	scanFor { arg argKey;
		^array.atIdentityHashInPairs(argKey)
	}

	doesNotUnderstand { arg selector ... args;
		var func;
		if (know) {
			func = this[selector];
			if (func.notNil) {
				^func.functionPerformList(\value, this, args);
			};
			if (selector.isSetter) {
				selector = selector.asGetter;
				^this[selector] = args[0];
			};
			func = this[\forward];
			if (func.notNil) {
				^func.functionPerformList(\value, this, selector, args);
			};
		};

		^this.superPerformList(\doesNotUnderstand, selector, args);
	}
	
	// not the fastest way, but the simplest
	writeAsPlist { arg path;
		this.as(Dictionary).writeAsPlist(path);
	}
	
}

