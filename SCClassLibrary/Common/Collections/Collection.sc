Collection {
	*newFrom { | aCollection |
		var newCollection = this.new(aCollection.size);
		aCollection.do {| item | newCollection.add(item) };
		^newCollection
	}
	*with { | ... args |
		var newColl;
		// answer a collection of my class of the given arguments
		// the class Array has a simpler implementation
		newColl = this.new(args.size);
		newColl.addAll(args);
		^newColl
	}
	*fill { | size, function |
		var obj;
		if(size.isSequenceableCollection) { ^this.fillND(size, function) };
		obj = this.new(size);
		size.do { | i |
			obj.add(function.value(i));
		};
		^obj
	}
	*fill2D { | rows, cols, function |
		var obj = this.new(rows);
		rows.do { |row|
			var obj2 = this.new(cols);
			cols.do { |col|
				obj2 = obj2.add(function.value(row, col))
			};
			obj = obj.add(obj2);
		};
		^obj
	}
	*fill3D { | planes, rows, cols, function |
		var obj = this.new(planes);
		planes.do { |plane|
			var obj2 = this.new(rows);
			rows.do { |row|
				var obj3 = this.new(cols);
				cols.do { |col|
					obj3 = obj3.add(function.value(plane, row, col))
				};
				obj2 = obj2.add(obj3);
			};
			obj = obj.add(obj2);
		};
		^obj
	}
	*fillND { | dimensions, function, args = #[] | // args are private
		var n = dimensions.first;
		var obj = this.new(n);
		var argIndex = args.size;
		args = args ++ 0;
		if(dimensions.size <= 1) {
			n.do { |i| obj.add(function.valueArray(args.put(argIndex, i))) }
		} {
			dimensions = dimensions.drop(1);
			n.do { |i|
				obj = obj.add(this.fillND(dimensions, function, args.put(argIndex, i)))
			}
		};
		^obj
	}

	@ { | index | ^this[index] }

	== { | aCollection |
		if (aCollection.class != this.class) { ^false };
		if (this.size != aCollection.size) { ^false };
		this.do { | item, i |
			// this is enough since both collections are finite
			if ((aCollection.includes(item)).not) { ^false };
		};
		^true
	}
	hash {
		var hash = this.class.hash;
		this.do { | item |
			hash = hash bitXor: item.hash;
		};
		^hash
	}

	species { ^Array }
	do { ^this.subclassResponsibility(thisMethod) }
	iter {
		^r { this.do {|item| item.yield } }
	}
	size {
		// this is the slow way. Most collections have a faster way.
		var tally = 0;
		this.do { tally = tally + 1 };
		^tally
	}
	flatSize {
		^this.sum(_.flatSize)
	}

	isEmpty { ^this.size == 0 }
	notEmpty { ^this.size > 0 }
	asCollection { ^this }
	isCollection { ^true }

	add { ^this.subclassResponsibility(thisMethod) }
	addAll { | aCollection | aCollection.asCollection.do { | item | this.add(item) } }
	remove { ^this.subclassResponsibility(thisMethod) }
	removeAll { | list | list.do { | item | this.remove(item) } }
	removeEvery { | list | this.removeAllSuchThat(list.includes(_)) }
	removeAllSuchThat { | function |
		var removedItems = this.class.new;
		var copy = this.copy;
		copy.do { | item, i |
			if ( function.value(item, i) )
			{
				this.remove(item);
				removedItems = removedItems.add(item);
			}
		};
		^removedItems
	}

	atAll { arg keys;
		^keys.collect {|index| this[index] }
	}
	putEach { arg keys, values;
		// works for ArrayedCollections and Dictionaries
		keys = keys.asArray;
		values = values.asArray;
		keys.do { |key, i| this[key] = values.wrapAt(i) } ;
	}

	includes { | item1 |
		this.do {|item2| if (item1 === item2) {^true} };
		^false
	}
	includesEqual { | item1 |
		this.do {|item2| if (item1 == item2) {^true} };
		^false
	}
	includesAny { | aCollection |
		aCollection.do { | item | if (this.includes(item)) {^true} };
		^false
	}
	includesAll { | aCollection |
		aCollection.do { | item | if (this.includes(item).not) {^false} };
		^true
	}
	matchItem { | item |
		^this.includes(item)
	}

	collect { | function |
		^this.collectAs(function, this.species);
	}
	select { | function |
		^this.selectAs(function, this.species);
	}
	reject { | function |
		^this.rejectAs(function, this.species);
	}
	collectAs { | function, class |
		var res = class.new(this.size);
		this.do {|elem, i| res.add(function.value(elem, i)) }
		^res;
	}
	selectAs { | function, class |
		var res = class.new(this.size);
		this.do {|elem, i| if (function.value(elem, i)) { res = res.add(elem) } }
		^res;
	}
	rejectAs { | function, class |
		var res = class.new(this.size);
		this.do {|elem, i|
			if (function.value(elem, i).not) {res.add(elem)}
		}
		^res;
	}
	detect { | function |
		this.do {|elem, i| if (function.value(elem, i)) { ^elem } }
		^nil;
	}
	detectIndex { | function |
		this.do {|elem, i| if (function.value(elem, i)) { ^i } }
		^nil;
	}
	doMsg { | selector ... args |
		this.do {| item | item.performList(selector, args) }
	}
	collectMsg { | selector ... args |
		^this.collect {| item | item.performList(selector, args) }
	}
	selectMsg { | selector ... args |
		^this.select {| item | item.performList(selector, args) }
	}
	rejectMsg { | selector ... args |
		^this.reject {| item | item.performList(selector, args) }
	}
	detectMsg { | selector ... args |
		^this.detect {| item | item.performList(selector, args) }
	}
	detectIndexMsg { | selector ... args |
		^this.detectIndex {| item | item.performList(selector, args) }
	}
	lastForWhich { | function |
		var prev;
		this.do {|elem, i|
			if (function.value(elem, i)) {
				prev = elem;
			}{
				^prev
			}
		};
		^prev
	}
	lastIndexForWhich { | function |
		var prev;
		this.do {|elem, i|
			if (function.value(elem, i)) {
				prev = i;
			}{
				^prev
			}
		};
		^prev
	}
	inject { | thisValue, function |
		var nextValue = thisValue;
		this.do { | item, i |
			nextValue = function.value(nextValue, item, i);
		};
		^nextValue
	}
	count { | function |
		var sum = 0;
		this.do {|elem, i| if (function.value(elem, i)) { sum=sum+1 } }
		^sum;
	}
	occurrencesOf { | obj |
		var sum = 0;
		this.do { | elem | if (elem == obj) { sum=sum+1 } }
		^sum;
	}
	any { | function |
		this.do {|elem, i| if (function.value(elem, i)) { ^true } }
		^false;
	}
	every { | function |
		this.do {|elem, i| if (function.value(elem, i).not) { ^false } }
		^true;
	}
	sum { | function |
		var sum = 0;
		if (function.isNil) { // optimized version if no function
			this.do { | elem | sum = sum + elem; }
		}{
			this.do {|elem, i| sum = sum + function.value(elem, i); }
		}
		^sum;
	}
	mean { | function |
		^this.sum(function) / this.size;
	}
	product { | function |
		var product = 1;
		if (function.isNil) { // optimized version if no function
			this.do { | elem | product = product * elem; }
		}{
			this.do {|elem, i| product = product * function.value(elem, i); }
		}
		^product;
	}
	sumabs {  // sum of the absolute values - used to convert Mikael Laursen's rhythm lists.
		var sum = 0;
		this.do { | elem |
			if (elem.isSequenceableCollection) { elem = elem[0] };
			sum = sum + elem.abs;
		}
		^sum;
	}

	maxItem { | function |
		var maxValue, maxElement;
		if (function.isNil) { // optimized version if no function
			this.do { | elem |
				if (maxElement.isNil) {
					maxElement = elem;
				}{
					if (elem > maxElement) {
						maxElement = elem;
					}
				}
			}
			^maxElement;
		}{
			this.do {|elem, i| var val;
				if (maxValue.isNil) {
					maxValue = function.value(elem, i);
					maxElement = elem;
				}{
					val = function.value(elem, i);
					if (val > maxValue) {
						maxValue = val;
						maxElement = elem;
					}
				}
			}
			^maxElement;
		}
	}
	minItem { | function |
		var minValue, minElement;
		if (function.isNil) { // optimized version if no function
			this.do {|elem, i|
				if (minElement.isNil) {
					minElement = elem;
				}{
					if (elem < minElement) {
						minElement = elem;
					}
				}
			};
			^minElement;
		}{
			this.do {|elem, i| var val;
				if (minValue.isNil) {
					minValue = function.value(elem, i);
					minElement = elem;
				}{
					val = function.value(elem, i);
					if (val < minValue) {
						minValue = val;
						minElement = elem;
					}
				}
			}
			^minElement;
		}
	}

	maxIndex { | function |
		var maxValue, maxIndex;
		if (function.isNil) { // optimized version if no function
			this.do { | elem, index |
				if (maxValue.isNil) {
					maxValue = elem;
					maxIndex = index;
				}{
					if (elem > maxValue) {
						maxValue = elem;
						maxIndex = index;
					}
				}
			}
			^maxIndex;
		}{
			this.do {|elem, i| var val;
				if (maxValue.isNil) {
					maxValue = function.value(elem, i);
					maxIndex = i;
				}{
					val = function.value(elem, i);
					if (val > maxValue) {
						maxValue = val;
						maxIndex = i;
					}
				}
			}
			^maxIndex;
		}
	}

	minIndex { | function |
		var minValue, minIndex;
		if (function.isNil) { // optimized version if no function
			this.do {|elem, i|
				if (minValue.isNil) {
					minValue = elem;
					minIndex = i;
				}{
					if (elem < minValue) {
						minValue = elem;
						minIndex = i;
					}
				}
			};
			^minIndex;
		}{
			this.do {|elem, i| var val;
				if (minValue.isNil) {
					minValue = function.value(elem, i);
					minIndex = i;
				}{
					val = function.value(elem, i);
					if (val < minValue) {
						minValue = val;
						minIndex = i;
					}
				}
			}
			^minIndex;
		}
	}


	maxValue { | function |			// must supply a function
		var maxValue, maxElement;
		this.do {|elem, i|
				var val;
				if (maxValue.isNil) {
					maxValue = function.value(elem, i);
					maxElement = elem;
				}{
					val = function.value(elem, i);
					if (val > maxValue) {
						maxValue = val;
						maxElement = elem;
					}
				}
		};
		^maxValue;
	}
	minValue { | function |
		var minValue, minElement;
		this.do {|elem, i|
				var val;
				if (minValue.isNil) {
					minValue = function.value(elem, i);
					minElement = elem;
				}{
					val = function.value(elem, i);
					if (val < minValue) {
						minValue = val;
						minElement = elem;
					}
				}
		};
		^minValue;
	}

	maxSizeAtDepth { arg rank;
		var maxsize = 0;
		if(rank == 0) { ^this.size };
		this.do { |sublist|
			var sz = if(sublist.isCollection)
					{ sublist.maxSizeAtDepth(rank - 1) } { 1 };
			if (sz > maxsize) { maxsize = sz };
		};
		^maxsize
	}

	maxDepth { arg max = 1;
		var res = max;
		this.do { |elem|
			if(elem.isCollection) { res = max(res, elem.maxDepth(max + 1)) }
		};
		^res
	}
	
	deepCollect { | depth = 1, function, index = 0, rank = 0 |
		if(depth.isNil) {
			rank = rank + 1;
			^this.collect { |item, i| item.deepCollect(depth, function, i, rank) }
		};
		if (depth <= 0) {
			^function.value(this, index, rank)
		};
		depth = depth - 1;
		rank = rank + 1;
		^this.collect { |item, i| item.deepCollect(depth, function, i, rank) }
	}

	deepDo { | depth = 1, function, index = 0, rank = 0 |
		if(depth.isNil) {
			rank = rank + 1;
			^this.do { |item, i| item.deepDo(depth, function, i, rank) }
		};
		if (depth <= 0) {
			function.value(this, index, rank);
			^this
		};
		depth = depth - 1;
		rank = rank + 1;
		^this.do { |item, i| item.deepDo(depth, function, i, rank) }
	}

	invert { | axis |
		var index;
		// can be used to invert a pitch list about a given axis
		// [3, 2, 9, 7].invert(11) becomes [ 19, 20, 13, 15 ]
		// if axis is nil, invert uses the registral center
		// [3, 2, 9, 7].invert becomes [ 8, 9, 2, 4 ]
		if(this.isEmpty) { ^this.species.new };
		if(axis.notNil) { index = axis * 2 } { index = this.minItem + this.maxItem };
		^index - this;
	}

	sect { | that |
		var result = this.species.new;
		this.do { | item |
			if (that.includes(item)) {
				result = result.add(item);
			}
		};
		^result
	}
	union { | that |
		var result = this.copy;
		that.do { | item |
			if (result.includes(item).not) {
				result = result.add(item);
			}
		};
		^result
	}
	difference { | that |
		^this.copy.removeAll(that);
	}
	symmetricDifference { | that |
		var result = this.species.new;
		this.do { | item |
			if (that.includes(item).not) {
				result = result.add(item);
			}
		};
		that.do { | item |
			if (this.includes(item).not) {
				result = result.add(item);
			}
		};
		^result;
	}
	isSubsetOf { | that | ^that.includesAll(this) }

	asArray { ^Array.new(this.size).addAll(this); }
	asBag { ^Bag.new(this.size).addAll(this); }
	asList { ^List.new(this.size).addAll(this); }
	asSet { ^Set.new(this.size).addAll(this); }
	asSortedList { | function | ^SortedList.new(this.size, function).addAll(this); }

	powerset {
		var species = this.species;
		var result = this.asArray.powerset;
		^if(species == Array) { result } {
			result.collectAs({ | item | item.as(species) }, species)
		}
	}

	flopDict { | unbubble=true |
		var res, first = true;
		this.do { | dict |
			if(first) { res = dict.class.new; first = false };
			dict.keysValuesDo { | key, val |
				res[key] = res[key].add(val)
			}
		};
		if(unbubble) { res = res.collect(_.unbubble) };
		^res
	}

	histo { arg steps = 100, min, max;
		var freqs, freqIndex, lastIndex, stepSize, outliers = 0;
		if(this.isEmpty) { ^this.species.new };
		min = min ?? { this.minItem };
		max = max ?? { this.maxItem };

		freqs = Array.fill(steps, 0);
		lastIndex = steps - 1;
		stepSize = steps / (max - min);

		this.do { arg el;
			freqIndex = ((el - min) * stepSize).asInteger;

			if (freqIndex.inclusivelyBetween(0, lastIndex)) {
				freqs[freqIndex] = freqs[freqIndex] + 1;
			} {
						// if max is derived from maxItem, count it in:
				if (el == max) {
					freqs[steps-1] = freqs[steps-1] + 1;
				} { 		// else it is an outlier.
					outliers =  outliers + 1;
				};
			};
		};

		if (outliers > 0) {
			("histogram :" + outliers + "out of (histogram) range values in collection.").inform;
		};

		^freqs;
	}

//	printAll { this.do { | item | item.postln; }; } // convenience method
	printAll { |before, after|
		if (before.isNil and: after.isNil) {
			this.do { | item | item.postln; };
		} {
			before = before ? ""; after = after ? "";
			this.do { | item | before.post; item.post; after.postln; };
		};
	}

	printcsAll { this.do { | item | item.postcs; }; } // convenience method
	dumpAll { this.do { | item | item.dump; }; } // convenience method

	printOn { | stream |
		if (stream.atLimit) { ^this };
		stream << this.class.name << "[ " ;
		this.printItemsOn(stream);
		stream << " ]" ;
	}
	storeOn { | stream |
		if (stream.atLimit) { ^this };
		stream << this.class.name << "[ " ;
		this.storeItemsOn(stream);
		stream << " ]" ;
	}
	storeItemsOn { | stream |
		var addComma = false;
		this.do { | item |
			if (stream.atLimit) { ^this };
			if (addComma) { stream.comma.space; } { addComma = true };
			item.storeOn(stream);
		};
	}
	printItemsOn { | stream |
		var addComma = false;
		this.do { | item |
			if (stream.atLimit) { ^this };
			if (addComma) { stream.comma.space; } { addComma = true };
			item.printOn(stream);
		};
	}

	// Synth support

	writeDef { | file |
		file.putString("SCgf");
		file.putInt32(2); // file version
		file.putInt16(this.size); // number of defs in file.

		this.do { | item | item.writeDef(file); }
	}

	writeInputSpec { | file, synthDef |
		this.do { | item | item.writeInputSpec(file, synthDef) };
	}

	// Flow control
	case { | default |
		var out = this.detect {|it| it.key.value;};
		if (out.notNil) {
			^out.value.value
		}{
			^default.value;
		}
	}

	// Event support
	makeEnvirValPairs {
		var res = Array.new(this.size * 2);
		this.do { |item|
			res.add(item);
			res.add(currentEnvironment[item]);
		};
		^res
	}
}
