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
		obj = this.new(size);
		size.do { | i |
			obj.add(function.value(i));
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
	
	isEmpty { ^this.size == 0 }
	notEmpty { ^this.size > 0 }
	asCollection { ^this }
	
	add { ^this.subclassResponsibility(thisMethod) }
	addAll { | aCollection | aCollection.asCollection.do { | item | this.add(item) } }
	remove { ^this.subclassResponsibility(thisMethod) }
	removeAll { | list | list.do { | item | this.remove(item) } }
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
		this.do {|elem, i| if (function.value(elem, i)) { res.add(elem) } }
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
	occurencesOf { | obj |
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

	invert { 
		// can be used to invert a pitch list about its registral center
		//  [3, 2, 9, 7].invert  becomes  [ 8, 9, 2, 4 ]
		^this.minItem + this.maxItem - this
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
	
	printAll { this.do { | item | item.postln; }; } // convenience method
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
		this.do { | item, i |
			if (stream.atLimit) { ^this };
			if (i != 0) { stream.comma.space; };
			item.storeOn(stream);
		};
	}
	printItemsOn { | stream |
		this.do { | item, i |
			if (stream.atLimit) { ^this };
			if (i != 0) { stream.comma.space; };
			item.printOn(stream);
		};
	}
	
	// Synth support
	
	writeDef { | file |
		file.putString("SCgf");
		file.putInt32(1); // file version
		file.putInt16(this.size); // number of defs in file.
		
		this.do { | item | item.writeDef(file); }
	}
	
	writeInputSpec { | file, synthDef |
		this.do { | item | item.debug.writeInputSpec(file, synthDef) };
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
