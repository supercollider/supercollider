Dictionary : Set {

	*new { arg n=8; ^super.new(n*2) }
	*newFrom { arg aCollection;
		var newCollection = this.new(aCollection.size);
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
	matchAt { |key|
		this.keysValuesDo({ |k, v|
			if(k.matchItem(key)) { ^v }
		});
		^nil
	}
	trueAt { arg key;
		^this.at(key) ? false
	}
	add { arg anAssociation;
		this.put(anAssociation.key, anAssociation.value);
	}
	put { arg key, value;
		var atKey;
		var index;
		value ?? { this.removeAt(key); ^this };
		index = this.scanFor(key);
		array.put(index+1, value);
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
	putPairs { arg args;
		args.pairsDo { |key, val| this.put(key, val) }
	}
	getPairs { arg args;
		var result;
		args = args ?? { this.keys };
		args.do { |key|
			var val = this.at(key);
			val !? { result = result.add(key).add(val) }
		};
		^result
	}

	associationAt { arg key;
		var index = this.scanFor(key);
		if (index >= 0, {
			^Association.new(array.at(index), array.at(index+1));
		},{
			^nil
		});
	}
	associationAtFail { arg argKey, function;
		var index = this.scanFor(argKey);
		var key = array.at(index);
		if ( key.isNil, { ^function.value }, {
			^Association.new(key, array.at(index+1)) });
	}
	keys { arg species(Set);
		var set = species.new(size);
		this.keysDo({ arg key; set.add(key) });
		^set
	}
	values {
		var list = List.new(size);
		this.do({ arg value; list.add(value) });
		^list
	}

	// testing
	includes { arg item1;
		this.do({ arg item2; if (item1 == item2, {^true}) });
		^false
	}
	includesKey { arg key;
		^this.at( key ).notNil;
	}

	// removing
	removeAt { arg key;
		var val;
		var index = this.scanFor(key);
		var atKeyIndex = array.at(index);
		if ( atKeyIndex.isNil, { ^nil });
		val = array.at(index+1);
		array.put(index, nil);
		array.put(index+1, nil);
		size = size - 1;
		this.fixCollisionsFrom(index);
		^val
	}
	removeAtFail { arg key, function;
		var val;
		var index = this.scanFor(key);
		var atKeyIndex = array.at(index);
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
	pairsDo { arg function;
		this.keysValuesArrayDo(array, function);

	}
	collect { arg function;
		var res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; res.put(key, function.value(elem, key)) }
		^res;
	}
	select { arg function;
		var res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; if(function.value(elem, key)) { res.put(key, elem) } }
		^res;
	}
	reject { arg function;
		var res = this.class.new(this.size);
		this.keysValuesDo { arg key, elem; if(function.value(elem, key).not)
			{ res.put(key, elem) } }
		^res;
	}

	invert {
		var dict = this.class.new(this.size);
		this.keysValuesDo {|key, val|
			dict.put(val, key)
		};
		^dict
	}

	merge {|that, func, fill = true|
		var commonKeys, myKeys = this.keys, otherKeys = that.keys;
		var res = ();

		if (myKeys == otherKeys) {
			commonKeys = myKeys
		} {
			commonKeys = myKeys.sect(otherKeys);
		};

		commonKeys.do { |key|
			res[key] = func.value(this[key], that[key], key)
		};

		if (fill) {
			myKeys.difference(otherKeys).do { |key| res[key] = this[key] };
			otherKeys.difference(myKeys).do { |key| res[key] = that[key] };
		};
		^res
	}

	blend { |that, blend = 0.5, fill = true, specs|

		^this.merge(that, { |a, b, key|
			var spec = if (specs.notNil) { specs[key].asSpec };
			if (spec.notNil) {
				spec.map(blend(spec.unmap(a), spec.unmap(b), blend))
			} {
				blend(a, b, blend)
			}
		}, fill)
	}

	findKeyForValue { arg argValue;
		this.keysValuesArrayDo(array, { arg key, val, i;
			if (argValue == val, { ^key })
		});
		^nil
	}

	sortedKeysValuesDo { arg function, sortFunc;
		var keys = this.keys(Array);
		keys.sort(sortFunc);

		keys.do { arg key, i;
			function.value(key, this[key], i);
		};
	}

	choose {
		var index, key, val;
		if( this.isEmpty, { ^nil }); // empty dictionary
		while({
			index = (array.size >> 1).rand << 1; // generate an even index.
			array.at(index).isNil;			  // key is at even index.
		});
		// return the value for the first non Nil key we find.
		// the value is at the odd index.
		^array.at(index + 1);
	}
	order { arg func;
		var assoc;
		if( this.isEmpty, { ^nil });
		this.keysValuesDo { arg key, val;
			assoc = assoc.add(key -> val);
		};
		^assoc.sort(func).collect(_.key)
	}
	powerset {
		var keys = this.keys.asArray.powerset;
		^keys.collect { | list |
			var dict = this.class.new;
			list.do { |key| dict.put(key, this[key]) };
			dict
		}
	}

	// Pattern support
	transformEvent { arg event;
		^event.putAll(this);
	}
	embedInStream { arg event;
		^yield(event !? { event.copy.putAll(this) })
	}

	asSortedArray {
		var array;
		if ( this.notEmpty ){
			this.keysValuesDo({ arg key, value; array = array.add([key,value]); });
			array = array.sort({ arg a, b; a.at(0) < b.at(0) });
		}{
			array = [];
		};
		^array;
	}
	asKeyValuePairs {
		var array = Array.new(this.size * 2);
		this.keysValuesDo { |key, val| array.add(key); array.add(val) };
		^array
	}

	// PRIVATE IMPLEMENTATION
	keysValuesArrayDo { arg argArray, function;
		// special byte codes inserted by compiler for this method
		var i=0, j=0, key, val;
		var arraySize = argArray.size;
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
		var index;
		var oldElements = array;
		array = Array.newClear(array.size * 2);
		this.keysValuesArrayDo(oldElements,
		{ arg key, val;
			index = this.scanFor(key);
			array.put(index, key);
			array.put(index+1, val);
		});
	}
	fixCollisionsFrom { arg index;
		var newIndex, key;

		var oldIndex = index;
		var lastKeyIndex = array.size - 2;
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
		var maxHash = array.size div: 2;
		var start = (argKey.hash % maxHash) * 2;
		var end = array.size-1;
		var i = start;
		forBy( start, end, 2, { arg i;
			var key = array.at(i);
			if ( key.isNil or: { key == argKey }, { ^i });
		});
		end = start - 1;
		forBy( 0, start-2, 2, { arg i;
			var key = array.at(i);
			if ( key.isNil or: { key == argKey }, { ^i });
		});
		^-2
	}

	storeItemsOn { arg stream, itemsPerLine = 5;
		var itemsPerLinem1 = itemsPerLine - 1;
		var last = this.size - 1;
		this.associationsDo({ arg item, i;
			item.storeOn(stream);
			if (i < last, { stream.comma.space;
				if (i % itemsPerLine == itemsPerLinem1, { stream.nl.space.space });
			});
		});
	}
	printItemsOn { arg stream, itemsPerLine = 5;
		var itemsPerLinem1 = itemsPerLine - 1;
		var last = this.size - 1;
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

	at { arg key;
		_IdentDict_At
		^this.primitiveFailed
		/*^array.at(this.scanFor(key) + 1)*/
	}
	put { arg key, value;
		_IdentDict_Put
		value ?? { this.removeAt(key); ^this };
		^this.primitiveFailed
		/*
		var index, atKey;
		index = this.scanFor(key);
		array.put(index+1, value);
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
		array.put(index+1, value);
		if ( array.at(index).isNil, {
			array.put(index, key);
			size = size + 1;
			if (array.size < (size * 4), { this.grow });
		});
		^prev
		*/
	}

	includesKey { arg key;
		^this.at( key ).notNil;
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
				if(this.respondsTo(selector)) {
					warn(selector.asCompileString
						+ "exists a method name, so you can't use it as pseudo-method.")
				};
				^this[selector] = args[0];
			};
			func = this[\forward];
			if (func.notNil) {
				^func.functionPerformList(\value, this, selector, args);
			};
			^nil
		};
		^this.superPerformList(\doesNotUnderstand, selector, args);
	}

		// Quant support.
		// The Quant class assumes the quant/phase/offset scheduling model.
		// If you want a different model, you can write a dictionary like so:
		// (nextTimeOnGrid: { |self, clock| ... calculate absolute beat number here ... },
		//	parameter: value, parameter: value, etc.)
		// If you leave out the nextTimeOnGrid function, fallback to quant/phase/offset.

	nextTimeOnGrid { |clock|
		if(this[\nextTimeOnGrid].notNil) {
			^this[\nextTimeOnGrid].value(this, clock)
		} {
			^clock.nextTimeOnGrid(this[\quant] ? 1, (this[\phase] ? 0) - (this[\offset] ? 0))
		}
	}
	asQuant { ^this.copy }
	timingOffset { ^this[\timingOffset] }		// for synchWithQuant()
}
